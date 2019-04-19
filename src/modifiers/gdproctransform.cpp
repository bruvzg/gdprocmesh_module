/*************************************************************************/
/*  gdproctransform.cpp                                                  */
/*************************************************************************/

#include "src/modifiers/gdproctransform.h"

void GDProcTransform::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_rotation"), &GDProcTransform::get_rotation);
	ClassDB::bind_method(D_METHOD("set_rotation", "rotation"), &GDProcTransform::set_rotation);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "rotation"), "set_rotation", "get_rotation");

	ClassDB::bind_method(D_METHOD("get_translation"), &GDProcTransform::get_translation);
	ClassDB::bind_method(D_METHOD("set_translation", "translation"), &GDProcTransform::set_translation);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "translation"), "set_translation", "get_translation");

	ClassDB::bind_method(D_METHOD("get_scale"), &GDProcTransform::get_scale);
	ClassDB::bind_method(D_METHOD("set_scale", "scale"), &GDProcTransform::set_scale);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
}

String GDProcTransform::get_type_name() const {
	return RTR("Transform");
}

String GDProcTransform::get_description() const {
	return RTR("Transforms the surface by the given translation, rotation and scale.");
}

void GDProcTransform::set_rotation(Vector3 p_rotation) {
	if (default_rotation != p_rotation) {
		default_rotation = p_rotation;
		must_update = true;
		emit_signal("changed");
	}
}

Vector3 GDProcTransform::get_rotation() {
	return default_rotation;
}

void GDProcTransform::set_translation(Vector3 p_translation) {
	if (default_translation != p_translation) {
		default_translation = p_translation;
		must_update = true;
		emit_signal("changed");
	}
}

Vector3 GDProcTransform::get_translation() {
	return default_translation;
}

void GDProcTransform::set_scale(Vector3 p_scale) {
	if (default_scale != p_scale) {
		default_scale = p_scale;
		must_update = true;
		emit_signal("changed");
	}
}

Vector3 GDProcTransform::get_scale() {
	return default_scale;
}

bool GDProcTransform::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int num_vectors = 0;
		PoolVector<Vector3> input_vectors;
		int num_normals = 0;
		PoolVector<Vector3> input_normals;
		int num_tangents = 0;
		PoolVector<real_t> input_tangents;
		int num_rotations = 0;
		PoolVector<Color> rotations;
		int num_translations = 0;
		PoolVector<Vector3> translations;
		int num_scales = 0;
		PoolVector<Vector3> scales;

		// resize our array just in case
		surface_arr.resize(ArrayMesh::ARRAY_MAX);

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::ARRAY) {
				// get our surface
				Array surface = p_inputs[0];
				if (surface.size() == ArrayMesh::ARRAY_MAX) {
					// we're going to process our vectors
					input_vectors = surface[ArrayMesh::ARRAY_VERTEX];
					num_vectors = input_vectors.size();
					input_normals = surface[ArrayMesh::ARRAY_NORMAL];
					num_normals = input_normals.size();
					input_tangents = surface[ArrayMesh::ARRAY_TANGENT];
					num_tangents = input_tangents.size();

					// and just copy the rest
					surface_arr[ArrayMesh::ARRAY_COLOR] = surface[ArrayMesh::ARRAY_COLOR];
					surface_arr[ArrayMesh::ARRAY_TEX_UV] = surface[ArrayMesh::ARRAY_TEX_UV];
					surface_arr[ArrayMesh::ARRAY_TEX_UV2] = surface[ArrayMesh::ARRAY_TEX_UV2];
					surface_arr[ArrayMesh::ARRAY_BONES] = surface[ArrayMesh::ARRAY_BONES];
					surface_arr[ArrayMesh::ARRAY_WEIGHTS] = surface[ArrayMesh::ARRAY_WEIGHTS];
					surface_arr[ArrayMesh::ARRAY_INDEX] = surface[ArrayMesh::ARRAY_INDEX];
				}
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_COLOR_ARRAY) {
				rotations = p_inputs[1];
				num_rotations = rotations.size();
			}
		}
		if (input_count > 2) {
			if (p_inputs[2].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				translations = p_inputs[2];
				num_translations = translations.size();
			}
		}
		if (input_count > 3) {
			if (p_inputs[3].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				scales = p_inputs[3];
				num_scales = scales.size();
			}
		}

		if (num_rotations == 0) {
			// our default rotation contains euler angles, but we're using quaternions
			Quat q;
			float pi_180 = Math_PI / 180.0f;
			q.set_euler_xyz(default_rotation * Vector3(pi_180, pi_180, pi_180));
			rotations.push_back(Color(q.x, q.y, q.z, q.w)); // quaternions should be normalized so w should be sqrt(1.0 - x2 - y2 - z2)
			num_rotations++;
		}

		if (num_translations == 0) {
			translations.push_back(default_translation);
			num_translations++;
		}

		if (num_scales == 0) {
			scales.push_back(default_scale);
			num_scales++;
		}

		// Convert my quaternion array to basis, xform on quaternion seems broken
		PoolVector<Color>::Read q = rotations.read();
		Basis *rots = (Basis *)memalloc(sizeof(Basis) * num_rotations);
		for (int i = 0; i < num_rotations; i++) {
			Color rot = q[i];
			Quat quat(rot.r, rot.g, rot.b, rot.a);

			// convert to basis, I had problems with quat.xform...
			rots[i] = Basis(quat);
		}

		// Lock our inputs for reading
		PoolVector<Vector3>::Read t = translations.read();
		PoolVector<Vector3>::Read s = scales.read();

		if (num_vectors > 0) {
			PoolVector<Vector3> vectors;
			vectors.resize(num_vectors);

			PoolVector<Vector3>::Write w = vectors.write();
			PoolVector<Vector3>::Read r = input_vectors.read();

			for (int i = 0; i < num_vectors; i++) {
				w[i] = (rots[i % num_rotations].xform(r[i]) * s[i % num_scales]) + t[i % num_translations];
			}

			surface_arr[ArrayMesh::ARRAY_VERTEX] = vectors;
		}

		if (num_normals > 0) {
			PoolVector<Vector3> normals;
			normals.resize(num_normals);

			PoolVector<Vector3>::Write w = normals.write();
			PoolVector<Vector3>::Read r = input_normals.read();

			for (int i = 0; i < num_normals; i++) {
				w[i] = rots[i % num_rotations].xform(r[i]);
			}

			surface_arr[ArrayMesh::ARRAY_NORMAL] = normals;
		}

		if (num_tangents > 0) {
			int j = 0;
			PoolVector<real_t> tangents;
			tangents.resize(num_tangents);

			PoolVector<real_t>::Write w = tangents.write();
			PoolVector<real_t>::Read r = input_tangents.read();

			for (int i = 0; i < num_tangents; i += 4) {
				Vector3 v(r[i], r[i + 1], r[i + 2]);
				v = rots[j % num_rotations].xform(v);
				w[i] = v.x;
				w[i + 1] = v.y;
				w[i + 2] = v.z;
				w[i + 3] = r[i + 3]; // no need to update this...
				j++;
			}

			surface_arr[ArrayMesh::ARRAY_TANGENT] = tangents;
		}

		// free
		memfree(rots);
	}

	return updated;
}

int GDProcTransform::get_input_connector_count() const {
	return 4;
}

Variant::Type GDProcTransform::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::ARRAY;
	} else if (p_slot == 1) {
		// we don't have a POOL_QUAD_ARRAY, abusing color for storing
		return Variant::POOL_COLOR_ARRAY;
	} else if (p_slot == 2) {
		return Variant::POOL_VECTOR3_ARRAY;
	} else if (p_slot == 3) {
		return Variant::POOL_VECTOR3_ARRAY;
	}

	return Variant::NIL;
}

String GDProcTransform::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("surface");
	} else if (p_slot == 1) {
		return String("rotations");
	} else if (p_slot == 2) {
		return String("translations");
	} else if (p_slot == 3) {
		return String("scales");
	}

	return String();
}

String GDProcTransform::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("rotation");
	} else if (p_slot == 2) {
		return String("translation");
	} else if (p_slot == 3) {
		return String("scale");
	}

	return String();
}

int GDProcTransform::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcTransform::get_output_connector_type(int p_slot) const {
	return Variant::ARRAY;
}

String GDProcTransform::get_output_connector_name(int p_slot) const {
	return String("surface");
}

Variant GDProcTransform::get_output(int p_slot) const {
	return Variant(surface_arr);
}

GDProcTransform::GDProcTransform() {
	default_rotation = Vector3(0.0, 0.0, 0.0);
	default_translation = Vector3(0.0, 0.0, 0.0);
	default_scale = Vector3(1.0, 1.0, 1.0);
}
