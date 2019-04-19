/*************************************************************************/
/*  gdprocmult.cpp                                                       */
/*************************************************************************/

#include "src/transforms/gdprocmult.h"

void GDProcMult::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_mult"), &GDProcMult::get_mult);
	ClassDB::bind_method(D_METHOD("set_mult", "mult"), &GDProcMult::set_mult);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "mult"), "set_mult", "get_mult");
}

String GDProcMult::get_type_name() const {
	return RTR("Multiply");
}

String GDProcMult::get_description() const {
	return RTR("Multiplies all reals in input by mult.");
}

void GDProcMult::set_mult(float p_mult) {
	if (default_mult != p_mult) {
		default_mult = p_mult;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcMult::get_mult() {
	return default_mult;
}

bool GDProcMult::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int num_values = 0;
		PoolVector<real_t> input_values;
		int num_mults = 0;
		PoolVector<real_t> mults;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_REAL_ARRAY) {
				input_values = p_inputs[0];
				num_values = input_values.size();
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_REAL_ARRAY) {
				mults = p_inputs[1];
				num_mults = mults.size();
			}
		}

		if (num_mults == 0) {
			mults.push_back(default_mult);
			num_mults++;
		}

		if (num_values > 0) {
			int new_size = num_values > num_mults ? num_values : num_mults;
			values.resize(new_size);

			PoolVector<real_t>::Write w = values.write();
			PoolVector<real_t>::Read r = input_values.read();
			PoolVector<real_t>::Read m = mults.read();

			for (int i = 0; i < new_size; i++) {
				w[i] = r[i % num_values] * m[i % num_mults];
			}

		} else {
			values.resize(0);
		}
	}

	return updated;
}

int GDProcMult::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcMult::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_REAL_ARRAY;
	} else {
		return Variant::POOL_REAL_ARRAY;
	}
}

String GDProcMult::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("values");
	} else if (p_slot == 1) {
		return String("multiply");
	}

	return String();
}

String GDProcMult::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("mult");
	}

	return String();
}

int GDProcMult::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcMult::get_output_connector_type(int p_slot) const {
	return Variant::POOL_REAL_ARRAY;
}

String GDProcMult::get_output_connector_name(int p_slot) const {
	return String("values");
}

Variant GDProcMult::get_output(int p_slot) const {
	return Variant(values);
}

GDProcMult::GDProcMult() {
	// default values
	values.resize(1);
	values.set(0, 0.0f);
	default_mult = 1.0f;
}
