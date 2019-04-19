/*************************************************************************/
/*  gdprocinint.cpp                                                      */
/*************************************************************************/

#include "src/input/gdprocinint.h"

void GDProcInInt::_bind_methods() {
}

String GDProcInInt::get_type_name() const {
	return RTR_LOCAL("Input Int");
}

String GDProcInInt::get_description() const {
	return RTR_LOCAL("Allows you to specify an integer as input for the procedural mesh.\nCan be set from the editor.");
}

void GDProcInInt::set_input(Variant p_input) {
	int new_value = p_input;
	if (value[0] != new_value) {
		value.set(0, new_value);
		must_update = true;
		emit_signal("changed");
	}
}

Variant GDProcInInt::get_input() {
	return Variant(value[0]);
}

bool GDProcInInt::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	return updated;
}

Variant::Type GDProcInInt::get_input_property_type() const {
	return Variant::INT;
}

int GDProcInInt::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcInInt::get_output_connector_type(int p_slot) const {
	return Variant::POOL_INT_ARRAY;
}

String GDProcInInt::get_output_connector_name(int p_slot) const {
	return String("value");
}

Variant GDProcInInt::get_output(int p_slot) const {
	return Variant(value);
}

GDProcInInt::GDProcInInt() {
	// init our buffer
	value.resize(1);
	value.set(0, 0);
}
