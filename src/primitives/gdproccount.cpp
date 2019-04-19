/*************************************************************************/
/*  gdproccount.cpp                                                      */
/*************************************************************************/

#include "src/primitives/gdproccount.h"

void GDProcCount::_bind_methods() {
}

String GDProcCount::get_type_name() const {
	return RTR("Count");
}

String GDProcCount::get_description() const {
	return RTR("Returns the amount of elements in the input.");
}

bool GDProcCount::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int count = 0;

		int input_count = p_inputs.size();
		for (int i = 0; i < input_count; i++) {
			switch (p_inputs[i].get_type()) {
				case Variant::POOL_INT_ARRAY: {
					PoolVector<int> input = p_inputs[i];
					count += input.size();
				}; break;
				case Variant::POOL_REAL_ARRAY: {
					PoolVector<real_t> input = p_inputs[i];
					count += input.size();
				}; break;
				case Variant::POOL_VECTOR2_ARRAY: {
					PoolVector<Vector2> input = p_inputs[i];
					count += input.size();
				}; break;
				case Variant::POOL_VECTOR3_ARRAY: {
					PoolVector<Vector3> input = p_inputs[i];
					count += input.size();
				}; break;
				case Variant::POOL_COLOR_ARRAY: {
					PoolVector<Color> input = p_inputs[i];
					count += input.size();
				}; break;
				case Variant::ARRAY: {
					Array input = p_inputs[i];
					count += input.size();
				}; break;
				default:
					count++; // assume contains one
					WARN_PRINTS(RTR("Can't count this input"));
					break;
			}
		}

		//printf("Counted %i elements\n", count);
		value.set(0, count);
	}

	return updated;
}

int GDProcCount::get_input_connector_count() const {
	return 1;
}

Variant::Type GDProcCount::get_input_connector_type(int p_slot) const {
	///@TODO we should find a way to properly mark that we will accept anything
	return Variant::POOL_VECTOR3_ARRAY;
}

String GDProcCount::get_input_connector_name(int p_slot) const {
	return String("any");
}

int GDProcCount::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcCount::get_output_connector_type(int p_slot) const {
	return Variant::POOL_INT_ARRAY;
}

String GDProcCount::get_output_connector_name(int p_slot) const {
	return String("count");
}

Variant GDProcCount::get_output(int p_slot) const {
	return Variant(value);
}

GDProcCount::GDProcCount() {
	// init
	value.resize(1);
	value.set(0, 0);
}
