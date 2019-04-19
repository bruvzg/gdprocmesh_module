/*************************************************************************/
/*  gdprocline.cpp                                                       */
/*************************************************************************/

#include "src/shapes/gdprocline.h"

void GDProcLine::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_vector"), &GDProcLine::get_vector);
	ClassDB::bind_method(D_METHOD("set_vector", "vector"), &GDProcLine::set_vector);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "vector"), "set_vector", "get_vector");

	ClassDB::bind_method(D_METHOD("get_segments"), &GDProcLine::get_segments);
	ClassDB::bind_method(D_METHOD("set_segments", "segments"), &GDProcLine::set_segments);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "segments"), "set_segments", "get_segments");
}

String GDProcLine::get_type_name() const {
	return RTR_LOCAL("Line");
}

String GDProcLine::get_description() const {
	return RTR_LOCAL("Creates a line by repeating a vector over a number of segments.");
}

void GDProcLine::set_vector(Vector3 p_vector) {
	if (default_vector != p_vector) {
		default_vector = p_vector;
		must_update = true;
		emit_signal("changed");
	}
}

Vector3 GDProcLine::get_vector() const {
	return default_vector;
}

void GDProcLine::set_segments(int p_segments) {
	if (default_segments != p_segments) {
		default_segments = p_segments;
		must_update = true;
		emit_signal("changed");
	}
}

int GDProcLine::get_segments() const {
	return default_segments;
}

bool GDProcLine::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		int num_values = 0;
		PoolVector<Vector3> input_values;
		int segments = default_segments;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				input_values = p_inputs[0];
				num_values = input_values.size();
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_INT_ARRAY) {
				PoolVector<int> s = p_inputs[1];
				if (s.size() > 0) {
					segments = s[0];
				}
			}
		}

		if (num_values == 0) {
			input_values.push_back(default_vector);
			num_values++;
		}

		// prepare our data
		vertices.resize(segments + 1);
		{
			int i = 0;
			Vector3 v = Vector3(0.0, 0.0, 0.0);

			PoolVector<Vector3>::Write wv = vertices.write();
			PoolVector<Vector3>::Read r = input_values.read();

			wv[i++] = v;
			for (int s = 0; s < segments; s++) {
				v += r[s % num_values];
				wv[i++] = v;
			}
		}
	}

	return updated;
}

int GDProcLine::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcLine::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_VECTOR3_ARRAY;
	} else if (p_slot == 1) {
		return Variant::POOL_INT_ARRAY;
	}

	return Variant::NIL;
}

String GDProcLine::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("vectors");
	} else if (p_slot == 1) {
		return String("segments");
	}

	return String();
}

String GDProcLine::get_connector_property_name(int p_slot) const {
	if (p_slot == 0) {
		return String("vector");
	} else if (p_slot == 1) {
		return String("segments");
	}

	return String();
}

int GDProcLine::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcLine::get_output_connector_type(int p_slot) const {
	switch (p_slot) {
		case 0:
			return Variant::POOL_VECTOR3_ARRAY;
		default:
			return Variant::NIL;
	}
}

String GDProcLine::get_output_connector_name(int p_slot) const {
	switch (p_slot) {
		case 0:
			return String("vertices");
		default:
			return String();
	}
}

Variant GDProcLine::get_output(int p_slot) const {
	switch (p_slot) {
		case 0:
			return Variant(vertices);
		default:
			return Variant();
	}
}

GDProcLine::GDProcLine() {
	// default size
	default_vector = Vector3(1.0, 0.0, 0.0);
	default_segments = 1;
}
