/*************************************************************************/
/*  gdproccircle.cpp                                                     */
/*************************************************************************/

#include "src/shapes/gdproccircle.h"

void GDProcCircle::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_radius"), &GDProcCircle::get_radius);
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &GDProcCircle::set_radius);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius"), "set_radius", "get_radius");

	ClassDB::bind_method(D_METHOD("get_segments"), &GDProcCircle::get_segments);
	ClassDB::bind_method(D_METHOD("set_segments", "segments"), &GDProcCircle::set_segments);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "segments"), "set_segments", "get_segments");
}

String GDProcCircle::get_type_name() const {
	return RTR_LOCAL("Circle");
}

String GDProcCircle::get_description() const {
	return RTR_LOCAL("Creates a circle on a 2d plane with a set radius.\nSegments defines the amount of edges the circle has.");
}

void GDProcCircle::set_radius(float p_radius) {
	if (default_radius != p_radius) {
		default_radius = p_radius;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcCircle::get_radius() const {
	return default_radius;
}

void GDProcCircle::set_segments(int p_segments) {
	if (default_segments != p_segments) {
		default_segments = p_segments;
		must_update = true;
		emit_signal("changed");
	}
}

int GDProcCircle::get_segments() const {
	return default_segments;
}

bool GDProcCircle::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		float radius = default_radius;
		int segments = default_segments;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> r = p_inputs[0];
				if (r.size() > 0) {
					radius = r[0];
				}
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

		// prepare our data
		vertices.resize(segments);
		{
			PoolVector<Vector3>::Write wv = vertices.write();
			float two_pi = 2.0f * Math_PI;
			float step = two_pi / segments;
			float angle = 0.0;

			for (int s = 0; s < segments; s++) {
				wv[s].x = radius * sin(angle);
				wv[s].y = -radius * cos(angle);
				wv[s].z = 0.0f; // should already be zero but just in case..

				angle += step;
			}
		}
	}

	return updated;
}

int GDProcCircle::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcCircle::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_REAL_ARRAY;
	} else if (p_slot == 1) {
		return Variant::POOL_INT_ARRAY;
	}

	return Variant::NIL;
}

String GDProcCircle::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("radius");
	} else if (p_slot == 1) {
		return String("segments");
	}

	return String();
}

String GDProcCircle::get_connector_property_name(int p_slot) const {
	if (p_slot == 0) {
		return String("radius");
	} else if (p_slot == 1) {
		return String("segments");
	}

	return String();
}

int GDProcCircle::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcCircle::get_output_connector_type(int p_slot) const {
	switch (p_slot) {
		case 0:
			return Variant::POOL_VECTOR3_ARRAY;
		default:
			return Variant::NIL;
	}
}

String GDProcCircle::get_output_connector_name(int p_slot) const {
	switch (p_slot) {
		case 0:
			return String("vertices");
		default:
			return String();
	}
}

Variant GDProcCircle::get_output(int p_slot) const {
	switch (p_slot) {
		case 0:
			return Variant(vertices);
		default:
			return Variant();
	}
}

GDProcCircle::GDProcCircle() {
	// default size
	default_radius = 1.0;
	default_segments = 16;
}
