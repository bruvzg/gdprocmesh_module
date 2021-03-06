/*************************************************************************/
/*  gdprocbevel.cpp                                                      */
/*************************************************************************/

#include "src/transforms/gdprocbevel.h"

void GDProcBevel::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_distance"), &GDProcBevel::get_distance);
	ClassDB::bind_method(D_METHOD("set_distance", "distance"), &GDProcBevel::set_distance);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "distance"), "set_distance", "get_distance");

	ClassDB::bind_method(D_METHOD("get_iterations"), &GDProcBevel::get_iterations);
	ClassDB::bind_method(D_METHOD("set_iterations", "iterations"), &GDProcBevel::set_iterations);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "iterations"), "set_iterations", "get_iterations");

	ClassDB::bind_method(D_METHOD("get_is_closed"), &GDProcBevel::get_is_closed);
	ClassDB::bind_method(D_METHOD("set_is_closed", "is_closed"), &GDProcBevel::set_is_closed);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_closed"), "set_is_closed", "get_is_closed");
}

String GDProcBevel::get_type_name() const {
	return RTR_LOCAL("Bevel");
}

String GDProcBevel::get_description() const {
	return RTR_LOCAL("Bevels a 2D path to round each corner.");
}

void GDProcBevel::set_distance(float p_distance) {
	if (distance != p_distance) {
		distance = p_distance;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcBevel::get_distance() const {
	return distance;
}

void GDProcBevel::set_iterations(int p_iterations) {
	if (p_iterations <= 0) {
		ERR_EXPLAIN(RTR_LOCAL("Can't set iterations to ") + String::num_int64(p_iterations));
		ERR_FAIL();
	} else if (p_iterations > 10) {
		ERR_EXPLAIN(RTR_LOCAL("Can't set iterations to ") + String::num_int64(p_iterations));
		ERR_FAIL();
	}

	if (iterations != p_iterations) {
		iterations = p_iterations;
		must_update = true;
		emit_signal("changed");
	}
}

int GDProcBevel::get_iterations() const {
	return iterations;
}

void GDProcBevel::set_is_closed(bool p_is_closed) {
	if (is_closed != p_is_closed) {
		is_closed = p_is_closed;
		must_update = true;
		emit_signal("changed");
	}
}

bool GDProcBevel::get_is_closed() const {
	return is_closed;
}

void GDProcBevel::do_bevel(PoolVector<Vector3>::Write &p_w, Vector3 p1, Vector3 p2, Vector3 p3, float p_distance, int p_iterations, int &p_idx) {
	Vector3 n = (p1 - p2).normalized() * p_distance;
	Vector3 a = p2 + n;
	n = (p3 - p2).normalized() * p_distance;
	Vector3 b = p2 + n;

	if (p_iterations > 1) {
		float new_dist = (b - a).length() / 4.0f;

		do_bevel(p_w, p1, a, b, new_dist, p_iterations - 1, p_idx);
		do_bevel(p_w, a, b, p3, new_dist, p_iterations - 1, p_idx);
	} else {
		// printf("Adding %i -> %0.2f, %0.2f, %0.2f\n", p_idx, a.x, a.y, a.z);
		p_w[p_idx++] = a;
		// printf("Adding %i -> %0.2f, %0.2f, %0.2f\n", p_idx, b.x, b.y, b.z);
		p_w[p_idx++] = b;
	}
}

bool GDProcBevel::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		float d = distance;
		int itr = iterations;
		int closed = is_closed;

		int num_vectors = 0;
		PoolVector<Vector3> input_vectors;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				input_vectors = p_inputs[0];
				num_vectors = input_vectors.size();
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> input = p_inputs[1];
				if (input.size() > 0) {
					d = input[0];
				}
			}
		}
		if (input_count > 2) {
			if (p_inputs[2].get_type() == Variant::POOL_INT_ARRAY) {
				PoolVector<int> input = p_inputs[2];
				if (input.size() > 0) {
					itr = input[0];
				}
			}
		}
		if (input_count > 3) {
			if (p_inputs[3].get_type() == Variant::BOOL) {
				closed = p_inputs[3];
			}
		}

		if (num_vectors > 0) {
			int o = 0;
			int cnt = 1;
			for (int i = 0; i < itr; i++) {
				cnt *= 2;
			}
			if (closed) {
				cnt = num_vectors * cnt;
			} else {
				cnt = 2 + ((num_vectors - 2) * cnt);
			}
			// printf("Input points: %i\nIterations: %i\nNew number of points: %i\n", num_vectors, itr, cnt);
			vectors.resize(cnt);

			PoolVector<Vector3>::Write w = vectors.write();
			PoolVector<Vector3>::Read r = input_vectors.read();

			if (closed) {
				for (int b = 0; b < num_vectors; b++) {
					int a = b == 0 ? num_vectors - 1 : (b - 1);
					int c = (b + 1) % num_vectors;

					do_bevel(w, r[a], r[b], r[c], d, itr, o);
				}
			} else {
				// copy our first one
				w[o++] = r[0];

				for (int i = 1; i < num_vectors - 1; i++) {
					do_bevel(w, r[i - 1], r[i], r[i + 1], d, itr, o);
				}

				// copy our last one
				w[o++] = r[num_vectors - 1];
			}

		} else {
			vectors.resize(0);
		}
	}

	return updated;
}

int GDProcBevel::get_input_connector_count() const {
	return 4;
}

Variant::Type GDProcBevel::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_VECTOR3_ARRAY;
	} else if (p_slot == 1) {
		return Variant::POOL_REAL_ARRAY;
	} else if (p_slot == 2) {
		return Variant::POOL_INT_ARRAY;
	} else if (p_slot == 3) {
		return Variant::BOOL;
	}
	return Variant::NIL;
}

String GDProcBevel::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("vectors");
	} else if (p_slot == 1) {
		return String("distance");
	} else if (p_slot == 2) {
		return String("iterations");
	} else if (p_slot == 3) {
		return String("is_closed");
	}

	return String();
}

String GDProcBevel::get_connector_property_name(int p_slot) const {
	if (p_slot == 1) {
		return String("distance");
	} else if (p_slot == 2) {
		return String("iterations");
	} else if (p_slot == 3) {
		return String("is_closed");
	}

	return String();
}

int GDProcBevel::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcBevel::get_output_connector_type(int p_slot) const {
	return Variant::POOL_VECTOR3_ARRAY;
}

String GDProcBevel::get_output_connector_name(int p_slot) const {
	return String("vectors");
}

Variant GDProcBevel::get_output(int p_slot) const {
	return Variant(vectors);
}

GDProcBevel::GDProcBevel() {
	// defaults
	distance = 0.1f;
	iterations = 1;
	is_closed = false;
}
