/*************************************************************************/
/*  gdprocnode.cpp                                                       */
/*************************************************************************/

#include <core/script_language.h>

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
	ClassDB::bind_method(D_METHOD("get_input_property_hint"), &GDProcNode::get_input_property_hint);

	ClassDB::bind_method(D_METHOD("get_input"), &GDProcNode::get_input);
	ClassDB::bind_method(D_METHOD("set_input", "input"), &GDProcNode::set_input);

	// connectors
	ClassDB::bind_method(D_METHOD("get_input_connector_count"), &GDProcNode::get_input_connector_count);
	ClassDB::bind_method(D_METHOD("get_input_connector_type", "slot"), &GDProcNode::get_input_connector_type);
	ClassDB::bind_method(D_METHOD("get_input_connector_name", "slot"), &GDProcNode::get_input_connector_name);

	ClassDB::bind_method(D_METHOD("get_connector_property_name", "slot"), &GDProcNode::get_connector_property_name);

	ClassDB::bind_method(D_METHOD("get_output_connector_count"), &GDProcNode::get_output_connector_count);
	ClassDB::bind_method(D_METHOD("get_output_connector_type", "slot"), &GDProcNode::get_output_connector_type);
	ClassDB::bind_method(D_METHOD("get_output_connector_name", "slot"), &GDProcNode::get_output_connector_name);

	ClassDB::bind_method(D_METHOD("get_output", "slot"), &GDProcNode::get_output);

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
	if (get_script_instance() && get_script_instance()->has_method("_get_type_name")) {
		//printf("scr call _get_type_name\n");
		Variant::CallError ce;
		return get_script_instance()->call("_get_type_name", NULL, 0, ce);
	} else {
		return RTR_LOCAL("Node");
	}
}

String GDProcNode::get_description() const {
	if (get_script_instance() && get_script_instance()->has_method("_get_description")) {
		//printf("scr call _get_description\n");
		Variant::CallError ce;
		return get_script_instance()->call("_get_description", NULL, 0, ce);
	} else {
		return RTR_LOCAL("No Description Available");
	}
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
		if (get_script_instance() && get_script_instance()->has_method("_update")) {
			//printf("scr call _update\n");
			Variant::CallError ce;
			Variant inputs = Variant(p_inputs);
			Variant *argptrs[1] = {
				&inputs
			};
			return get_script_instance()->call("_update", (const Variant **)&argptrs, 1, ce);
		}
	}

	return updated;
}

Variant::Type GDProcNode::get_input_property_type() const {
	if (get_script_instance() && get_script_instance()->has_method("_get_input_property_type")) {
		//printf("scr call _get_input_property_type\n");
		Variant::CallError ce;
		int64_t v_type = get_script_instance()->call("_get_input_property_type", NULL, 0, ce);
		return (Variant::Type)(v_type);
	} else {
		return Variant::NIL;
	}
}

String GDProcNode::get_input_property_hint() const {
	if (get_script_instance() && get_script_instance()->has_method("_get_input_property_hint")) {
		//printf("scr call _get_input_property_hint\n");
		Variant::CallError ce;
		return get_script_instance()->call("_get_input_property_hint", NULL, 0, ce);
	} else {
		return String();
	}
}

void GDProcNode::set_input(Variant p_input) {
	if (get_script_instance() && get_script_instance()->has_method("_set_input")) {
		//printf("scr call _set_input\n");
		Variant::CallError ce;
		Variant *argptrs[1] = {
			&p_input
		};
		get_script_instance()->call("_set_input", (const Variant **)&argptrs, 1, ce);
	}
}

Variant GDProcNode::get_input() {
	if (get_script_instance() && get_script_instance()->has_method("_get_input")) {
		//printf("scr call _get_input\n");
		Variant::CallError ce;
		return get_script_instance()->call("_get_input", NULL, 0, ce);
	} else {
		return Variant();
	}
}

void GDProcNode::set_hidden_input(bool p_set) {
	hidden_input = p_set;
}

bool GDProcNode::get_hidden_input() const {
	return hidden_input;
}

int GDProcNode::get_input_connector_count() const {
	if (get_script_instance() && get_script_instance()->has_method("_get_input_connector_count")) {
		//printf("scr call _get_input_connector_count\n");
		Variant::CallError ce;
		return get_script_instance()->call("_get_input_connector_count", NULL, 0, ce);
	} else {
		return 0;
	}
}

Variant::Type GDProcNode::get_input_connector_type(int p_slot) const {
	if (get_script_instance() && get_script_instance()->has_method("_get_input_connector_type")) {
		//printf("scr call _get_input_connector_type\n");
		Variant::CallError ce;
		Variant slot = Variant(p_slot);
		Variant *argptrs[1] = {
			&slot
		};
		int64_t v_type = get_script_instance()->call("_get_input_connector_type", (const Variant **)&argptrs, 1, ce);
		return (Variant::Type)(v_type);
	} else {
		return Variant::NIL;
	}
}

String GDProcNode::get_input_connector_name(int p_slot) const {
	if (get_script_instance() && get_script_instance()->has_method("_get_input_connector_name")) {
		//printf("scr call _get_input_connector_name\n");
		Variant::CallError ce;
		Variant slot = Variant(p_slot);
		Variant *argptrs[1] = {
			&slot
		};
		return get_script_instance()->call("_get_input_connector_name", (const Variant **)&argptrs, 1, ce);
	} else {
		return String();
	}
}

String GDProcNode::get_connector_property_name(int p_slot) const {
	if (get_script_instance() && get_script_instance()->has_method("_get_connector_property_name")) {
		//printf("scr call _get_connector_property_name\n");
		Variant::CallError ce;
		Variant slot = Variant(p_slot);
		Variant *argptrs[1] = {
			&slot
		};
		return get_script_instance()->call("_get_connector_property_name", (const Variant **)&argptrs, 1, ce);
	} else {
		return String();
	}
}

int GDProcNode::get_output_connector_count() const {
	// we should always have one output unless this is our final node
	if (get_script_instance() && get_script_instance()->has_method("_get_output_connector_count")) {
		//printf("scr call _get_output_connector_count\n");
		Variant::CallError ce;
		return get_script_instance()->call("_get_output_connector_count", NULL, 0, ce);
	} else {
		return 1;
	}
}

Variant::Type GDProcNode::get_output_connector_type(int p_slot) const {
	if (get_script_instance() && get_script_instance()->has_method("_get_output_connector_type")) {
		//printf("scr call _get_output_connector_type\n");
		Variant::CallError ce;
		Variant slot = Variant(p_slot);
		Variant *argptrs[1] = {
			&slot
		};
		int64_t v_type = get_script_instance()->call("_get_output_connector_type", (const Variant **)&argptrs, 1, ce);
		return (Variant::Type)(v_type);
	} else {
		return Variant::NIL;
	}
}

String GDProcNode::get_output_connector_name(int p_slot) const {
	if (get_script_instance() && get_script_instance()->has_method("_get_output_connector_name")) {
		//printf("scr call _get_output_connector_name\n");
		Variant::CallError ce;
		Variant slot = Variant(p_slot);
		Variant *argptrs[1] = {
			&slot
		};
		return get_script_instance()->call("_get_output_connector_name", (const Variant **)&argptrs, 1, ce);
	} else {
		return String("default");
	}
}

Variant GDProcNode::get_output(int p_slot) const {
	if (get_script_instance() && get_script_instance()->has_method("_get_output")) {
		//printf("scr call _get_output\n");
		Variant::CallError ce;
		Variant slot = Variant(p_slot);
		Variant *argptrs[1] = {
			&slot
		};
		return get_script_instance()->call("_get_output", (const Variant **)&argptrs, 1, ce);
	} else {
		return Variant();
	}
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
