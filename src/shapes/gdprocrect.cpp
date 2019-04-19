/*************************************************************************/
/*  gdprocrect.cpp                                                       */
/*************************************************************************/

#include "src/shapes/gdprocrect.h"

void GDProcRect::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_width"), &GDProcRect::get_width);
	ClassDB::bind_method(D_METHOD("set_width", "width"), &GDProcRect::set_width);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "width"), "set_width", "get_width");

	ClassDB::bind_method(D_METHOD("get_height"), &GDProcRect::get_height);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &GDProcRect::set_height);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "height"), "set_height", "get_height");
}

String GDProcRect::get_type_name() const {
	return RTR("Rectangle");
}

String GDProcRect::get_description() const {
	return RTR("Creates a rect on a 2d plane with a set width and height.");
}

void GDProcRect::set_width(float p_width) {
	if (default_width != p_width) {
		default_width = p_width;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcRect::get_width() const {
	return default_width;
}

void GDProcRect::set_height(float p_height) {
	if (default_height != p_height) {
		default_height = p_height;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcRect::get_height() const {
	return default_height;
}

bool GDProcRect::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		float width = default_width;
		float height = default_height;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> w = p_inputs[0];
				if (w.size() > 0) {
					width = w[0];
				}
			}
		}
		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_REAL_ARRAY) {
				PoolVector<real_t> h = p_inputs[1];
				if (h.size() > 0) {
					height = h[0];
				}
			}
		}

		// prepare our data
		vertices.resize(4);
		{
			PoolVector<Vector3>::Write wv = vertices.write();
			float hsx = width / 2.0f;
			float hsy = height / 2.0f;

			wv[0].x = -hsx;
			wv[0].y = -hsy;
			wv[0].z = 0.0f;
			wv[1].x = hsx;
			wv[1].y = -hsy;
			wv[1].z = 0.0f;
			wv[2].x = hsx;
			wv[2].y = hsy;
			wv[2].z = 0.0f;
			wv[3].x = -hsx;
			wv[3].y = hsy;
			wv[3].z = 0.0f;
		}
	}

	return updated;
}

int GDProcRect::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcRect::get_input_connector_type(int p_slot) const {
	return Variant::POOL_REAL_ARRAY;
}

String GDProcRect::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("width");
	} else if (p_slot == 1) {
		return String("height");
	}

	return String();
}

String GDProcRect::get_connector_property_name(int p_slot) const {
	if (p_slot == 0) {
		return String("width");
	} else if (p_slot == 1) {
		return String("height");
	}

	return String();
}

int GDProcRect::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcRect::get_output_connector_type(int p_slot) const {
	switch (p_slot) {
		case 0:
			return Variant::POOL_VECTOR3_ARRAY;
		default:
			return Variant::NIL;
	}
}

String GDProcRect::get_output_connector_name(int p_slot) const {
	switch (p_slot) {
		case 0:
			return String("vertices");
		default:
			return String();
	}
}

Variant GDProcRect::get_output(int p_slot) const {
	switch (p_slot) {
		case 0:
			return Variant(vertices);
		default:
			return Variant();
	}
}

GDProcRect::GDProcRect() {
	// default size
	default_width = 1.0;
	default_height = 1.0;
}
