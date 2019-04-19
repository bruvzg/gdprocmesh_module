/*************************************************************************/
/*  gdprocrotate.cpp                                                     */
/*************************************************************************/

#include "src/transforms/gdprocrotate.h"

void GDProcRotate::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_rotation"), &GDProcRotate::get_rotation);
	ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), &GDProcRotate::set_rotation);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "rotation"), "set_rotation", "get_rotation");
}

String GDProcRotate::get_type_name() const {
	return RTR("Rotate");
}

String GDProcRotate::get_description() const {
	return RTR("Rotates vectors from input by rotation.");
}

void GDProcRotate::set_rotation(Vector3 p_rotation) {
	if (default_rotation != p_rotation) {
		default_rotation = p_rotation;
		must_update = true;
		emit_signal("changed");
	}
}

Vector3 GDProcRotate::get_rotation() {
	return default_rotation;
}

bool GDProcRotate::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int num_vectors = 0;
		PoolVector<Vector3> input_vectors;
		int num_rotations = 0;
		PoolVector<Color> rotations;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				input_vectors = p_inputs[0];
				num_vectors = input_vectors.size();
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_COLOR_ARRAY) {
				rotations = p_inputs[1];
				num_rotations = rotations.size();
			}
		}

		if (num_rotations == 0) {
			// our default rotation contains euler angles, but we're using quaternions
			Quat q;
			float pi_180 = Math_PI / 180.0f;
			q.set_euler_xyz(default_rotation * Vector3(pi_180, pi_180, pi_180));
			rotations.push_back(Color(q.x, q.y, q.z, q.w));
			num_rotations++;
		}

		if (num_vectors > 0) {
			int new_size = num_vectors > num_rotations ? num_vectors : num_rotations;
			vectors.resize(new_size);

			// Convert quaternions to basis, some weird issues using quaternions directly
			PoolVector<Color>::Read q = rotations.read();
			Basis *rots = (Basis *)memalloc(sizeof(Basis) * num_rotations);
			for (int i = 0; i < num_rotations; i++) {
				Color rot = q[i];
				Quat quat(rot.r, rot.g, rot.b, rot.a);

				// convert to basis, I had problems with quat.xform...
				rots[i] = Basis(quat);
			}

			PoolVector<Vector3>::Write w = vectors.write();
			PoolVector<Vector3>::Read r = input_vectors.read();

			for (int i = 0; i < new_size; i++) {
				w[i] = rots[i % num_rotations].xform(r[i % num_vectors]);
			}

			// free
			memfree(rots);
		} else {
			vectors.resize(0);
		}
	}

	return updated;
}

int GDProcRotate::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcRotate::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_VECTOR3_ARRAY;
	} else if (p_slot == 1) {
		return Variant::POOL_COLOR_ARRAY;
	} else {
		return Variant::NIL;
	}
}

String GDProcRotate::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("vectors");
	} else if (p_slot == 1) {
		return String("rotation");
	}

	return String();
}

String GDProcRotate::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("rotation");
	}

	return String();
}

int GDProcRotate::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcRotate::get_output_connector_type(int p_slot) const {
	return Variant::POOL_VECTOR3_ARRAY;
}

String GDProcRotate::get_output_connector_name(int p_slot) const {
	return String("vectors");
}

Variant GDProcRotate::get_output(int p_slot) const {
	return Variant(vectors);
}

GDProcRotate::GDProcRotate() {
	default_rotation = Vector3(0.0, 0.0, 0.0);
}
