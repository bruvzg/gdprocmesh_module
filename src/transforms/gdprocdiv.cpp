/*************************************************************************/
/*  gdprocdiv.cpp                                                        */
/*************************************************************************/

#include "src/transforms/gdprocdiv.h"

void GDProcDiv::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_divider"), &GDProcDiv::get_divider);
	ClassDB::bind_method(D_METHOD("set_divider", "divider"), &GDProcDiv::set_divider);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "divider"), "set_divider", "get_divider");
}

String GDProcDiv::get_type_name() const {
	return RTR_LOCAL("Division");
}

String GDProcDiv::get_description() const {
	return RTR_LOCAL("Divides all reals in input by divider.");
}

void GDProcDiv::set_divider(float p_divider) {
	if (default_div != p_divider) {
		default_div = p_divider;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcDiv::get_divider() {
	return default_div;
}

bool GDProcDiv::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int num_values = 0;
		PoolVector<real_t> input_values;
		int num_divs = 0;
		PoolVector<real_t> divs;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_REAL_ARRAY) {
				input_values = p_inputs[0];
				num_values = input_values.size();
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_REAL_ARRAY) {
				divs = p_inputs[1];
				num_divs = divs.size();
			}
		}

		if (num_divs == 0) {
			divs.push_back(default_div);
			num_divs++;
		}

		if (num_values > 0) {
			int new_size = num_values > num_divs ? num_values : num_divs;
			values.resize(new_size);

			PoolVector<real_t>::Write w = values.write();
			PoolVector<real_t>::Read r = input_values.read();
			PoolVector<real_t>::Read m = divs.read();

			for (int i = 0; i < new_size; i++) {
				w[i] = r[i % num_values] / m[i % num_divs];
			}

		} else {
			values.resize(0);
		}
	}

	return updated;
}

int GDProcDiv::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcDiv::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_REAL_ARRAY;
	} else {
		return Variant::POOL_REAL_ARRAY;
	}
}

String GDProcDiv::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("values");
	} else if (p_slot == 1) {
		return String("divider");
	}

	return String();
}

String GDProcDiv::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("divider");
	}

	return String();
}

int GDProcDiv::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcDiv::get_output_connector_type(int p_slot) const {
	return Variant::POOL_REAL_ARRAY;
}

String GDProcDiv::get_output_connector_name(int p_slot) const {
	return String("values");
}

Variant GDProcDiv::get_output(int p_slot) const {
	return Variant(values);
}

GDProcDiv::GDProcDiv() {
	// default values
	values.resize(1);
	values.set(0, 0.0f);
	default_div = 1.0f;
}
