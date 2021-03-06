/*************************************************************************/
/*  gdprocsub.cpp                                                        */
/*************************************************************************/

#include "src/transforms/gdprocsub.h"

void GDProcSub::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_subtract"), &GDProcSub::get_subtract);
	ClassDB::bind_method(D_METHOD("set_subtract", "subtract"), &GDProcSub::set_subtract);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "subtract"), "set_subtract", "get_subtract");
}

String GDProcSub::get_type_name() const {
	return RTR_LOCAL("Subtract");
}

String GDProcSub::get_description() const {
	return RTR_LOCAL("Subtract all reals in input by value.");
}

void GDProcSub::set_subtract(float p_subtract) {
	if (default_sub != p_subtract) {
		default_sub = p_subtract;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcSub::get_subtract() {
	return default_sub;
}

bool GDProcSub::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int num_values = 0;
		PoolVector<real_t> input_values;
		int num_subs = 0;
		PoolVector<real_t> subs;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_REAL_ARRAY) {
				input_values = p_inputs[0];
				num_values = input_values.size();
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_REAL_ARRAY) {
				subs = p_inputs[1];
				num_subs = subs.size();
			}
		}

		if (num_subs == 0) {
			subs.push_back(default_sub);
			num_subs++;
		}

		if (num_values > 0) {
			int new_size = num_values > num_subs ? num_values : num_subs;
			values.resize(new_size);

			PoolVector<real_t>::Write w = values.write();
			PoolVector<real_t>::Read r = input_values.read();
			PoolVector<real_t>::Read s = subs.read();

			for (int i = 0; i < new_size; i++) {
				w[i] = r[i % num_values] - s[i % num_subs];
			}

		} else {
			values.resize(0);
		}
	}

	return updated;
}

int GDProcSub::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcSub::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_REAL_ARRAY;
	} else {
		return Variant::POOL_REAL_ARRAY;
	}
}

String GDProcSub::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("values");
	} else if (p_slot == 1) {
		return String("subtract");
	}

	return String();
}

String GDProcSub::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("subtract");
	}

	return String();
}

int GDProcSub::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcSub::get_output_connector_type(int p_slot) const {
	return Variant::POOL_REAL_ARRAY;
}

String GDProcSub::get_output_connector_name(int p_slot) const {
	return String("values");
}

Variant GDProcSub::get_output(int p_slot) const {
	return Variant(values);
}

GDProcSub::GDProcSub() {
	// default values
	values.resize(1);
	values.set(0, 0.0f);
	default_sub = 1.0f;
}
