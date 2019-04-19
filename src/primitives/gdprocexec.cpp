/*************************************************************************/
/*  gdprocexec.cpp                                                       */
/*************************************************************************/

#include <core/math/expression.h>

#include "src/primitives/gdprocexec.h"

void GDProcExec::_bind_methods() {

	ClassDB::bind_method(D_METHOD("get_expression"), &GDProcExec::get_expression);
	ClassDB::bind_method(D_METHOD("set_expression", "expression"), &GDProcExec::set_expression);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression"), "set_expression", "get_expression");

	ClassDB::bind_method(D_METHOD("get_a"), &GDProcExec::get_a);
	ClassDB::bind_method(D_METHOD("set_a", "a"), &GDProcExec::set_a);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "a"), "set_a", "get_a");

	ClassDB::bind_method(D_METHOD("get_b"), &GDProcExec::get_b);
	ClassDB::bind_method(D_METHOD("set_b", "b"), &GDProcExec::set_b);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "b"), "set_b", "get_b");

	ClassDB::bind_method(D_METHOD("get_c"), &GDProcExec::get_c);
	ClassDB::bind_method(D_METHOD("set_c", "c"), &GDProcExec::set_c);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "c"), "set_c", "get_c");

	ClassDB::bind_method(D_METHOD("get_d"), &GDProcExec::get_d);
	ClassDB::bind_method(D_METHOD("set_d", "d"), &GDProcExec::set_d);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "d"), "set_d", "get_d");

	ClassDB::bind_method(D_METHOD("get_e"), &GDProcExec::get_e);
	ClassDB::bind_method(D_METHOD("set_e", "e"), &GDProcExec::set_e);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "e"), "set_e", "get_e");
}

String GDProcExec::get_type_name() const {
	return RTR("Execute Expression");
}

String GDProcExec::get_description() const {
	return RTR("Executes an expression against multiple reals.\nUse variables 'a', 'b', 'c', 'd' and 'e' to access the specific array.\nUse 'i' to get the index in the array.");
}

void GDProcExec::set_expression(String p_expression) {
	if (p_expression != default_expression) {
		default_expression = p_expression;
		must_update = true;
		emit_signal("changed");
	}
}

String GDProcExec::get_expression() const {
	return default_expression;
}

void GDProcExec::set_a(real_t a) {
	if (default_a != a) {
		default_a = a;
		must_update = true;
		emit_signal("changed");
	}
}

real_t GDProcExec::get_a() const {
	return default_a;
}

void GDProcExec::set_b(real_t b) {
	if (default_b != b) {
		default_b = b;
		must_update = true;
		emit_signal("changed");
	}
}

real_t GDProcExec::get_b() const {
	return default_b;
}

void GDProcExec::set_c(real_t c) {
	if (default_c != c) {
		default_c = c;
		must_update = true;
		emit_signal("changed");
	}
}

real_t GDProcExec::get_c() const {
	return default_c;
}

void GDProcExec::set_d(real_t d) {
	if (default_d != d) {
		default_d = d;
		must_update = true;
		emit_signal("changed");
	}
}

real_t GDProcExec::get_d() const {
	return default_d;
}

void GDProcExec::set_e(real_t e) {
	if (default_e != e) {
		default_e = e;
		must_update = true;
		emit_signal("changed");
	}
}

real_t GDProcExec::get_e() const {
	return default_e;
}

bool GDProcExec::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update || p_inputs_updated;
	must_update = false;

	if (updated) {
		String expression = default_expression;
		int num_a;
		PoolVector<real_t> a;
		int num_b;
		PoolVector<real_t> b;
		int num_c;
		PoolVector<real_t> c;
		int num_d;
		PoolVector<real_t> d;
		int num_e;
		PoolVector<real_t> e;

		int input_count = p_inputs.size();
		if (input_count > 0 && p_inputs[0].get_type() == Variant::STRING) {
			expression = p_inputs[0];
		}

		if (expression == String()) {
			// No expression. Leave output unmodified
			WARN_PRINTS(RTR("Node '{0}' failed to parse expression. Output will be unmodified.\nError Message: No expression found.")
								.format(varray(get_node_name())));
			return false;
		}

		if (input_count > 1 && p_inputs[1].get_type() == Variant::POOL_REAL_ARRAY && ((PoolVector<real_t>)p_inputs[1]).size() > 0) {
			a = p_inputs[1];
			num_a = a.size();
		} else {
			a.push_back(default_a);
			num_a = 1;
		}

		if (input_count > 2 && p_inputs[2].get_type() == Variant::POOL_REAL_ARRAY && ((PoolVector<real_t>)p_inputs[2]).size() > 0) {
			b = p_inputs[2];
			num_b = b.size();
		} else {
			b.push_back(default_b);
			num_b = 1;
		}

		if (input_count > 3 && p_inputs[3].get_type() == Variant::POOL_REAL_ARRAY && ((PoolVector<real_t>)p_inputs[3]).size() > 0) {
			c = p_inputs[3];
			num_c = c.size();
		} else {
			c.push_back(default_c);
			num_c = 1;
		}

		if (input_count > 4 && p_inputs[4].get_type() == Variant::POOL_REAL_ARRAY && ((PoolVector<real_t>)p_inputs[4]).size() > 0) {
			d = p_inputs[4];
			num_d = d.size();
		} else {
			d.push_back(default_d);
			num_d = 1;
		}

		if (input_count > 5 && p_inputs[5].get_type() == Variant::POOL_REAL_ARRAY && ((PoolVector<real_t>)p_inputs[5]).size() > 0) {
			e = p_inputs[5];
			num_e = e.size();
		} else {
			e.push_back(default_e);
			num_e = 1;
		}

		// Parse expression
		Ref<Expression> executor;
		executor.instance();
		Vector<String> input_names;
		input_names.push_back("i");
		input_names.push_back("a");
		input_names.push_back("b");
		input_names.push_back("c");
		input_names.push_back("d");
		input_names.push_back("e");
		Error err = executor->parse(expression, input_names);
		if (err != OK) {
			// We error'd out. output is unmodified.
			WARN_PRINTS(RTR("Node '{0}' failed to parse expression. Output will be unmodified.\nError Message: {1}")
								.format(varray(get_node_name(), executor->get_error_text())));
			return false;
		}

		// How many reals will we output?
		int max = num_a > num_b ? num_a : num_b;
		max = max > num_c ? max : num_c;
		max = max > num_d ? max : num_d;
		max = max > num_e ? max : num_e;

		// Process
		PoolVector<real_t>::Read ra = a.read();
		PoolVector<real_t>::Read rb = b.read();
		PoolVector<real_t>::Read rc = c.read();
		PoolVector<real_t>::Read rd = d.read();
		PoolVector<real_t>::Read re = e.read();

		values.resize(max);
		PoolVector<real_t>::Write vw = values.write();
		Array inputs;
		for (int i = 0; i < max; i++) {
			inputs.push_back((float)i);
			inputs.push_back(ra[i % num_a]);
			inputs.push_back(rb[i % num_b]);
			inputs.push_back(rc[i % num_c]);
			inputs.push_back(rd[i % num_d]);
			inputs.push_back(re[i % num_e]);
			// Ideally '*executor' should be replaced with NULL but passing NULL crashes Godot so this is a workaround
			Variant v = executor->execute(inputs, *executor, false);
			if (executor->has_execute_failed()) {
				// If first execution fails, all of them will fail so exit early
				WARN_PRINTS(RTR("Node '{0}' failed to execute expression. Output will be unmodified.\nError Message: {1}")
									.format(varray(get_node_name(), executor->get_error_text())));
				return false;
			} else {
				vw[i] = v;
			}
			inputs.clear();
		}
	}

	return updated;
}

int GDProcExec::get_input_connector_count() const {
	return 6;
}

Variant::Type GDProcExec::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::STRING;
	} else {
		return Variant::POOL_REAL_ARRAY;
	}
}

String GDProcExec::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return String("expression");
	} else if (p_slot == 1) {
		return String("a");
	} else if (p_slot == 2) {
		return String("b");
	} else if (p_slot == 3) {
		return String("c");
	} else if (p_slot == 4) {
		return String("d");
	} else if (p_slot == 5) {
		return String("e");
	}

	return String();
}

String GDProcExec::get_connector_property_name(int p_slot) const {
	if (p_slot == 0) {
		return String("expression");
	} else if (p_slot == 1) {
		return String("a");
	} else if (p_slot == 2) {
		return String("b");
	} else if (p_slot == 3) {
		return String("c");
	} else if (p_slot == 4) {
		return String("d");
	} else if (p_slot == 5) {
		return String("e");
	}

	return String();
}

int GDProcExec::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcExec::get_output_connector_type(int p_slot) const {
	return Variant::POOL_REAL_ARRAY;
}

String GDProcExec::get_output_connector_name(int p_slot) const {
	return String("values");
}

Variant GDProcExec::get_output(int p_slot) const {
	return values;
}

GDProcExec::GDProcExec() {
	default_expression = "";
	default_a = 0.0;
	default_b = 0.0;
	default_c = 0.0;
	default_d = 0.0;
	default_e = 0.0;
}
