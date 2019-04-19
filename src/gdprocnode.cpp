/*************************************************************************/
/*  gdprocnode.cpp                                                       */
/*************************************************************************/

#include "src/gdprocnode.h"

void GDProcNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_type_name"), &GDProcNode::get_type_name);
	ClassDB::bind_method(D_METHOD("get_description"), &GDProcNode::get_description);
	ClassDB::bind_method(D_METHOD("_touch"), &GDProcNode::_touch);

	ClassDB::bind_method(D_METHOD("get_must_update"), &GDProcNode::get_must_update);
	ClassDB::bind_method(D_METHOD("update", "inputs_updated", "inputs"), &GDProcNode::update);

	// position
	ClassDB::bind_method(D_METHOD("get_position"), &GDProcNode::get_position);
	ClassDB::bind_method(D_METHOD("set_position", "pos"), &GDProcNode::set_position);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position"), "set_position", "get_position");

	// node name
	ClassDB::bind_method(D_METHOD("get_node_name"), &GDProcNode::get_node_name);
	ClassDB::bind_method(D_METHOD("set_node_name", "node_name"), &GDProcNode::set_node_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "node_name"), "set_node_name", "get_node_name");

	// hidden input
	ClassDB::bind_method(D_METHOD("get_hidden_input"), &GDProcNode::get_hidden_input);
	ClassDB::bind_method(D_METHOD("set_hidden_input", "hidden_input"), &GDProcNode::set_hidden_input);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hidden_input"), "set_hidden_input", "get_hidden_input");

	ClassDB::bind_method(D_METHOD("get_input_property_type"), &GDProcNode::get_input_property_type);

	// connectors
	ClassDB::bind_method(D_METHOD("get_input_connector_count"), &GDProcNode::get_input_connector_count);
	ClassDB::bind_method(D_METHOD("get_input_connector_type", "slot"), &GDProcNode::get_input_connector_type);
	ClassDB::bind_method(D_METHOD("get_input_connector_name", "slot"), &GDProcNode::get_input_connector_name);

	ClassDB::bind_method(D_METHOD("get_connector_property_name", "slot"), &GDProcNode::get_connector_property_name);

	ClassDB::bind_method(D_METHOD("get_output_connector_count"), &GDProcNode::get_output_connector_count);
	ClassDB::bind_method(D_METHOD("get_output_connector_type", "slot"), &GDProcNode::get_output_connector_type);
	ClassDB::bind_method(D_METHOD("get_output_connector_name", "slot"), &GDProcNode::get_output_connector_name);

	// and a special signal for node name changes
	ADD_SIGNAL(MethodInfo("node_name_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::STRING, "from"), PropertyInfo(Variant::STRING, "to")));

	BIND_ENUM_CONSTANT(PROCESS_STATUS_PENDING);
	BIND_ENUM_CONSTANT(PROCESS_STATUS_INPROGRESS);
	BIND_ENUM_CONSTANT(PROCESS_STATUS_UNCHANGED);
	BIND_ENUM_CONSTANT(PROCESS_STATUS_CHANGED);
}

GDProcNode::ProcessStatus GDProcNode::get_status() const {
	return status;
}

void GDProcNode::set_status(GDProcNode::ProcessStatus p_status) {
	status = p_status;
}

String GDProcNode::get_type_name() const {
	return RTR("Node");
}

String GDProcNode::get_description() const {
	return RTR("No Description Available");
}

void GDProcNode::set_node_name(const String &p_node_name) {
	if (node_name != p_node_name) {
		String was_name = node_name;

		// change the name
		node_name = p_node_name;

		// emit name change signal
		emit_signal("node_name_changed", this, was_name, node_name);

		// emit changed signal
		emit_signal("changed");
	}
}

String GDProcNode::get_node_name() const {
	return node_name;
}

void GDProcNode::_touch() {
	must_update = true;
}

bool GDProcNode::get_must_update() {
	return must_update;
}

bool GDProcNode::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		// just an example here, but implement updating data here..
	}

	return updated;
}

Variant::Type GDProcNode::get_input_property_type() const {
	return Variant::NIL;
}

String GDProcNode::get_input_property_hint() const {
	return String();
}

void GDProcNode::set_input(Variant p_input) {
	// nothing to do here
}

Variant GDProcNode::get_input() {
	return Variant();
}

void GDProcNode::set_hidden_input(bool p_set) {
	hidden_input = p_set;
}

bool GDProcNode::get_hidden_input() const {
	return hidden_input;
}

int GDProcNode::get_input_connector_count() const {
	return 0;
}

Variant::Type GDProcNode::get_input_connector_type(int p_slot) const {
	return Variant::NIL;
}

String GDProcNode::get_input_connector_name(int p_slot) const {
	return String();
}

String GDProcNode::get_connector_property_name(int p_slot) const {
	return String();
}

int GDProcNode::get_output_connector_count() const {
	// we should always have one output unless this is our final node
	return 1;
}

Variant::Type GDProcNode::get_output_connector_type(int p_slot) const {
	return Variant::NIL;
}

String GDProcNode::get_output_connector_name(int p_slot) const {
	return String("default");
}

Variant GDProcNode::get_output(int p_slot) const {
	return Variant();
}

Vector2 GDProcNode::get_position() const {
	return position;
}

void GDProcNode::set_position(Vector2 p_pos) {
	if (position != p_pos) {
		position = p_pos;

		// probably should send signal
	}
}

GDProcNode::GDProcNode() {
	must_update = true;
	position = Vector2(0.0, 0.0);
	node_name = "";
	hidden_input = false;
}
