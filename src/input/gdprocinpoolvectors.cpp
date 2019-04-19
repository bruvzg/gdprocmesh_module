/*************************************************************************/
/*  gdprocinpoolvectors.cpp                                              */
/*************************************************************************/

#include "src/input/gdprocinpoolvectors.h"

void GDProcInPoolVectors::_bind_methods() {
}

String GDProcInPoolVectors::get_type_name() const {
	return RTR("Input Vectors");
}

String GDProcInPoolVectors::get_description() const {
	return RTR("Allows you to specify an array of vectors as input for the procedural mesh.\nCan be set from the editor.");
}

void GDProcInPoolVectors::set_input(Variant p_input) {
	// assume changed, maybe do something at some point to check if it actually has..
	value = p_input;
	must_update = true;
	emit_signal("changed");
}

Variant GDProcInPoolVectors::get_input() {
	return Variant(value);
}

bool GDProcInPoolVectors::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	return updated;
}

Variant::Type GDProcInPoolVectors::get_input_property_type() const {
	return Variant::POOL_VECTOR3_ARRAY;
}

int GDProcInPoolVectors::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcInPoolVectors::get_output_connector_type(int p_slot) const {
	return Variant::POOL_VECTOR3_ARRAY;
}

String GDProcInPoolVectors::get_output_connector_name(int p_slot) const {
	return String("vectors");
}

Variant GDProcInPoolVectors::get_output(int p_slot) const {
	return Variant(value);
}

GDProcInPoolVectors::GDProcInPoolVectors() {
	// add some defaults
	value.push_back(Vector3(0.0, 0.0, 0.0));
	value.push_back(Vector3(0.0, 0.0, 1.0));
}
