/*************************************************************************/
/*  gdprocsplitvector.cpp                                                */
/*************************************************************************/

#include "src/primitives/gdprocsplitvector.h"

void GDProcSplitVector::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_vectors"), &GDProcSplitVector::get_vector);
	ClassDB::bind_method(D_METHOD("set_vectors", "vectors"), &GDProcSplitVector::set_vector);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "vectors"), "set_vectors", "get_vectors");
}

String GDProcSplitVector::get_type_name() const {
	return RTR("Vector Split");
}

String GDProcSplitVector::get_description() const {
	return RTR("Helper node to split one vector into three reals.");
}

void GDProcSplitVector::set_vector(Vector3 vector) {
	if (default_vector != vector) {
		default_vector = vector;
		must_update = true;
		emit_signal("changed");
	}
}

Vector3 GDProcSplitVector::get_vector() const {
	return default_vector;
}

bool GDProcSplitVector::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		PoolVector<Vector3> vectors;

		int input_count = p_inputs.size();
		if (input_count > 0 && p_inputs[0].get_type() == Variant::POOL_VECTOR3_ARRAY) {
			vectors = p_inputs[0];
		}

		// check our input and add defaults where we need them
		int num_vectors = vectors.size();
		if (num_vectors == 0) {
			vectors.push_back(default_vector);
			num_vectors++;
		}

		// and lets process...
		PoolVector<Vector3>::Read rv = vectors.read();

		output_x.resize(num_vectors);
		output_y.resize(num_vectors);
		output_z.resize(num_vectors);
		PoolVector<real_t>::Write wx = output_x.write();
		PoolVector<real_t>::Write wy = output_y.write();
		PoolVector<real_t>::Write wz = output_z.write();

		for (int i = 0; i < num_vectors; i++) {
			Vector3 vector = rv[i];
			wx[i] = vector.x;
			wy[i] = vector.y;
			wz[i] = vector.z;
		}
	}

	return updated;
}

int GDProcSplitVector::get_input_connector_count() const {
	return 1;
}

Variant::Type GDProcSplitVector::get_input_connector_type(int p_slot) const {
	return Variant::POOL_VECTOR3_ARRAY;
}

String GDProcSplitVector::get_input_connector_name(int p_slot) const {
	return String("vectors");
}

String GDProcSplitVector::get_connector_property_name(int p_slot) const {
	if (p_slot == 0) {
		return String("vectors");
	}
	return String();
}

int GDProcSplitVector::get_output_connector_count() const {
	return 3;
}

Variant::Type GDProcSplitVector::get_output_connector_type(int p_slot) const {
	return Variant::POOL_REAL_ARRAY;
}

String GDProcSplitVector::get_output_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("x");
	} else if (p_slot == 1) {
		return String("y");
	} else if (p_slot == 2) {
		return String("z");
	}

	return String();
}

Variant GDProcSplitVector::get_output(int p_slot) const {
	if (p_slot == 0) {
		return output_x;
	} else if (p_slot == 1) {
		return output_y;
	} else if (p_slot == 2) {
		return output_z;
	}

	return Variant();
}

GDProcSplitVector::GDProcSplitVector() {
	default_vector = Vector3();
}
