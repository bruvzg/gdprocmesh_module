/*************************************************************************/
/*  gdprocmesh.h                                                         */
/*************************************************************************/

#ifndef GD_PROC_MESH_H
#define GD_PROC_MESH_H

#include <scene/resources/material.h>
#include <scene/resources/mesh.h>

#include <core/map.h>
#include <core/vector.h>

#include "src/gdprocnode.h"
#include "src/output/gdprocoutput.h"
#include "src/shapes/gdprocbox.h"
#include "src/surfaces/gdprocsurface.h"
#include "src/transforms/gdprocgennormals.h"

class GDProcMesh : public ArrayMesh {
	GDCLASS(GDProcMesh, ArrayMesh)

private:
	bool is_dirty;
	void trigger_update();

	Map<int, Ref<GDProcNode> > nodes;
	bool node_id_is_used(int p_id) const;
	bool node_name_used(const String &p_name);
	String get_unique_node_name(const String &p_base_name);
	void _child_name_changed(const Ref<GDProcNode> &p_child, const String &p_from, const String &p_to);

	bool do_update_node(int p_id, Ref<GDProcNode> &p_node);

public:
	struct ctor {
		int node;
		int connector;

		ctor() {
			node = -1;
			connector = 0;
		}

		ctor(int p_node, int p_connector) {
			node = p_node;
			connector = p_connector;
		}
	};

	struct connection {
		ctor input;
		ctor output;

		connection() {
		}

		connection(int p_input_node, int p_input_connector, int p_output_node, int p_output_connector) {
			input.node = p_input_node;
			input.connector = p_input_connector;
			output.node = p_output_node;
			output.connector = p_output_connector;
		}
	};

private:
	Vector<connection> connections;
	const ctor get_output_for_input(int p_input_node, int p_input_connector) const;

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	static void _bind_methods();

public:
	void _post_init();
	void _update();

	/* nodes */
	int get_free_id();
	int add_node(Ref<GDProcNode> p_node, int p_id = 0);
	int find_node(const Ref<GDProcNode> &p_node);
	Ref<GDProcNode> get_node(int p_id);
	Vector<int> get_node_id_list();
	Array _get_node_id_list();
	void remove_node(int p_id);

	/* connections */
	void add_connection(int p_input_node, int p_input_connector, int p_output_node, int p_output_connector);
	void remove_connection(int p_input_node, int p_input_connector);
	Vector<connection> get_connection_list();
	Array _get_connection_list();

	/* old
	float get_size() const;
	void set_size(float new_size);
	*/

	GDProcMesh();
	~GDProcMesh();
};

#endif /* !GD_PROC_MESH_H */
