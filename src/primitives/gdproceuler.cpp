/*************************************************************************/
/*  gdproceuler.h                                                        */
/*************************************************************************/

#include "src/primitives/gdproceuler.h"

void GDProcEuler::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_x"), &GDProcEuler::get_x);
	ClassDB::bind_method(D_METHOD("set_x", "x"), &GDProcEuler::set_x);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "x"), "set_x", "get_x");

	ClassDB::bind_method(D_METHOD("get_y"), &GDProcEuler::get_y);
	ClassDB::bind_method(D_METHOD("set_y", "y"), &GDProcEuler::set_y);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "y"), "set_y", "get_y");

	ClassDB::bind_method(D_METHOD("get_z"), &GDProcEuler::get_z);
	ClassDB::bind_method(D_METHOD("set_z", "z"), &GDProcEuler::set_z);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "z"), "set_z", "get_z");
}

String GDProcEuler::get_type_name() const {
	return RTR_LOCAL("Euler Angles");
}

String GDProcEuler::get_description() const {
	return RTR_LOCAL("Helper node to combine a rotation around the x, y and z axis (euler angles) into a quarternion.");
}

void GDProcEuler::set_x(float x) {
	if (defaults.x != x) {
		defaults.x = x;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcEuler::get_x() const {
	return defaults.x;
}

void GDProcEuler::set_y(float y) {
	if (defaults.y != y) {
		defaults.y = y;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcEuler::get_y() const {
	return defaults.y;
}

void GDProcEuler::set_z(float z) {
	if (defaults.z != z) {
		defaults.z = z;
		must_update = true;
		emit_signal("changed");
	}
}

float GDProcEuler::get_z() const {
	return defaults.z;
}

bool GDProcEuler::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		PoolVector<real_t> x;
		PoolVector<real_t> y;
		PoolVector<real_t> z;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_REAL_ARRAY) {
				x = p_inputs[0];
			}
		}

		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_REAL_ARRAY) {
				y = p_inputs[1];
			}
		}

		if (input_count > 2) {
			if (p_inputs[2].get_type() == Variant::POOL_REAL_ARRAY) {
				z = p_inputs[2];
			}
		}

		// check our input and add defaults where we need them
		int num_x = x.size();
		if (num_x == 0) {
			x.push_back(defaults.x);
			num_x++;
		}

		int num_y = y.size();
		if (num_y == 0) {
			y.push_back(defaults.y);
			num_y++;
		}

		int num_z = z.size();
		if (num_z == 0) {
			z.push_back(defaults.z);
			num_z++;
		}

		// how many vertices will we output?
		int max = num_x > num_y ? num_x : num_y;
		if (max < num_z) {
			max = num_z;
		}

		// and lets process...
		PoolVector<real_t>::Read rx = x.read();
		PoolVector<real_t>::Read ry = y.read();
		PoolVector<real_t>::Read rz = z.read();

		value.resize(max);
		PoolVector<Color>::Write vw = value.write();

		for (int i = 0; i < max; i++) {
			float pi_180 = Math_PI / 180.0f;
			Quat q;
			q.set_euler_xyz(Vector3(rx[i % num_x], ry[i % num_y], rz[i % num_z]) * Vector3(pi_180, pi_180, pi_180));

			vw[i] = Color(q.x, q.y, q.z, q.w);
		}
	}

	return updated;
}

int GDProcEuler::get_input_connector_count() const {
	return 3;
}

Variant::Type GDProcEuler::get_input_connector_type(int p_slot) const {
	return Variant::POOL_REAL_ARRAY;
}

String GDProcEuler::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("x");
	} else if (p_slot == 1) {
		return String("y");
	} else if (p_slot == 2) {
		return String("z");
	}

	return String();
}

String GDProcEuler::get_connector_property_name(int p_slot) const {
	if (p_slot == 0) {
		return String("x");
	} else if (p_slot == 1) {
		return String("y");
	} else if (p_slot == 2) {
		return String("z");
	}

	return String();
}

int GDProcEuler::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcEuler::get_output_connector_type(int p_slot) const {
	return Variant::POOL_COLOR_ARRAY;
}

String GDProcEuler::get_output_connector_name(int p_slot) const {
	return String("rotations");
}

Variant GDProcEuler::get_output(int p_slot) const {
	return Variant(value);
}

GDProcEuler::GDProcEuler() {
	// init our buffer
	value.resize(1);
	value.set(0, Color(0.0, 0.0, 0.0, 1.0));
}
