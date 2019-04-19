/*************************************************************************/
/*  gdprocmesh.cpp                                                       */
/*************************************************************************/

#include "servers/visual_server.h"

#include "src/gdprocmesh.h"

Vector<Variant> varray(const Variant &p_arg1, const Variant &p_arg2, const Variant &p_arg3, const Variant &p_arg4, const Variant &p_arg5, const Variant &p_arg6) {

	Vector<Variant> v;
	v.push_back(p_arg1);
	v.push_back(p_arg2);
	v.push_back(p_arg3);
	v.push_back(p_arg4);
	v.push_back(p_arg5);
	v.push_back(p_arg6);
	return v;
}

void GDProcMesh::_get_property_list(List<PropertyInfo> *p_list) const {

	// add a property for each node that we have
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, String("nodes/") + String::num_int64(E->key()), PROPERTY_HINT_RESOURCE_TYPE, "GDProcNode", PROPERTY_USAGE_NOEDITOR));
	}

	// now add a property for our connection
	p_list->push_back(PropertyInfo(Variant::POOL_INT_ARRAY, String("graph/connections"), PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR));

	// now add properties for any inputs, we should check if we have any issue with names not being unique
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {

		Variant::Type type = E->get()->get_input_property_type();
		String node_name = E->get()->get_node_name();

		if ((type != Variant::NIL) && (node_name != "")) {
			if (type == Variant::OBJECT) {
				p_list->push_back(PropertyInfo(type, String("inputs/") + node_name, PROPERTY_HINT_RESOURCE_TYPE, E->get()->get_input_property_hint(), (E->get()->get_hidden_input()) ? PROPERTY_USAGE_NOEDITOR : PROPERTY_USAGE_EDITOR));
			} else {
				p_list->push_back(PropertyInfo(type, String("inputs/") + node_name, PROPERTY_HINT_NONE, "", (E->get()->get_hidden_input()) ? PROPERTY_USAGE_NOEDITOR : PROPERTY_USAGE_EDITOR));
			}
		}
	}
}

bool GDProcMesh::_get(const StringName &p_name, Variant &r_ret) const {

	// temporary workaround to get around compile issue on Linux, needs further research
	String str_nodes = String("nodes/");
	String str_inputs = String("inputs/");
	String name = String(p_name);

	if (name.begins_with(str_nodes)) {
		String index = name.split("/")[1];
		int id = (int)index.to_int();

		const Map<int, Ref<GDProcNode> >::Element *E = nodes.find(id);
		if (!E) {
			ERR_EXPLAIN(RTR("Couldn't find node ") + String::num_int64(id));
			ERR_FAIL_V(false);
		} else {
			r_ret = Variant(E->get());
			return true;
		}
	} else if (name == "graph/connections") {
		PoolVector<int> ret;

		ret.resize((int)connections.size() * 4);
		{
			PoolVector<int>::Write w = ret.write();
			int *i = w.ptr();
			int p = 0;

			// add our connection data here
			for (int E = 0; E < connections.size(); E++) {
				i[p++] = connections[E].input.node;
				i[p++] = connections[E].input.connector;
				i[p++] = connections[E].output.node;
				i[p++] = connections[E].output.connector;
			}
		}

		r_ret = Variant(ret);
		return true;
	} else if (name.begins_with(str_inputs)) {
		String input_name = name.split("/")[1];

		for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
			if (E->get()->get_node_name() == input_name) {
				r_ret = E->get()->get_input();
				return true;
			}
		}
	}

	// Must be a property of our superclass, returning an empty (NIL) variant will handle it further
	return false;
}

bool GDProcMesh::_set(const StringName &p_name, const Variant &p_value) {
	// temporary workaround to get around compile issue on Linux, needs further research
	String str_nodes = String("nodes/");
	String str_inputs = String("inputs/");
	String name = String(p_name);

	if (name.begins_with(str_nodes)) {
		String index = name.split("/")[1];
		int id = (int)index.to_int();

		// printf("Loading node %i from scene\n", id);
		Ref<GDProcNode> n_node = p_value;
		add_node(n_node, id);

		return true;
	} else if (name == "graph/connections") {
		PoolVector<int> data = p_value;
		int count = data.size();

		for (int i = 0; i < count; i += 4) {
			add_connection(data[i], data[i + 1], data[i + 2], data[i + 3]);
		}

		return true;
	} else if (name.begins_with(str_inputs)) {
		String input_name = name.split("/")[1];

		for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
			if (E->get()->get_node_name() == input_name) {
				E->get()->set_input(p_value);
				return true;
			}
		}

		return true;
	}

	// Must be a property of our superclass, returning false will handle it further
	return false;
}

int GDProcMesh::get_free_id() {
	int new_id = 1;

	// loop through our keys to find our highest + 1
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		if (E->key() >= new_id) {
			new_id = E->key() + 1;
		}
	}

	return new_id;
}

bool GDProcMesh::node_id_is_used(int p_id) const {
	return nodes.find(p_id);
}

bool GDProcMesh::node_name_used(const String &p_name) {
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		if (E->get()->get_node_name() == p_name) {
			return true;
		}
	}

	return false;
}

String GDProcMesh::get_unique_node_name(const String &p_base_name) {
	int cnt = 0;
	String node_name = p_base_name;

	while (node_name_used(node_name)) {
		cnt += 1;
		node_name = p_base_name + "_";
		node_name += String::num_int64(cnt);
	}

	return node_name;
}

void GDProcMesh::_child_name_changed(const Ref<GDProcNode> &p_child, const String &p_from, const String &p_to) {
	// check if we need to change a surface
	if (p_child->get_output_connector_count() == 0) {
		int64_t s = surface_find_by_name(p_from);
		if (s != -1) {
			surface_set_name(s, p_to);
		}
	}

	// we have potentially renamed properties
	emit_signal("changed");
}

int GDProcMesh::add_node(Ref<GDProcNode> p_node, int p_id) {
	if (!p_node.is_valid()) {
		ERR_EXPLAIN(RTR("Invalid node passed to add_node"));
		ERR_FAIL_V(-1);
	} else if (p_id == 0) {
		// no id set? get an unused id
		p_id = get_free_id();
	} else if (node_id_is_used(p_id)) {
		// can't add this!
		return -1;
	}

	// make sure our node has a unique name..
	String node_name = p_node->get_node_name();
	if (node_name == "") {
		p_node->set_node_name(get_unique_node_name(p_node->get_type_name()));
	} else if (node_name_used(node_name)) {
		p_node->set_node_name(get_unique_node_name(node_name));
	}

	// trigger our trigger_update on a change of this node
	p_node->connect("changed", this, "trigger_update");
	p_node->connect("node_name_changed", this, "_child_name_changed");

	// Add our node
	nodes[p_id] = p_node;

	// trigger update
	trigger_update();

	// return the ID that we ended up using
	return p_id;
}

int GDProcMesh::find_node(const Ref<GDProcNode> &p_node) {
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		// note, == on Ref<> compares internal pointer :)
		if (E->get() == p_node) {
			return E->key();
		}
	}

	return -1;
}

Ref<GDProcNode> GDProcMesh::get_node(int p_id) {
	const Map<int, Ref<GDProcNode> >::Element *E = nodes.find(p_id);
	if (!E) {
		return Ref<GDProcNode>();
	} else {
		return E->get();
	}
}

Vector<int> GDProcMesh::get_node_id_list() {

	Vector<int> ret;
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		ret.push_back(Variant(E->key()));
	}

	return ret;
}

Array GDProcMesh::_get_node_id_list() {
	Array arr;

	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		arr.push_back(Variant(E->key()));
	}

	return arr;
}

void GDProcMesh::remove_node(int p_id) {
	size_t max = connections.size(); // size_t is unsigned so will wrap around!!
	if (max != 0) {
		// first remove any connector related to this
		for (size_t i = max - 1; i >= 0 && i < max; i--) {
			if ((connections[i].input.node == p_id) || (connections[i].output.node == p_id)) {
				if (connections[i].input.node != p_id) {
					// touch our input node
					get_node(connections[i].input.node)->_touch();
				}

				connections.remove(i);
			}
		}
	}

	// disconnect our signal
	nodes[p_id]->disconnect("changed", this, "trigger_update");
	nodes[p_id]->disconnect("node_name_changed", this, "_child_name_changed");

	// now remove our node
	nodes.erase(p_id);

	// trigger update
	trigger_update();
}

void GDProcMesh::add_connection(int p_input_node, int p_input_connector, int p_output_node, int p_output_connector) {
	// does our input node exist?
	Ref<GDProcNode> input_node = get_node(p_input_node);
	if (input_node.is_null()) {
		ERR_EXPLAIN(RTR("Unknown input node ") + String::num_int64(p_input_node));
		ERR_FAIL();
	}

	// does our input node actually have this connector?
	if (p_input_connector >= input_node->get_input_connector_count()) {
		ERR_EXPLAIN(RTR("Input node ") + String::num_int64(p_input_node) + RTR(" does not have connector ") + String::num_int64(p_input_connector));
		ERR_FAIL();
	}

	// does our output node exist?
	Ref<GDProcNode> output_node = get_node(p_output_node);
	if (output_node.is_null()) {
		ERR_EXPLAIN(RTR("Unknown output node ") + String::num_int64(p_output_node));
		ERR_FAIL();
	}

	// does our output nod eactually have this connector?
	if (p_output_connector >= output_node->get_output_connector_count()) {
		ERR_EXPLAIN(RTR("Output node ") + String::num_int64(p_output_node) + RTR(" does not have connector ") + String::num_int64(p_output_connector));
		ERR_FAIL();
	}

	// first remove any existing connection on our input connector
	remove_connection(p_input_node, p_input_connector);

	// now add a new one
	connections.push_back(connection(p_input_node, p_input_connector, p_output_node, p_output_connector));

	// touch our input node
	input_node->_touch();

	// trigger an update
	trigger_update();
}

void GDProcMesh::remove_connection(int p_input_node, int p_input_connector) {
	size_t max = connections.size(); // size_t is unsigned so will wrap around!!
	if (max == 0) {
		return;
	}

	// there should be only one but better safe then sorry..
	for (size_t i = max - 1; i >= 0 && i < max; i--) {
		if ((connections[i].input.node == p_input_node) && (connections[i].input.connector == p_input_connector)) {
			// touch our input node
			get_node(p_input_node)->_touch();

			connections.remove(i);

			// trigger update
			trigger_update();
		}
	}
}

Vector<GDProcMesh::connection> GDProcMesh::get_connection_list() {

	return connections;
}

Array GDProcMesh::_get_connection_list() {
	Array arr;

	for (int E = 0; E < connections.size(); E++) {
		Array row;

		row.push_back(connections[E].input.node);
		row.push_back(connections[E].input.connector);
		row.push_back(connections[E].output.node);
		row.push_back(connections[E].output.connector);

		arr.push_back(row);
	}

	return arr;
}

const GDProcMesh::ctor GDProcMesh::get_output_for_input(int p_input_node, int p_input_connector) const {
	size_t max = connections.size(); // size_t is unsigned so will wrap around!!
	if (max == 0) {
		return ctor(-1, 0);
	}

	for (size_t i = max - 1; i >= 0 && i < max; i--) {
		connection c = connections[i];
		if ((c.input.node == p_input_node) && (c.input.connector == p_input_connector)) {
			return c.output;
		}
	}

	return ctor(-1, 0);
}

void GDProcMesh::_bind_methods() {
	/* init and update */
	ClassDB::bind_method(D_METHOD("_post_init"), &GDProcMesh::_post_init);
	ClassDB::bind_method(D_METHOD("_update"), &GDProcMesh::_update);
	ClassDB::bind_method(D_METHOD("trigger_update"), &GDProcMesh::trigger_update);

	/* nodes */
	ClassDB::bind_method(D_METHOD("get_free_id"), &GDProcMesh::get_free_id);
	ClassDB::bind_method(D_METHOD("add_node", "node", "id"), &GDProcMesh::add_node, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("find_node", "node"), &GDProcMesh::find_node);
	ClassDB::bind_method(D_METHOD("get_node", "id"), &GDProcMesh::get_node);
	ClassDB::bind_method(D_METHOD("get_node_id_list"), &GDProcMesh::_get_node_id_list);
	ClassDB::bind_method(D_METHOD("remove_node", "id"), &GDProcMesh::remove_node);
	ClassDB::bind_method(D_METHOD("_child_name_changed", "child", "from", "to"), &GDProcMesh::_child_name_changed);

	/* should add some methods to interact with nodes directly */

	/* connections */
	ClassDB::bind_method(D_METHOD("add_connection", "input_node_id", "input_connector", "output_node_id", "output_connector"), &GDProcMesh::add_connection);
	ClassDB::bind_method(D_METHOD("remove_connection", "input_node_id", "input_connector"), &GDProcMesh::remove_connection);
	ClassDB::bind_method(D_METHOD("get_connection_list"), &GDProcMesh::_get_connection_list);
}

void GDProcMesh::trigger_update() {
	if (!is_dirty) {
		is_dirty = true;
		call_deferred("_update");
	}
}

void GDProcMesh::_post_init() {
	// Set dirty as false even though we haven't called update.
	// This means that if we've loaded a graph and our
	// previously generated mesh, which is saved in the scene,
	// we skip a bunch of expensive recalculating.
	is_dirty = false;

	if (nodes.size() == 0) {
		// we have no nodes, so create our defaults...

		// create our output
		Ref<GDProcOutput> output;
		output.instance();
		output->set_name("Output");
		output->set_position(Vector2(600.0, 50.0));
		/*int output_id = */ add_node(output);

		// create a box
		//Ref<GDProcBox> box;
		//box.instance();
		//box->set_position(Vector2(50.0, 50.0));
		//int box_id = add_node(box);

		// add our connections
		// add_connection(output_id, 0, box_id, 0); // bind to our output

		// note that this will have trigger an update...
	}
}

bool GDProcMesh::do_update_node(int p_id, Ref<GDProcNode> &p_node) {
	switch (p_node->get_status()) {
		case GDProcNode::PROCESS_STATUS_INPROGRESS: {
			// this is bad, we have a circular dependency
			ERR_EXPLAIN(RTR("Found circular dependency in procedural mesh!"));
			ERR_FAIL_V(false);
		}; break;
		case GDProcNode::PROCESS_STATUS_UNCHANGED: {
			// already processed and found it to be unchanged
			return false;
		}; break;
		case GDProcNode::PROCESS_STATUS_CHANGED: {
			// already processed and found it to be changed
			return true;
		}; break;
		default: {
			bool updated = false;
			Array inputs;

			// set node as being in progress
			p_node->set_status(GDProcNode::PROCESS_STATUS_INPROGRESS);

			// check all our inputs
			for (int i = 0; i < p_node->get_input_connector_count(); i++) {
				// find if this has been connected
				ctor c = get_output_for_input(p_id, i);

				if (c.node == -1) {
					// printf("Node %i, Connector %i is not connected\n", p_id, i);
					// if not, just add a NIL input
					inputs.push_back(Variant());
				} else {
					// make sure this is a valid node
					const Map<int, Ref<GDProcNode> >::Element *E = nodes.find(c.node);
					if (!E) {
						WARN_PRINTS(RTR("Unknown node ") + String::num_int64(c.node));
						inputs.push_back(Variant());
					} else {
						Ref<GDProcNode> output_node = E->get();

						// lets make sure this has been updated (if we already did then that is fine)
						if (do_update_node(c.node, output_node)) {
							updated = true;
						}

						// and get our output
						Variant output = output_node->get_output(c.connector);

						// Do automatic conversions of outputs if necessary
						Variant::Type output_type = output_node->get_output_connector_type(c.connector);
						Variant::Type input_type = p_node->get_input_connector_type(i);
						if (output_type == Variant::POOL_REAL_ARRAY && input_type == Variant::POOL_INT_ARRAY) {
							PoolVector<real_t> reals = output;
							PoolVector<real_t>::Read read = reals.read();
							PoolVector<int> integers;
							integers.resize(reals.size());
							PoolVector<int>::Write write = integers.write();
							for (int i_convert = 0; i_convert < reals.size(); i_convert++) {
								write[i_convert] = (int)read[i_convert];
							}
							output = integers;
						} else if (output_type == Variant::POOL_INT_ARRAY && input_type == Variant::POOL_REAL_ARRAY) {
							PoolVector<int> integers = output;
							PoolVector<int>::Read read = integers.read();
							PoolVector<real_t> reals;
							reals.resize(integers.size());
							PoolVector<real_t>::Write write = reals.write();
							for (int i_convert = 0; i_convert < integers.size(); i_convert++) {
								write[i_convert] = (real_t)read[i_convert];
							}
							output = reals;
						} else if (output_type != input_type) {
							// In this case we have a problem
							WARN_PRINTS(RTR("Output type ({0}) and input type ({1}) do not match.\n"
											"Error: Bad Connection - Node {2} : Connector {3} --> Node {4} : Connector {5}")
												.format(varray(output_type, input_type, output_node->get_node_name(),
														c.connector, p_node->get_node_name(), i)));
							output = Variant();
						}

						inputs.push_back(output);
					}
				}
			}

			// update this node
			// printf("Update %s\n", p_node->get_node_name().utf8().get_data());
			if (p_node->update(updated, inputs)) {
				updated = true;
			}

			// set node as processed with our result status..
			p_node->set_status(updated ? GDProcNode::PROCESS_STATUS_CHANGED : GDProcNode::PROCESS_STATUS_UNCHANGED);

			return updated;
		}; break;
	}
}

void GDProcMesh::_update() {

	// if we've already updated this, exit....
	if (!is_dirty) {
		// printf("Update called but not dirty\n");
		return;
	}

	// if we change any surface we turn this to true and check if we need to do any post processing.
	//bool has_changed = false;

	// clear surfaces we'll no longer be needing
	for (int64_t s = get_surface_count() - 1; s >= 0; s--) {
		String name = surface_get_name(s);
		bool found = false;

		for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); !found && E; E = E->next()) {
			String node_name = E->get()->get_node_name();
			if (node_name == "") {
				node_name = "Surface_";
				node_name += String::num_int64(E->key());
			}
			if ((node_name == name) && (E->get()->get_output_connector_count() == 0)) {
				found = true;
			}
		}

		if (!found) {
			//printf("Removing unused surface %s\n", name.utf8().get_data());
			surface_remove(s);
		}
	}

	// Reset our process status, we want to check each node atleast once...
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		E->get()->set_status(GDProcNode::PROCESS_STATUS_PENDING);
	}

	// now find all our final nodes
	for (Map<int, Ref<GDProcNode> >::Element *E = nodes.front(); E; E = E->next()) {
		if (E->get()->get_output_connector_count() == 0) {
			// trigger updating this node
			bool changed = do_update_node(E->key(), E->get());

			// if contents has changed, update our surface
			if (changed) {
				// get our new surface
				Variant surface = E->get()->get_output(0);

				// check if this is a valid array and update!
				if (surface.get_type() != Variant::ARRAY) {
					WARN_PRINTS(RTR("Final node is not returning an array"));
				} else {
					Array arr = surface;

					if (arr.size() != GDProcMesh::ARRAY_MAX) {
						WARN_PRINTS(RTR("Final node is not returning a correctly sized array"));
					} else if (arr[GDProcMesh::ARRAY_VERTEX].get_type() != Variant::POOL_VECTOR3_ARRAY) {
						WARN_PRINTS(RTR("No vertices in surface"));
					} else if (((PoolVector<Vector3>)arr[GDProcMesh::ARRAY_VERTEX]).size() == 0) {
						WARN_PRINTS(RTR("No vertices in surface"));
					} else if (arr[GDProcMesh::ARRAY_INDEX].get_type() != Variant::POOL_INT_ARRAY) {
						WARN_PRINTS(RTR("No indices in surface"));
					} else if (((PoolVector<int>)arr[GDProcMesh::ARRAY_INDEX]).size() == 0) {
						WARN_PRINTS(RTR("No indices in surface"));
					} else {
						// only replace the surface if we have a valid surface or we'll loose our material.

						String name = E->get()->get_node_name();
						if (name == "") {
							name = "Surface_";
							name += String::num_int64(E->key());
						}
						Ref<Material> material;

						// find our surface and get some info we may want to cache like our material
						int64_t s = surface_find_by_name(name);
						if (s != -1) {
							// printf("Removing changed surface %lli, %s\n", s, name.utf8().get_data());

							// remember our material, we're reusing it!
							material = surface_get_material(s);

							// clear our surface
							surface_remove(s);
						}

						// lets add a new surface
						int64_t new_surface_id = get_surface_count();
						add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr);

						surface_set_name(new_surface_id, name);

						if (material.is_valid()) {
							surface_set_material(new_surface_id, material);
						}
					}
				}
			}
		}
	}

	// and we're good
	is_dirty = false;
}

GDProcMesh::GDProcMesh() {
	// prevent deferred updates until we're ready
	is_dirty = true;

	// this should delay running _post_init until after all our data has been loaded
	call_deferred("_post_init");
}

GDProcMesh::~GDProcMesh() {
	// do we need to clean up our map?
}
