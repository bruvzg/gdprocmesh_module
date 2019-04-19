/*************************************************************************/
/*  gdprocscale.cpp                                                      */
/*************************************************************************/

#include "src/transforms/gdprocscale.h"

void GDProcScale::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_scale"), &GDProcScale::get_scale);
	ClassDB::bind_method(D_METHOD("set_scale", "scale"), &GDProcScale::set_scale);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"), "set_scale", "get_scale");
}

String GDProcScale::get_type_name() const {
	return RTR_LOCAL("Scale");
}

String GDProcScale::get_description() const {
	return RTR_LOCAL("Multiplies together vectors from translation and vectors. In other words:\noutput[i] = vectors[i % vectors.size()] * scale[i % scale.size()]");
}

void GDProcScale::set_scale(Vector3 p_scale) {
	if (default_scale != p_scale) {
		default_scale = p_scale;
		must_update = true;
		emit_signal("changed");
	}
}

Vector3 GDProcScale::get_scale() {
	return default_scale;
}

bool GDProcScale::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int num_scales = 0;
		PoolVector<Vector3> scales;
		int num_vectors = 0;
		PoolVector<Vector3> input_vectors;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				input_vectors = p_inputs[0];
				num_vectors = input_vectors.size();
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				scales = p_inputs[1];
				num_scales = scales.size();
			}
		}

		if (num_scales == 0) {
			scales.push_back(default_scale);
			num_scales++;
		}

		if (num_vectors > 0) {
			int new_size = num_vectors > num_scales ? num_vectors : num_scales;
			vectors.resize(new_size);

			PoolVector<Vector3>::Write w = vectors.write();
			PoolVector<Vector3>::Read r = input_vectors.read();
			PoolVector<Vector3>::Read s = scales.read();

			for (int i = 0; i < new_size; i++) {
				w[i] = r[i % num_vectors] * s[i % num_scales];
			}

		} else {
			vectors.resize(0);
		}
	}

	return updated;
}

int GDProcScale::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcScale::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_VECTOR3_ARRAY;
	} else {
		return Variant::POOL_VECTOR3_ARRAY;
	}
}

String GDProcScale::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("vectors");
	} else if (p_slot == 1) {
		return String("scale");
	}

	return String();
}

String GDProcScale::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("scale");
	}

	return String();
}

int GDProcScale::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcScale::get_output_connector_type(int p_slot) const {
	return Variant::POOL_VECTOR3_ARRAY;
}

String GDProcScale::get_output_connector_name(int p_slot) const {
	return String("vectors");
}

Variant GDProcScale::get_output(int p_slot) const {
	return Variant(vectors);
}

GDProcScale::GDProcScale() {
	default_scale = Vector3(1.0, 1.0, 1.0);
}
