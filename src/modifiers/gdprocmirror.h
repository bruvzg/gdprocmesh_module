/*************************************************************************/
/*  gdprocmirror.h                                                       */
/*************************************************************************/

#ifndef GD_PROC_MIRROR_H
#define GD_PROC_MIRROR_H

#include "src/gdprocnode.h"

class GDProcMirror : public GDProcNode {
	GDCLASS(GDProcMirror, GDProcNode)

private:
	bool default_mirror_x;
	bool default_mirror_y;
	bool default_mirror_z;
	bool default_duplicate;

	Array surface_arr;

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	// properties
	void set_mirror_x(bool p_mirror);
	bool get_mirror_x();

	void set_mirror_y(bool p_mirror);
	bool get_mirror_y();

	void set_mirror_z(bool p_mirror);
	bool get_mirror_z();

	void set_duplicate(bool p_duplicate);
	bool get_duplicate();

	virtual bool update(bool p_inputs_updated, const Array &p_inputs); // checks if our node has to be updated and if so, applies our calculations

	virtual int get_input_connector_count() const; // returns the number of input connectors this note has
	virtual Variant::Type get_input_connector_type(int p_slot) const; // returns the type of the data expected for this input
	virtual String get_input_connector_name(int p_slot) const; // returns the name for this input

	virtual String get_connector_property_name(int p_slot) const; // if we want an editable field for this slot, returns the name of the related property

	virtual int get_output_connector_count() const; // returns the number of output connectors this node has
	virtual Variant::Type get_output_connector_type(int p_slot) const; // returns the type of the data that is output by this output
	virtual String get_output_connector_name(int p_slot) const; // returns the name for this output

	virtual Variant get_output(int p_slot) const; // returns the output data itself

	GDProcMirror();
};

#endif /* !GD_PROC_MIRROR_H */
