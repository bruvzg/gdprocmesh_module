/*************************************************************************/
/*  gdprocmesheditor.cpp                                                 */
/*************************************************************************/

#include <core/io/compression.h>
#include <core/io/file_access_memory.h>
#include <core/io/marshalls.h>
#include <core/io/resource_loader.h>
#include <core/io/translation_loader_po.h>
#include <core/os/keyboard.h>

#include "src/editor/translations.gen.h"

#include "src/editor/gdprocmesheditor.h"
#include "src/editor/icons/icons.gen.h"

#include "modules/svg/image_loader_svg.h"

/*************************************************************************/

void EditVec3::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_text_entered_x", "new_text"), &EditVec3::_text_entered_x);
	ClassDB::bind_method(D_METHOD("_focus_exited_x"), &EditVec3::_focus_exited_x);
	ClassDB::bind_method(D_METHOD("_text_entered_y", "new_text"), &EditVec3::_text_entered_y);
	ClassDB::bind_method(D_METHOD("_focus_exited_y"), &EditVec3::_focus_exited_y);
	ClassDB::bind_method(D_METHOD("_text_entered_z", "new_text"), &EditVec3::_text_entered_z);
	ClassDB::bind_method(D_METHOD("_focus_exited_z"), &EditVec3::_focus_exited_z);

	ClassDB::bind_method(D_METHOD("get_vector"), &EditVec3::get_vector);
	ClassDB::bind_method(D_METHOD("set_vector", "vector"), &EditVec3::set_vector);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "vector"), "set_vector", "get_vector");

	ADD_SIGNAL(MethodInfo("changed_vector", PropertyInfo(Variant::VECTOR3, "vector")));
}

void EditVec3::_text_entered_x(const String &p_new_text) {
	vector.x = p_new_text.to_float();
	emit_signal("changed_vector", vector);
}

void EditVec3::_focus_exited_x() {
	_text_entered_x(vec3_x_edit->get_text());
}

void EditVec3::_text_entered_y(const String &p_new_text) {
	vector.y = p_new_text.to_float();
	emit_signal("changed_vector", vector);
}

void EditVec3::_focus_exited_y() {
	_text_entered_y(vec3_y_edit->get_text());
}

void EditVec3::_text_entered_z(const String &p_new_text) {
	vector.z = p_new_text.to_float();
	emit_signal("changed_vector", vector);
}

void EditVec3::_focus_exited_z() {
	_text_entered_z(vec3_z_edit->get_text());
}

Vector3 EditVec3::get_vector() const {
	vector.x = vec3_x_edit->get_text().to_float();
	vector.y = vec3_y_edit->get_text().to_float();
	vector.z = vec3_z_edit->get_text().to_float();
	return vector;
}

void EditVec3::set_vector(const Vector3 &p_vector) {
	vector = p_vector;
	vec3_x_edit->set_text(String::num_real(vector.x));
	vec3_y_edit->set_text(String::num_real(vector.y));
	vec3_z_edit->set_text(String::num_real(vector.z));
}

EditVec3::EditVec3() {

	vector = Vector3(0.0, 0.0, 0.0);

	vec3_x_edit = memnew(LineEdit);
	vec3_x_edit->connect("focus_exited", this, "_focus_exited_x");
	vec3_x_edit->connect("text_entered", this, "_text_entered_x");
	vec3_x_edit->set_expand_to_text_length(true);
	vec3_x_edit->set_custom_minimum_size(Vector2(50.0, 0.0));
	add_child(vec3_x_edit);

	vec3_y_edit = memnew(LineEdit);
	vec3_y_edit->connect("focus_exited", this, "_focus_exited_Y");
	vec3_y_edit->connect("text_entered", this, "_text_entered_y");
	vec3_y_edit->set_expand_to_text_length(true);
	vec3_y_edit->set_custom_minimum_size(Vector2(50.0, 0.0));
	add_child(vec3_y_edit);

	vec3_z_edit = memnew(LineEdit);
	vec3_z_edit->connect("focus_exited", this, "_focus_exited_z");
	vec3_z_edit->connect("text_entered", this, "_text_entered_z");
	vec3_z_edit->set_expand_to_text_length(true);
	vec3_z_edit->set_custom_minimum_size(Vector2(50.0, 0.0));
	add_child(vec3_z_edit);

	vec3_x_edit->set_text(String::num_real(vector.x));
	vec3_y_edit->set_text(String::num_real(vector.y));
	vec3_z_edit->set_text(String::num_real(vector.z));
}

/*************************************************************************/

void GDProcMeshNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_close_request"), &GDProcMeshNode::_close_request);
	ClassDB::bind_method(D_METHOD("_dragged", "from", "to"), &GDProcMeshNode::_dragged);
	ClassDB::bind_method(D_METHOD("_focus_exited"), &GDProcMeshNode::_focus_exited);
	ClassDB::bind_method(D_METHOD("_text_entered", "new_text"), &GDProcMeshNode::_text_entered);
	ClassDB::bind_method(D_METHOD("_input_toggled", "toggled"), &GDProcMeshNode::_input_toggled);

	ClassDB::bind_method(D_METHOD("_set_node_property_int", "value", "property", "field"), &GDProcMeshNode::_set_node_property_int);
	ClassDB::bind_method(D_METHOD("_exit_node_property_int", "property", "field"), &GDProcMeshNode::_exit_node_property_int);

	ClassDB::bind_method(D_METHOD("_set_node_property_real", "value", "property", "field"), &GDProcMeshNode::_set_node_property_real);
	ClassDB::bind_method(D_METHOD("_exit_node_property_real", "property", "field"), &GDProcMeshNode::_exit_node_property_real);

	ClassDB::bind_method(D_METHOD("_set_node_property_text", "value", "property", "field"), &GDProcMeshNode::_set_node_property_text);
	ClassDB::bind_method(D_METHOD("_exit_node_property_text", "property", "field"), &GDProcMeshNode::_exit_node_property_text);

	ClassDB::bind_method(D_METHOD("_set_node_property_bool", "value", "property", "field"), &GDProcMeshNode::_set_node_property_bool);
	ClassDB::bind_method(D_METHOD("_set_node_property_vec3", "value", "property", "field"), &GDProcMeshNode::_set_node_property_vec3);

	ADD_SIGNAL(MethodInfo("changed"));
}

Color GDProcMeshNode::_get_type_color(Variant::Type p_type) {
	switch (p_type) {
		case Variant::REAL: {
			return Color::html("#f34d4d");
		} break;
		case Variant::INT: {
			return Color::html("#0d6498");
		} break;
		case Variant::BOOL: {
			return Color::html("#c4780c");
		} break;
		case Variant::VECTOR3: {
			return Color::html("#1e8a76");
		} break;
		case Variant::STRING: {
			return Color::html("#107dcc");
		} break;
		case Variant::POOL_VECTOR3_ARRAY: {
			return Color::html("#d67dee");
		} break;
		case Variant::POOL_VECTOR2_ARRAY: {
			return Color::html("#61daf4");
		} break;
		case Variant::POOL_INT_ARRAY: {
			return Color::html("#f6a86e");
		} break;
		case Variant::POOL_REAL_ARRAY: {
			return Color::html("#8ad82b");
		} break;
		case Variant::POOL_COLOR_ARRAY: {
			// quad array
			return Color::html("#d82ba5");
		} break;
		case Variant::ARRAY: {
			// array is assumed to be a surface
			return Color::html("#a8fbde");
		} break;
		default: {
			WARN_PRINTS(RTR_LOCAL("Need a color for type ") + String(Variant::get_type_name(p_type)));
			return Color(0.0, 0.0, 1.0);
		} break;
	}
}

void GDProcMeshNode::_set_node_property_int(const String &p_value, const String &p_property, Object *p_field) {
	if (node) {
		node->set(p_property, p_value.to_int());

		// see what it changed to and update our display, need to figure out how...
		int prop_value = node->get(p_property);
		Object::cast_to<LineEdit>(p_field)->set_text(String::num_int64(prop_value));
	}
}

void GDProcMeshNode::_exit_node_property_int(const String &p_property, Object *p_field) {
	String value = Object::cast_to<LineEdit>(p_field)->get_text();
	_set_node_property_int(value, p_property, p_field);
}

void GDProcMeshNode::_set_node_property_real(const String &p_value, const String &p_property, Object *p_field) {
	if (node) {
		node->set(p_property, p_value.to_float());

		// see what it changed to and update our display, need to figure out how...
		float prop_value = node->get(p_property);
		Object::cast_to<LineEdit>(p_field)->set_text(String::num_real(prop_value));
	}
}

void GDProcMeshNode::_exit_node_property_real(const String &p_property, Object *p_field) {
	String value = Object::cast_to<LineEdit>(p_field)->get_text();
	_set_node_property_real(value, p_property, p_field);
}

void GDProcMeshNode::_set_node_property_text(const String &p_value, const String &p_property, Object *p_field) {
	if (node) {
		node->set(p_property, p_value);

		// see what it changed to and update our display, need to figure out how...
		Variant prop_value = node->get(p_property);
		Object::cast_to<LineEdit>(p_field)->set_text(prop_value);
	}
}

void GDProcMeshNode::_exit_node_property_text(const String &p_property, Object *p_field) {
	String value = Object::cast_to<LineEdit>(p_field)->get_text();
	_set_node_property_text(value, p_property, p_field);
}

void GDProcMeshNode::_set_node_property_bool(bool p_value, const String &p_property, Object *p_field) {
	if (node) {
		node->set(p_property, p_value);

		// see what it changed to and update our display, need to figure out how...
		Variant prop_value = node->get(p_property);
		Object::cast_to<CheckBox>(p_field)->set_pressed(prop_value);
	}
}

void GDProcMeshNode::_set_node_property_vec3(const Vector3 &p_value, const String &p_property, Object *p_field) {
	if (node) {
		node->set(p_property, p_value);

		// see what it changed to and update our display, need to figure out how...
		Variant prop_value = node->get(p_property);
		Object::cast_to<EditVec3>(p_field)->set_vector(prop_value);
	}
}

void GDProcMeshNode::_close_request() {
	if (proc_mesh) {
		proc_mesh->remove_node(node_id);
		emit_signal("changed");
	}
}

void GDProcMeshNode::_dragged(const Vector2 &p_from, const Vector2 &p_to) {
	if (node) {
		node->set_position(p_to);
	}
}

void GDProcMeshNode::_text_entered(const String &p_new_text) {
	if (node) {
		// need to change this logic to check if our node name has changed and if so, if the node name isn't already in use
		node->set_node_name(p_new_text);
	}
}

void GDProcMeshNode::_focus_exited() {
	_text_entered(name_edit->get_text());
}

void GDProcMeshNode::_input_toggled(bool p_button_pressed) {
	if (node) {
		node->set_hidden_input(p_button_pressed);
	}
}

void GDProcMeshNode::set_proc_node(GDProcMesh *p_proc_mesh, int p_node_id) {
	int slot_offset = 0;

	connect("close_request", this, "_close_request");
	connect("dragged", this, "_dragged");

	name_edit->connect("focus_exited", this, "_focus_exited");
	name_edit->connect("text_entered", this, "_text_entered");
	hide_input->connect("toggled", this, "_input_toggled");

	// copy some basic data
	proc_mesh = p_proc_mesh;
	node_id = p_node_id;
	node = proc_mesh->get_node(p_node_id).ptr();

	// init our graph node component
	set_offset(node->get_position());
	set_title(node->get_type_name());
	set_name(String::num_int64(p_node_id));
	set_tooltip(node->get_description());

	// copy our name
	name_edit->set_text(node->get_node_name());
	slot_offset += 1;

	// add input and output connectors
	int input_connector_count = node->get_input_connector_count();
	int output_connector_count = node->get_output_connector_count();

	if ((input_connector_count > 0) || (output_connector_count > 0)) {
		for (int i = 0; i < MAX(input_connector_count, output_connector_count); i++) {
			bool left_is_valid = i < input_connector_count;
			Variant::Type left_type = Variant::NIL;
			Color left_color = Color(0.0, 0.0, 1.0, 1.0);

			bool right_is_valid = i < output_connector_count;
			Variant::Type right_type = Variant::NIL;
			Color right_color = Color(0.0, 0.0, 1.0, 1.0);

			HBoxContainer *hb = memnew(HBoxContainer);

			if (left_is_valid) {
				Label *label = memnew(Label);
				label->set_text(node->get_input_connector_name(i));
				label->set_custom_minimum_size(Vector2(75.0, 0.0));
				hb->add_child(label);
				left_type = node->get_input_connector_type(i);
				left_color = _get_type_color(left_type);
			}

			String prop_name = node->get_connector_property_name(i);
			if (prop_name != String()) {
				Variant prop_value = node->get(prop_name);
				Variant::Type prop_type = prop_value.get_type();
				switch (prop_type) {
					case Variant::REAL: {
						LineEdit *prop_field = memnew(LineEdit);
						prop_field->set_text(String::num_real(float(prop_value)));
						prop_field->set_expand_to_text_length(true);
						prop_field->connect("text_entered", this, "_set_node_property_real", varray(prop_name, prop_field));
						prop_field->connect("focus_exited", this, "_exit_node_property_real", varray(prop_name, prop_field));
						prop_field->set_custom_minimum_size(Vector2(75.0, 0.0));
						hb->add_child(prop_field);
					} break;
					case Variant::INT: {
						LineEdit *prop_field = memnew(LineEdit);
						prop_field->set_text(String::num_int64(int(prop_value)));
						prop_field->set_expand_to_text_length(true);
						prop_field->connect("text_entered", this, "_set_node_property_int", varray(prop_name, prop_field));
						prop_field->connect("focus_exited", this, "_exit_node_property_int", varray(prop_name, prop_field));
						prop_field->set_custom_minimum_size(Vector2(75.0, 0.0));
						hb->add_child(prop_field);
					} break;
					case Variant::BOOL: {
						CheckBox *prop_field = memnew(CheckBox);
						prop_field->set_text(RTR_LOCAL("On"));
						prop_field->set_pressed(prop_value);
						prop_field->connect("toggled", this, "_set_node_property_bool", varray(prop_name, prop_field));
						prop_field->set_custom_minimum_size(Vector2(75.0, 0.0));
						hb->add_child(prop_field);
					} break;
					case Variant::VECTOR3: {
						EditVec3 *prop_field = memnew(EditVec3);
						prop_field->set_vector(prop_value);
						prop_field->connect("changed_vector", this, "_set_node_property_vec3", varray(prop_name, prop_field));
						hb->add_child(prop_field);
					} break;
					case Variant::STRING: {
						LineEdit *prop_field = memnew(LineEdit);
						prop_field->set_text(prop_value);
						prop_field->set_expand_to_text_length(true);
						prop_field->connect("text_entered", this, "_set_node_property_text", varray(prop_name, prop_field));
						prop_field->connect("focus_exited", this, "_exit_node_property_text", varray(prop_name, prop_field));
						prop_field->set_custom_minimum_size(Vector2(150.0, 0.0));
						hb->add_child(prop_field);
					} break;
					default: {
						WARN_PRINTS(RTR_LOCAL("Invalid property type"));
					} break;
				}
			}

			if (node->get_input_property_type() != Variant::NIL) {
				hide_input->set_visible(true);
				hide_input->set_pressed(node->get_hidden_input());
			} else {
				hide_input->set_visible(false);
			}

			hb->add_spacer(false);

			if (right_is_valid) {
				Label *label = memnew(Label);
				label->set_text(node->get_output_connector_name(i));
				label->set_align(Label::ALIGN_RIGHT);
				hb->add_child(label);
				right_type = node->get_output_connector_type(i);
				right_color = _get_type_color(right_type);
			}

			add_child(hb);
			set_slot(i + slot_offset, left_is_valid, left_type, left_color, right_is_valid, right_type, right_color);
		}
	}
}

void GDProcMeshNode::set_icon(Ref<ImageTexture> p_icon) {
	icon_rect->set_texture(p_icon);
}

GDProcMeshNode::GDProcMeshNode() {
	proc_mesh = NULL;
	node_id = -1;
	node = NULL;

	set_custom_minimum_size(Vector2(250.0, 75.0));
	set_show_close_button(true);

	name_container = memnew(HBoxContainer);
	add_child(name_container);

	icon_rect = memnew(TextureRect);
	name_container->add_child(icon_rect);

	name_label = memnew(Label);
	name_label->set_text(RTR_LOCAL("Name:"));
	name_container->add_child(name_label);

	name_edit = memnew(LineEdit);
	name_container->add_child(name_edit);
	name_edit->set_expand_to_text_length(true);
	name_edit->set_custom_minimum_size(Vector2(100.0, 0.0));

	hide_input = memnew(CheckBox);
	hide_input->set_text(RTR_LOCAL("Hide"));
	name_container->add_child(hide_input);
}

GDProcMeshNode::~GDProcMeshNode() {
}

/*************************************************************************/

void GDProcMeshEditor::_bind_methods() {

	ClassDB::bind_method(D_METHOD("_clear_graph"), &GDProcMeshEditor::_clear_graph);
	ClassDB::bind_method(D_METHOD("_update_graph"), &GDProcMeshEditor::_update_graph);
	ClassDB::bind_method(D_METHOD("_add_node", "type"), &GDProcMeshEditor::_add_node);

	ClassDB::bind_method(D_METHOD("_connection_request", "from", "from_index", "to", "to_index"), &GDProcMeshEditor::_connection_request);
	ClassDB::bind_method(D_METHOD("_disconnection_request", "from", "from_index", "to", "to_index"), &GDProcMeshEditor::_disconnection_request);
	ClassDB::bind_method(D_METHOD("_popup_request", "position"), &GDProcMeshEditor::_popup_request);

	ClassDB::bind_method(D_METHOD("_file_selected", "file"), &GDProcMeshEditor::_file_selected);
}

void GDProcMeshEditor::_clear_graph() {
	graph_edit->clear_connections();
	for (int i = 0; i < graph_edit->get_child_count(); i++) {

		if (Object::cast_to<GraphNode>(graph_edit->get_child(i))) {
			memdelete(graph_edit->get_child(i));
			i--;
		}
	}
}

void GDProcMeshEditor::_update_graph() {
	// clear graph
	_clear_graph();

	// lets build our new graph
	Vector<int> node_ids = proc_mesh->get_node_id_list();
	for (int i = 0; i < node_ids.size(); i++) {
		GDProcMeshNode *gn = _create_graph_node(node_ids[i]);
		graph_edit->add_child(gn);
	}

	Vector<GDProcMesh::connection> connections = proc_mesh->get_connection_list();
	for (int i = 0; i < connections.size(); i++) {
		graph_edit->connect_node(String::num_int64(connections[i].output.node), connections[i].output.connector, String::num_int64(connections[i].input.node), connections[i].input.connector);
	}
}

GDProcMeshNode *GDProcMeshEditor::_create_graph_node(int p_node_id) {

	GDProcMeshNode *gn = memnew(GDProcMeshNode);
	gn->set_proc_node(proc_mesh, p_node_id);
	gn->set_icon(plugin->get_gui_icon(proc_mesh->get_node(p_node_id)->get_class_name()));
	gn->connect("changed", this, "_update_graph");

	return gn;
}

void GDProcMeshEditor::_add_node(int p_type) {
	if (p_type == node_classes.size() + 100) {
		// add custom node
		file->popup_centered_ratio();
	} else if ((p_type > 0) && (p_type < node_classes.size())) {
		// add built-in node
		if (node_classes[p_type].type != String()) {
			Ref<GDProcNode> new_node = Ref<GDProcNode>(Object::cast_to<GDProcNode>(ClassDB::instance(node_classes[p_type].type)));

			if (new_node.is_valid()) {
				int id_to_use = proc_mesh->get_free_id();

				new_node->set_position(place + graph_edit->get_scroll_ofs());
				proc_mesh->add_node(new_node, id_to_use);

				_update_graph();
			} else {
				WARN_PRINTS(RTR_LOCAL("Error adding node: ") + node_classes[p_type].type);
			}
		} else {
			WARN_PRINTS(RTR_LOCAL("Error adding node: Invalid node type!"));
		}
	}
}

void GDProcMeshEditor::_file_selected(const String &p_file) {
	RES res = ResourceLoader::load(p_file, "GDProcNode");
	if (res.is_null()) {
		WARN_PRINTS(RTR_LOCAL("Error loading file: Not a resource!"));
	} else {
		Ref<GDProcNode> new_node = res;
		int id_to_use = proc_mesh->get_free_id();

		new_node->set_position(place + graph_edit->get_scroll_ofs());
		proc_mesh->add_node(new_node, id_to_use);

		_update_graph();
	}
}

void GDProcMeshEditor::_connection_request(const String &p_from, int p_from_index, const String &p_to, int p_to_index) {

	int output_node = p_from.to_int();
	int input_node = p_to.to_int();
	proc_mesh->add_connection(input_node, p_to_index, output_node, p_from_index);

	_update_graph();
}

void GDProcMeshEditor::_disconnection_request(const String &p_from, int p_from_index, const String &p_to, int p_to_index) {
	// ignore from
	int input_node = p_to.to_int();
	proc_mesh->remove_connection(input_node, p_to_index);

	_update_graph();
}

void GDProcMeshEditor::_popup_request(const Vector2 &p_position) {
	place = get_local_mouse_position();
	add_button->get_popup()->popup(Rect2(p_position, Vector2(1, 1)));
}

void GDProcMeshEditor::edit_mesh(GDProcMesh *p_mesh) {
	if (p_mesh) {
		proc_mesh = p_mesh;
		_update_graph();
	} else {
		proc_mesh = NULL;
		_clear_graph();
		hide();
	}
}

GDProcMeshEditor::GDProcMeshEditor(EditorNode *p_editor, GDProcMeshEditorPlugin *p_plugin) {
	editor = p_editor;
	plugin = p_plugin;

	graph_edit = memnew(GraphEdit);
	add_child(graph_edit);
	graph_edit->set_right_disconnects(true);
	graph_edit->set_v_size_flags(SIZE_EXPAND_FILL);
	graph_edit->connect("connection_request", this, "_connection_request", varray(), CONNECT_DEFERRED);
	graph_edit->connect("disconnection_request", this, "_disconnection_request", varray(), CONNECT_DEFERRED);
	graph_edit->connect("popup_request", this, "_popup_request");

	graph_edit->add_valid_connection_type(Variant::POOL_REAL_ARRAY, Variant::POOL_INT_ARRAY);
	graph_edit->add_valid_connection_type(Variant::POOL_INT_ARRAY, Variant::POOL_REAL_ARRAY);

	// create our add node button
	add_button = memnew(MenuButton);
	add_button->set_text(String(RTR_LOCAL("Add node...")));
	add_button->connect("pressed", this, "set", varray("place", Vector2(10.0, 50.0)));
	graph_edit->get_zoom_hbox()->add_child(add_button);
	graph_edit->get_zoom_hbox()->move_child(add_button, 0);

	// add some options (should make this smarter, maybe build an array first)
	add_button->get_popup()->connect("id_pressed", this, "_add_node");

	node_classes.push_back(AddNodeOption());

	// inputs
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Inputs/Input Curve"), "GDProcInCurve"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Inputs/Input Int"), "GDProcInInt"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Inputs/Input Mesh"), "GDProcInMesh"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Inputs/Input Vectors"), "GDProcInPoolVectors"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Inputs/Input Real"), "GDProcInReal"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Inputs/Input Vector"), "GDProcInVector"));

	// primitives
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Primitives/Count"), "GDProcCount"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Primitives/Euler Angles"), "GDProcEuler"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Primitives/Random"), "GDProcRandom"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Primitives/Split Vector"), "GDProcSplitVector"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Primitives/Vector"), "GDProcVector"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Primitives/Execute"), "GDProcExec"));

	// transforms (work on primitives)
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Add"), "GDProcAdd"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Bevel"), "GDProcBevel"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Division"), "GDProcDiv"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Generate Normals"), "GDProcGenNormals"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Redistribute"), "GDProcRedist"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Multiply"), "GDProcMult"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Rotate"), "GDProcRotate"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Multiply Rotation"), "GDProcRotMult"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Scale"), "GDProcScale"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Subtract"), "GDProcSub"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Transforms/Translate"), "GDProcTranslate"));

	// shapes
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Shapes/Box"), "GDProcBox"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Shapes/Circle"), "GDProcCircle"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Shapes/Line"), "GDProcLine"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Shapes/Rectangle"), "GDProcRect"));

	// surface
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Surfaces/Extrude Shape"), "GDProcExtrudeShape"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Surfaces/Simplify"), "GDProcSimplify"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Surfaces/Surface"), "GDProcSurface"));

	// modifiers (work on surfaces)
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Modifiers/Merge"), "GDProcMerge"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Modifiers/Mirror"), "GDProcMirror"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Modifiers/Place on Path"), "GDProcPlaceOnPath"));
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Modifiers/Transform"), "GDProcTransform"));

	// output
	node_classes.push_back(AddNodeOption(RTR_LOCAL("Outputs/Output"), "GDProcOutput"));

	add_button->get_popup()->clear();
	for (int i = 0; i < node_classes.size(); i++) {

		if ((node_classes[i].name != String()) && (node_classes[i].type != String())) {
			PopupMenu *popup = add_button->get_popup();

			Vector<String> names = node_classes[i].name.split("/");
			for (int j = 0; j < names.size() - 1; j++) {
				if (!popup->has_node(names[j])) {
					PopupMenu *new_popup = memnew(PopupMenu);
					new_popup->set_name(names[j]);
					new_popup->connect("id_pressed", this, "_add_node");
					popup->add_child(new_popup);
					popup->add_submenu_item(names[j], names[j]);
				}
				popup = Object::cast_to<PopupMenu>(popup->get_node(names[j]));
			}

			popup->add_icon_item(plugin->get_gui_icon(node_classes[i].type), names[names.size() - 1], i); //?
		}
	}
	add_button->get_popup()->add_separator();
	add_button->get_popup()->add_item(RTR_LOCAL("Custom Node..."), node_classes.size() + 100);

	// custom node resource selection dialog
	file = memnew(EditorFileDialog);
	add_child(file);
	file->hide();
	file->set_mode(EditorFileDialog::MODE_OPEN_FILE);

	List<String> extensions;
	ResourceLoader::get_recognized_extensions_for_type("GDProcNode", &extensions);
	Set<String> valid_extensions;
	for (List<String>::Element *E = extensions.front(); E; E = E->next()) {
		valid_extensions.insert(E->get());
	}
	file->clear_filters();
	for (Set<String>::Element *E = valid_extensions.front(); E; E = E->next()) {

		file->add_filter("*." + E->get() + " ; " + E->get().to_upper());
	}

	file->connect("file_selected", this, "_file_selected");
}

/*************************************************************************/

GDProcMeshEditorPlugin *GDProcMeshEditorPlugin::singleton = NULL;

void GDProcMeshEditorPlugin::_notification(int p_notification) {

	switch (p_notification) {

		case Node::NOTIFICATION_ENTER_TREE:
		case EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED: {

			// register node/resource type icons
			for (Map<String, Ref<ImageTexture> >::Element *E = type_icons.front(); E; E = E->next()) {
				editor->get_theme_base()->get_theme()->set_icon(E->key(), "EditorIcons", E->get());
			}
		}
	}
};

void GDProcMeshEditorPlugin::_bind_methods(){};

Ref<ImageTexture> GDProcMeshEditorPlugin::get_gui_icon(const String &p_key) {
	Ref<ImageTexture> icon = gui_icons[String("UI") + p_key];
	if (icon.is_null()) {
		icon = gui_icons["UIGDProcNode"];
	}
	return icon;
}

void GDProcMeshEditorPlugin::edit(Object *p_object) {
	if (!p_object)
		return;
	proc_mesh_editor->edit_mesh(Object::cast_to<GDProcMesh>(p_object));
}

bool GDProcMeshEditorPlugin::handles(Object *p_object) const {

	return p_object->is_class("GDProcMesh");
}

void GDProcMeshEditorPlugin::make_visible(bool p_visible) {

	if (p_visible) {
		button->show();
		editor->make_bottom_panel_item_visible(proc_mesh_editor);
		proc_mesh_editor->set_process(true);
	} else {

		if (proc_mesh_editor->is_visible_in_tree())
			editor->hide_bottom_panel();
		button->hide();
		proc_mesh_editor->set_process(false);
	}
}

StringName GDProcMeshEditorPlugin::tool_translate(const StringName &p_message) const {

	if (translation.is_valid()) {
		StringName r = translation->get_message(p_message);

		if (r) {
			return r;
		}
	}

	return p_message;
}

GDProcMeshEditorPlugin::GDProcMeshEditorPlugin(EditorNode *p_editor) {

	singleton = this;
	editor = p_editor;

	// init icons
	for (int i = 0; i < gdprocmesh_icons_count; i++) {
		Ref<ImageTexture> icon = memnew(ImageTexture);
		Ref<Image> img = memnew(Image);

		if (String(gdprocmesh_icons_names[i]).begins_with("GD")) {
			ImageLoaderSVG::create_image_from_string(img, gdprocmesh_icons_sources[i], EDSCALE, true, false);
			icon->create_from_image(img);
			type_icons[gdprocmesh_icons_names[i]] = icon; // node/resource type icons
		} else {
			ImageLoaderSVG::create_image_from_string(img, gdprocmesh_icons_sources[i], EDSCALE * 1.5, true, false);
			icon->create_from_image(img);
			gui_icons[gdprocmesh_icons_names[i]] = icon; // internal gui icons
		}
	}

	// init translations
	String lang = EditorSettings::get_singleton()->get("interface/editor/editor_language");
	EditorTranslationList *etl = _editor_translations;

	while (etl->data) {

		if (etl->lang == lang) {

			Vector<uint8_t> data;
			data.resize(etl->uncomp_size);
			Compression::decompress(data.ptrw(), etl->uncomp_size, etl->data, etl->comp_size, Compression::MODE_DEFLATE);

			FileAccessMemory *fa = memnew(FileAccessMemory);
			fa->open_custom(data.ptr(), data.size());

			Ref<Translation> tr = TranslationLoaderPO::load_translation(fa, NULL, "translation_" + String(etl->lang));

			if (tr.is_valid()) {
				tr->set_locale(etl->lang);

				translation = tr;
				break;
			}
		}
		etl++;
	}

	// create proc mesh editor control
	proc_mesh_editor = memnew(GDProcMeshEditor(editor, this));
	proc_mesh_editor->set_custom_minimum_size(Size2(400, 300));

	button = editor->add_bottom_panel_item(RTR_LOCAL("Procedural Mesh"), proc_mesh_editor);
	button->hide();
}

GDProcMeshEditorPlugin::~GDProcMeshEditorPlugin() {
	singleton = NULL;
}
