/*************************************************************************/
/*  gdprocmirror.cpp                                                     */
/*************************************************************************/

#include "src/modifiers/gdprocmirror.h"

void GDProcMirror::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_mirror_x"), &GDProcMirror::get_mirror_x);
	ClassDB::bind_method(D_METHOD("set_mirror_x", "mirror_x"), &GDProcMirror::set_mirror_x);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "mirror_x"), "set_mirror_x", "get_mirror_x");

	ClassDB::bind_method(D_METHOD("get_mirror_y"), &GDProcMirror::get_mirror_y);
	ClassDB::bind_method(D_METHOD("set_mirror_y", "mirror_y"), &GDProcMirror::set_mirror_y);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "mirror_y"), "set_mirror_y", "get_mirror_y");

	ClassDB::bind_method(D_METHOD("get_mirror_z"), &GDProcMirror::get_mirror_z);
	ClassDB::bind_method(D_METHOD("set_mirror_z", "mirror_z"), &GDProcMirror::set_mirror_z);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "mirror_z"), "set_mirror_z", "get_mirror_z");

	ClassDB::bind_method(D_METHOD("get_duplicate"), &GDProcMirror::get_duplicate);
	ClassDB::bind_method(D_METHOD("set_duplicate", "duplicate"), &GDProcMirror::set_duplicate);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "duplicate"), "set_duplicate", "get_duplicate");
}

String GDProcMirror::get_type_name() const {
	return RTR_LOCAL("Mirror");
}

String GDProcMirror::get_description() const {
	return RTR_LOCAL("Mirrors the surface on the chosen axes.\nIf Duplicate is enabled, the node makes a copy of the surface before mirroring.");
}

// properties
void GDProcMirror::set_mirror_x(bool p_mirror) {
	if (default_mirror_x != p_mirror) {
		default_mirror_x = p_mirror;
		must_update = true;
		emit_signal("changed");
	}
}

bool GDProcMirror::get_mirror_x() {
	return default_mirror_x;
}

void GDProcMirror::set_mirror_y(bool p_mirror) {
	if (default_mirror_y != p_mirror) {
		default_mirror_y = p_mirror;
		must_update = true;
		emit_signal("changed");
	}
}

bool GDProcMirror::get_mirror_y() {
	return default_mirror_y;
}

void GDProcMirror::set_mirror_z(bool p_mirror) {
	if (default_mirror_z != p_mirror) {
		default_mirror_z = p_mirror;
		must_update = true;
		emit_signal("changed");
	}
}

bool GDProcMirror::get_mirror_z() {
	return default_mirror_z;
}

void GDProcMirror::set_duplicate(bool p_duplicate) {
	if (default_duplicate != p_duplicate) {
		default_duplicate = p_duplicate;
		must_update = true;
		emit_signal("changed");
	}
}

bool GDProcMirror::get_duplicate() {
	return default_duplicate;
};

bool GDProcMirror::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		bool mirror_x = default_mirror_x;
		bool mirror_y = default_mirror_y;
		bool mirror_z = default_mirror_z;
		bool duplicate = default_duplicate;
		int offset = 0;
		Array input_surface;

		// resize our array just in case
		surface_arr.resize(ArrayMesh::ARRAY_MAX);

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::ARRAY) {
				// get our surface
				input_surface = p_inputs[0];
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::BOOL) {
				mirror_x = p_inputs[1];
			}
		}
		if (input_count > 2) {
			if (p_inputs[2].get_type() == Variant::BOOL) {
				mirror_y = p_inputs[2];
			}
		}
		if (input_count > 3) {
			if (p_inputs[3].get_type() == Variant::BOOL) {
				duplicate = p_inputs[3];
			}
		}
		if (input_count > 4) {
			if (p_inputs[4].get_type() == Variant::BOOL) {
				duplicate = p_inputs[4];
			}
		}

		if (input_surface.size() == ArrayMesh::ARRAY_MAX) {
			int mirror_count = (mirror_x ? 1 : 0) + (mirror_y ? 1 : 0) + (mirror_z ? 1 : 0);

			if (input_surface[ArrayMesh::ARRAY_VERTEX].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				PoolVector<Vector3> input = input_surface[ArrayMesh::ARRAY_VERTEX];
				PoolVector<Vector3> output;
				int size = input.size();

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
					offset = size;
				} else {
					offset = 0;
				}

				// double the size of our buffer
				output.resize(size * 2);
				PoolVector<Vector3>::Write w = output.write();
				PoolVector<Vector3>::Read r = input.read();

				for (int i = 0; i < size; i++) {
					Vector3 v = r[i];
					if (mirror_x) v.x = -v.x;
					if (mirror_y) v.y = -v.y;
					if (mirror_z) v.z = -v.z;
					w[offset + i] = v;
				}

				surface_arr[ArrayMesh::ARRAY_VERTEX] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_VERTEX] = Variant();
			}

			if (input_surface[ArrayMesh::ARRAY_NORMAL].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				PoolVector<Vector3> input = input_surface[ArrayMesh::ARRAY_NORMAL];
				PoolVector<Vector3> output;
				int size = input.size();
				int strafe = 0;

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
					strafe = size;
				}

				// double the size of our buffer
				output.resize(size * 2);
				PoolVector<Vector3>::Write w = output.write();
				PoolVector<Vector3>::Read r = input.read();

				for (int i = 0; i < size; i++) {
					Vector3 v = r[i];
					if (mirror_x) v.x = -v.x;
					if (mirror_y) v.y = -v.y;
					if (mirror_z) v.z = -v.z;
					w[strafe + i] = v;
				}

				surface_arr[ArrayMesh::ARRAY_NORMAL] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_NORMAL] = Variant();
			}

			if (input_surface[ArrayMesh::ARRAY_TANGENT].get_type() == Variant::POOL_REAL_ARRAY) {
				///@TODO need to figure this one out, probably just like with our faces, if we do an uneven mirror we need to flip something
				PoolVector<real_t> input = input_surface[ArrayMesh::ARRAY_TANGENT];
				PoolVector<real_t> output;
				int size = input.size();
				int strafe = 0;

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
					strafe = size;
				}

				/* double our size */
				output.resize(size * 2);
				PoolVector<real_t>::Write w = output.write();
				PoolVector<real_t>::Read r = input.read();

				// and a copy for our mirror
				for (int i = 0; i < size; i += 4) {
					w[i + strafe] = mirror_x ? -r[i] : r[i];
					w[i + strafe + 1] = mirror_y ? -r[i + 1] : r[i + 1];
					w[i + strafe + 2] = mirror_z ? -r[i + 2] : r[i + 2];
					w[i + strafe + 3] = (mirror_count == 1 || mirror_count == 3) ? -r[i + 3] : r[i + 3];
				}

				surface_arr[ArrayMesh::ARRAY_TANGENT] = output;
			}

			if (input_surface[ArrayMesh::ARRAY_COLOR].get_type() == Variant::POOL_COLOR_ARRAY) {
				PoolVector<Color> input = input_surface[ArrayMesh::ARRAY_COLOR];
				PoolVector<Color> output;

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
				}

				// and a copy for our mirror
				output.append_array(input);

				surface_arr[ArrayMesh::ARRAY_COLOR] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_COLOR] = Variant();
			}

			if (input_surface[ArrayMesh::ARRAY_TEX_UV].get_type() == Variant::POOL_VECTOR2_ARRAY) {
				PoolVector<Vector2> input = input_surface[ArrayMesh::ARRAY_TEX_UV];
				PoolVector<Vector2> output;

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
				}

				// and a copy for our mirror
				output.append_array(input);

				surface_arr[ArrayMesh::ARRAY_TEX_UV] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_TEX_UV] = Variant();
			}

			if (input_surface[ArrayMesh::ARRAY_TEX_UV2].get_type() == Variant::POOL_VECTOR2_ARRAY) {
				PoolVector<Vector2> input = input_surface[ArrayMesh::ARRAY_TEX_UV2];
				PoolVector<Vector2> output;

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
				}

				// and a copy for our mirror
				output.append_array(input);

				surface_arr[ArrayMesh::ARRAY_TEX_UV2] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_TEX_UV2] = Variant();
			}

			if (input_surface[ArrayMesh::ARRAY_BONES].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> input = input_surface[ArrayMesh::ARRAY_BONES];
				PoolVector<real_t> output;

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
				}

				// and a copy for our mirror
				output.append_array(input);

				surface_arr[ArrayMesh::ARRAY_BONES] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_BONES] = Variant();
			}

			if (input_surface[ArrayMesh::ARRAY_WEIGHTS].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> input = input_surface[ArrayMesh::ARRAY_WEIGHTS];
				PoolVector<real_t> output;

				// Start with copying our original
				if (duplicate) {
					output.append_array(input);
				}

				// and a copy for our mirror
				output.append_array(input);

				surface_arr[ArrayMesh::ARRAY_WEIGHTS] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_WEIGHTS] = Variant();
			}

			if (input_surface[ArrayMesh::ARRAY_INDEX].get_type() == Variant::POOL_INT_ARRAY) {
				PoolVector<int> input = input_surface[ArrayMesh::ARRAY_INDEX];
				PoolVector<int> output;
				int size = input.size();
				int strafe = 0;

				// double the size of our buffer
				output.resize(size * 2);
				PoolVector<int>::Write w = output.write();
				PoolVector<int>::Read r = input.read();

				// Start with copying our original
				if (duplicate) {
					for (int i = 0; i < size; i++) {
						w[i] = r[i];
					}
					strafe = size;
				}

				if ((mirror_count == 0) || (mirror_count == 2)) {
					for (int i = 0; i < size; i++) {
						w[i + strafe] = r[i] + offset;
					}
				} else {
					for (int i = 0; i < size; i += 3) {
						w[i + strafe] = r[i] + offset;
						w[i + strafe + 1] = r[i + 2] + offset;
						w[i + strafe + 2] = r[i + 1] + offset;
					}
				}

				surface_arr[ArrayMesh::ARRAY_INDEX] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_INDEX] = Variant();
			}
		}
	}

	return updated;
}

int GDProcMirror::get_input_connector_count() const {
	return 5;
}

Variant::Type GDProcMirror::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::ARRAY;
	} else if (p_slot == 1) {
		return Variant::BOOL;
	} else if (p_slot == 2) {
		return Variant::BOOL;
	} else if (p_slot == 3) {
		return Variant::BOOL;
	} else if (p_slot == 4) {
		return Variant::BOOL;
	}

	return Variant::NIL;
}

String GDProcMirror::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("surface");
	} else if (p_slot == 1) {
		return String("mirror_x");
	} else if (p_slot == 2) {
		return String("mirror_y");
	} else if (p_slot == 3) {
		return String("mirror_z");
	} else if (p_slot == 4) {
		return String("duplicate");
	}

	return String();
}

String GDProcMirror::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("mirror_x");
	} else if (p_slot == 2) {
		return String("mirror_y");
	} else if (p_slot == 3) {
		return String("mirror_z");
	} else if (p_slot == 4) {
		return String("duplicate");
	}

	return String();
}

int GDProcMirror::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcMirror::get_output_connector_type(int p_slot) const {
	return Variant::ARRAY;
}

String GDProcMirror::get_output_connector_name(int p_slot) const {
	return String("surface");
}

Variant GDProcMirror::get_output(int p_slot) const {
	return surface_arr;
}

GDProcMirror::GDProcMirror() {
	// init
	default_mirror_x = true;
	default_mirror_y = false;
	default_mirror_z = false;
	default_duplicate = true;
}