/*************************************************************************/
/*  gdprocplaceonpath.cpp                                                */
/*************************************************************************/

#include "src/modifiers/gdprocplaceonpath.h"

void GDProcPlaceOnPath::_bind_methods() {
}

String GDProcPlaceOnPath::get_type_name() const {
	return RTR("Place on Path");
}

String GDProcPlaceOnPath::get_description() const {
	return RTR("Places a copy of the surface at the given rotation(s), translation(s) and scale(s).");
}

bool GDProcPlaceOnPath::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		Array input_surface;
		int strafe = 0;
		int num_rotations = 0;
		PoolVector<Color> rotations;
		int num_translations = 0;
		PoolVector<Vector3> translations;
		int num_scales = 0;
		PoolVector<Vector3> scales;

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
			if (p_inputs[1].get_type() == Variant::POOL_COLOR_ARRAY) {
				rotations = p_inputs[1];
				num_rotations = rotations.size();
			}
		}
		if (input_count > 2) {
			if (p_inputs[2].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				translations = p_inputs[2];
				num_translations = translations.size();
			}
		}
		if (input_count > 3) {
			if (p_inputs[3].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				scales = p_inputs[3];
				num_scales = scales.size();
			}
		}

		if (num_rotations == 0) {
			rotations.push_back(Color(0.0, 0.0, 0.0, 1.0));
			num_rotations++;
		}

		// if we don't have inputs just default to center
		if (num_translations == 0) {
			translations.push_back(Vector3(0.0, 0.0, 0.0));
			num_translations++;
		}

		if (num_scales == 0) {
			scales.push_back(Vector3(1.0, 1.0, 1.0));
			num_scales++;
		}

		if (input_surface.size() == ArrayMesh::ARRAY_MAX) {
			int copies = num_translations > num_scales ? num_translations : num_scales;

			PoolVector<Vector3>::Read t = translations.read();
			PoolVector<Vector3>::Read s = scales.read();

			// Convert my quaternion array to basis, xform on quaternion seems broken
			PoolVector<Color>::Read q = rotations.read();
			Basis *rots = (Basis *)memalloc(sizeof(Basis) * num_rotations);
			for (int i = 0; i < num_rotations; i++) {
				Color rot = q[i];
				Quat quat(rot.r, rot.g, rot.b, rot.a);

				// convert to basis, I had problems with quat.xform...
				rots[i] = Basis(quat);
			}

			///@TODO this is a lot of duplication of code, there must be an easier way

			// Duplicate vertices and place
			if (input_surface[ArrayMesh::ARRAY_VERTEX].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				PoolVector<Vector3> input = input_surface[ArrayMesh::ARRAY_VERTEX];
				PoolVector<Vector3> output;

				int n = 0;
				int size = input.size();
				strafe = size; // remember this

				// printf("Duplicating %i vertices %i times\n", size, copies);

				output.resize(size * copies);
				PoolVector<Vector3>::Write w = output.write();
				PoolVector<Vector3>::Read r = input.read();
				for (int copy = 0; copy < copies; copy++) {
					Basis rotation = rots[copy % num_rotations];
					Vector3 translation = t[copy % num_translations];
					Vector3 scale = s[copy % num_scales];
					for (int i = 0; i < size; i++) {
						w[n++] = (rotation.xform(r[i]) * scale) + translation;
					}
				}

				surface_arr[ArrayMesh::ARRAY_VERTEX] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_VERTEX] = Variant();
			}

			// Duplicate normals
			if (input_surface[ArrayMesh::ARRAY_NORMAL].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				PoolVector<Vector3> input = input_surface[ArrayMesh::ARRAY_NORMAL];
				PoolVector<Vector3> output;

				int n = 0;
				int size = input.size();
				output.resize(size * copies);

				PoolVector<Vector3>::Write w = output.write();
				PoolVector<Vector3>::Read r = input.read();

				for (int copy = 0; copy < copies; copy++) {
					Basis rotation = rots[copy % num_rotations];
					for (int i = 0; i < size; i++) {
						w[n++] = rotation.xform(r[i]);
					}
				}

				surface_arr[ArrayMesh::ARRAY_NORMAL] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_NORMAL] = Variant();
			}

			// Duplicate tangents
			if (input_surface[ArrayMesh::ARRAY_TANGENT].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> input = input_surface[ArrayMesh::ARRAY_TANGENT];
				PoolVector<real_t> output;

				int n = 0;
				int size = input.size();
				output.resize(size * copies);

				PoolVector<real_t>::Write w = output.write();
				PoolVector<real_t>::Read r = input.read();

				for (int copy = 0; copy < copies; copy++) {
					Basis rotation = rots[copy % num_rotations];
					for (int i = 0; i < size; i += 4) {
						Vector3 v(r[i], r[i + 1], r[i + 2]);
						v = rotation.xform(v);
						w[n++] = v.x;
						w[n++] = v.y;
						w[n++] = v.z;
						w[n++] = r[i + 3]; // no need to update this...
					}
				}

				surface_arr[ArrayMesh::ARRAY_TANGENT] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_TANGENT] = Variant();
			}

			// Duplicate color
			if (input_surface[ArrayMesh::ARRAY_COLOR].get_type() == Variant::POOL_COLOR_ARRAY) {
				PoolVector<Color> input = input_surface[ArrayMesh::ARRAY_COLOR];
				PoolVector<Color> output;

				// printf("Duplicating color %i times\n", copies);

				for (int copy = 0; copy < copies; copy++) {
					output.append_array(input);
				}

				surface_arr[ArrayMesh::ARRAY_COLOR] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_COLOR] = Variant();
			}

			// Duplicate uv (should I change this to vec3? or support both?)
			if (input_surface[ArrayMesh::ARRAY_TEX_UV].get_type() == Variant::POOL_VECTOR2_ARRAY) {
				PoolVector<Vector2> input = input_surface[ArrayMesh::ARRAY_TEX_UV];
				PoolVector<Vector2> output;

				// printf("Duplicating uvs %i times\n", copies);

				for (int copy = 0; copy < copies; copy++) {
					output.append_array(input);
				}

				surface_arr[ArrayMesh::ARRAY_TEX_UV] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_TEX_UV] = Variant();
			}

			// Duplicate uv2 (should I change this to vec3? or support both?)
			if (input_surface[ArrayMesh::ARRAY_TEX_UV2].get_type() == Variant::POOL_VECTOR2_ARRAY) {
				PoolVector<Vector2> input = input_surface[ArrayMesh::ARRAY_TEX_UV2];
				PoolVector<Vector2> output;

				// printf("Duplicating uv2s %i times\n", copies);

				for (int copy = 0; copy < copies; copy++) {
					output.append_array(input);
				}

				surface_arr[ArrayMesh::ARRAY_TEX_UV2] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_TEX_UV2] = Variant();
			}

			// Duplicate bones
			if (input_surface[ArrayMesh::ARRAY_BONES].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> input = input_surface[ArrayMesh::ARRAY_BONES];
				PoolVector<real_t> output;

				// printf("Duplicating bones %i times\n", copies);

				for (int copy = 0; copy < copies; copy++) {
					output.append_array(input);
				}

				surface_arr[ArrayMesh::ARRAY_BONES] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_BONES] = Variant();
			}

			// Duplicate weights
			if (input_surface[ArrayMesh::ARRAY_WEIGHTS].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> input = input_surface[ArrayMesh::ARRAY_WEIGHTS];
				PoolVector<real_t> output;

				// printf("Duplicating bones %i times\n", copies);

				for (int copy = 0; copy < copies; copy++) {
					output.append_array(input);
				}

				surface_arr[ArrayMesh::ARRAY_WEIGHTS] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_WEIGHTS] = Variant();
			}

			// Duplicate indices
			if (input_surface[ArrayMesh::ARRAY_INDEX].get_type() == Variant::POOL_INT_ARRAY) {
				PoolVector<int> input = input_surface[ArrayMesh::ARRAY_INDEX];
				PoolVector<int> output;

				int n = 0;
				int size = input.size();

				// printf("Duplicating %i indices %i times\n", size, copies);

				output.resize(size * copies);
				PoolVector<int>::Write w = output.write();
				PoolVector<int>::Read r = input.read();
				for (int copy = 0; copy < copies; copy++) {
					int offset = copy * strafe;
					for (int i = 0; i < size; i++) {
						w[n++] = r[i] + offset;
					}
				}

				surface_arr[ArrayMesh::ARRAY_INDEX] = output;
			} else {
				surface_arr[ArrayMesh::ARRAY_INDEX] = Variant();
			}

			// free
			memfree(rots);
		}
	}

	return updated;
}

int GDProcPlaceOnPath::get_input_connector_count() const {
	return 4;
}

Variant::Type GDProcPlaceOnPath::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::ARRAY;
	} else if (p_slot == 1) {
		// we don't have a POOL_QUAD_ARRAY, abusing color for storing
		return Variant::POOL_COLOR_ARRAY;
	} else if (p_slot == 2) {
		return Variant::POOL_VECTOR3_ARRAY;
	} else if (p_slot == 3) {
		return Variant::POOL_VECTOR3_ARRAY;
	}

	return Variant::NIL;
}

String GDProcPlaceOnPath::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("surface");
	} else if (p_slot == 1) {
		return String("rotations");
	} else if (p_slot == 2) {
		return String("translations");
	} else if (p_slot == 3) {
		return String("scales");
	}

	return String();
}

int GDProcPlaceOnPath::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcPlaceOnPath::get_output_connector_type(int p_slot) const {
	return Variant::ARRAY;
}

String GDProcPlaceOnPath::get_output_connector_name(int p_slot) const {
	return String("surface");
}

Variant GDProcPlaceOnPath::get_output(int p_slot) const {
	return Variant(surface_arr);
}
