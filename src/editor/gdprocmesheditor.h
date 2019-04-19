/*************************************************************************/
/*  gdprocmesheditor.h                                                   */
/*************************************************************************/

#ifndef GD_PROC_MESH_EDITOR_H
#define GD_PROC_MESH_EDITOR_H

#include <core/map.h>
#include <core/resource.h>
#include <editor/editor_export.h>
#include <editor/editor_file_dialog.h>
#include <editor/editor_node.h>

#include <scene/gui/button.h>
#include <scene/gui/graph_edit.h>
#include <scene/gui/label.h>
#include <scene/gui/line_edit.h>
#include <scene/gui/popup.h>

#include "src/gdprocmesh.h"
#include "src/gdprocnode.h"

#include "src/local_translation.h"

class GDProcMeshEditorPlugin;

/*************************************************************************/

class EditVec3 : public HBoxContainer {
	GDCLASS(EditVec3, HBoxContainer);

	mutable Vector3 vector;

	LineEdit *vec3_x_edit;
	LineEdit *vec3_y_edit;
	LineEdit *vec3_z_edit;

protected:
	static void _bind_methods();

	void _text_entered_x(const String &p_new_text);
	void _focus_exited_x();
	void _text_entered_y(const String &p_new_text);
	void _focus_exited_y();
	void _text_entered_z(const String &p_new_text);
	void _focus_exited_z();

public:
	Vector3 get_vector() const;
	void set_vector(const Vector3 &p_vector);

	EditVec3();
};

/*************************************************************************/

class GDProcMeshNode : public GraphNode {
	GDCLASS(GDProcMeshNode, GraphNode);

	GDProcMesh *proc_mesh;
	int node_id;
	GDProcNode *node;

	HBoxContainer *name_container;
	Label *name_label;
	LineEdit *name_edit;
	CheckBox *hide_input;
	TextureRect *icon_rect;

protected:
	static void _bind_methods();

	Color _get_type_color(Variant::Type p_type);
	void _set_node_property_int(const String &p_value, const String &p_property, Object *p_field);
	void _exit_node_property_int(const String &p_property, Object *p_field);

	void _set_node_property_real(const String &p_value, const String &p_property, Object *p_field);
	void _exit_node_property_real(const String &p_property, Object *p_field);

	void _set_node_property_text(const String &p_value, const String &p_property, Object *p_field);
	void _exit_node_property_text(const String &p_property, Object *p_field);

	void _set_node_property_bool(bool p_value, const String &p_property, Object *p_field);
	void _set_node_property_vec3(const Vector3 &p_value, const String &p_property, Object *p_field);

	void _close_request();
	void _dragged(const Vector2 &p_from, const Vector2 &p_to);
	void _text_entered(const String &p_new_text);
	void _focus_exited();
	void _input_toggled(bool p_button_pressed);

public:
	void set_proc_node(GDProcMesh *p_proc_mesh, int p_node_id);
	void set_icon(Ref<ImageTexture> p_icon);

	GDProcMeshNode();
	~GDProcMeshNode();
};

/*************************************************************************/

class GDProcMeshEditor : public VBoxContainer {
	GDCLASS(GDProcMeshEditor, VBoxContainer);

	EditorNode *editor;
	GDProcMeshEditorPlugin *plugin;

	GDProcMesh *proc_mesh;
	GraphEdit *graph_edit;

	Vector2 place;

	MenuButton *add_button;
	EditorFileDialog *file;

	struct AddNodeOption {
		String name;
		String type;

		AddNodeOption(const String &p_name = String(), const String &p_type = String()) {
			name = p_name;
			type = p_type;
		}
	};
	Vector<AddNodeOption> node_classes;

protected:
	static void _bind_methods();

	void _clear_graph();
	void _update_graph();

	GDProcMeshNode *_create_graph_node(int p_node_id);

	void _add_node(int p_type);

	void _connection_request(const String &p_from, int p_from_index, const String &p_to, int p_to_index);
	void _disconnection_request(const String &p_from, int p_from_index, const String &p_to, int p_to_index);
	void _popup_request(const Vector2 &p_position);

	void _file_selected(const String &p_file);

public:
	void edit_mesh(GDProcMesh *p_mesh);

	GDProcMeshEditor(EditorNode *p_editor, GDProcMeshEditorPlugin *p_plugin);
};

/*************************************************************************/

class GDProcMeshEditorPlugin : public EditorPlugin {
	GDCLASS(GDProcMeshEditorPlugin, EditorPlugin)

	EditorNode *editor;
	GDProcMeshEditor *proc_mesh_editor;
	Button *button;

	Ref<Translation> translation;

	Map<String, Ref<ImageTexture> > gui_icons;
	Map<String, Ref<ImageTexture> > type_icons;

protected:
	void _notification(int p_notification);
	static void _bind_methods();

	static GDProcMeshEditorPlugin *singleton;

public:
	_FORCE_INLINE_ static GDProcMeshEditorPlugin *get_singleton() { return singleton; }
	StringName tool_translate(const StringName &p_message) const;

	Ref<ImageTexture> get_gui_icon(const String &p_key);

	virtual String get_name() const { return "GDProcMesh"; }
	bool has_main_screen() const { return false; }
	void edit(Object *p_object);
	bool handles(Object *p_object) const;
	void make_visible(bool p_visible);

	GDProcMeshEditorPlugin(EditorNode *p_editor);
	~GDProcMeshEditorPlugin();
};

#endif /* !GD_PROC_MESH_EDITOR_H */
