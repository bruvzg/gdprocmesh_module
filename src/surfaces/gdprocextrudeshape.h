/*************************************************************************/
/*  gdprocextrudeshape.h                                                 */
/*************************************************************************/

#ifndef GD_PROC_EXTRUDE_H
#define GD_PROC_EXTRUDE_H

#include "src/gdprocnode.h"

class GDProcExtrudeShape : public GDProcNode {
	GDCLASS(GDProcExtrudeShape, GDProcNode)

private:
	bool shape_is_closed;
	bool path_is_closed;
	bool path_follow;

	Array surface_arr;

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	void set_shape_is_closed(bool p_is_closed);
	bool get_shape_is_closed() const;

	void set_path_is_closed(bool p_is_closed);
	bool get_path_is_closed() const;

	void set_path_follow(bool p_follow);
	bool get_path_follow() const;

	virtual bool update(bool p_inputs_updated, const Array &p_inputs);

	virtual int get_input_connector_count() const; // returns the number of input connectors this note has
	virtual Variant::Type get_input_connector_type(int p_slot) const; // returns the type of the data expected for this input
	virtual String get_input_connector_name(int p_slot) const; // returns the name for this input

	virtual String get_connector_property_name(int p_slot) const; // if we want an editable field for this slot, returns the name of the related property

	virtual int get_output_connector_count() const; // returns the number of output connectors this node has
	virtual Variant::Type get_output_connector_type(int p_slot) const; // returns the type of the data that is output by this output
	virtual String get_output_connector_name(int p_slot) const; // returns the name for this output

	virtual Variant get_output(int p_slot) const; // returns the output data itself

	GDProcExtrudeShape();
};

#endif /* !GD_PROC_EXTRUDE_H */
