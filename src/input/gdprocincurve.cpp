/*************************************************************************/
/*  gdprocincurve.cpp                                                    */
/*************************************************************************/

#include "src/input/gdprocincurve.h"

void GDProcInCurve::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_curve_changed"), &GDProcInCurve::_curve_changed);
}

String GDProcInCurve::get_type_name() const {
	return RTR_LOCAL("Input Curve");
}

String GDProcInCurve::get_description() const {
	return RTR_LOCAL("Allows you to specify a Curve3D as input for the procedural mesh.\nCan be set from the editor.\nOutputs vertices at the baked interval.");
}

bool GDProcInCurve::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		// parse our curve into vertices, couldn't be simpler...
		if (curve.is_valid()) {
			value = curve->get_baked_points();
		} else {
			value.resize(0);
		}
	}

	return updated;
}

Variant::Type GDProcInCurve::get_input_property_type() const {
	return Variant::OBJECT;
}

String GDProcInCurve::get_input_property_hint() const {
	return String("Curve3D");
}

void GDProcInCurve::set_input(Variant p_input) {
	if (p_input.get_type() != Variant::OBJECT) {
		ERR_EXPLAIN(RTR_LOCAL("Not an object ") + Variant::get_type_name(p_input.get_type()));
		ERR_FAIL();
	}

	Ref<Curve3D> new_curve = p_input;
	if (new_curve.is_null()) {
		ERR_EXPLAIN(RTR_LOCAL("Not a valid object (Null)."));
		ERR_FAIL();
	} else if (new_curve->get_class() != "Curve3D") {
		ERR_EXPLAIN(RTR_LOCAL("Not a valid object (Class name)."));
		ERR_FAIL();
	}

	// disconnect from the old...
	if (curve.is_valid()) {
		curve->disconnect("changed", this, "_curve_changed");
	}

	// and update
	curve = new_curve;

	// connect to the new
	if (curve.is_valid()) {
		curve->connect("changed", this, "_curve_changed");
	}

	must_update = true;
	emit_signal("changed");
}

Variant GDProcInCurve::get_input() {
	if (curve.is_valid()) {
		return Variant(curve);
	}
	return Variant();
}

void GDProcInCurve::_curve_changed() {
	must_update = true;
	emit_signal("changed");
}

int GDProcInCurve::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcInCurve::get_output_connector_type(int p_slot) const {
	return Variant::POOL_VECTOR3_ARRAY;
}

String GDProcInCurve::get_output_connector_name(int p_slot) const {
	return String("vectors");
}

Variant GDProcInCurve::get_output(int p_slot) const {
	return Variant(value);
}

GDProcInCurve::~GDProcInCurve() {
	if (curve.is_valid()) {
		curve->disconnect("changed", this, "_curve_changed");
	}
}
