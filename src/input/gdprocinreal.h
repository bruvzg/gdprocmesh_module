/*************************************************************************/
/*  gdprocinreal.h                                                       */
/*************************************************************************/

#ifndef GD_PROC_INREAL_H
#define GD_PROC_INREAL_H

#include "src/gdprocnode.h"

class GDProcInReal : public GDProcNode {
	GDCLASS(GDProcInReal, GDProcNode)

private:
	PoolVector<real_t> value;

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	virtual bool update(bool p_inputs_updated, const Array &p_inputs); // checks if our node has to be updated and if so, applies our calculations

	virtual Variant::Type get_input_property_type() const; // if this is an input property, what is its type?
	virtual void set_input(Variant p_input); // if this is an input property, set its value
	virtual Variant get_input(); // if this is an input property, get its value

	virtual int get_output_connector_count() const; // returns the number of output connectors this node has
	virtual Variant::Type get_output_connector_type(int p_slot) const; // returns the type of the data that is output by this output
	virtual String get_output_connector_name(int p_slot) const; // returns the name for this output

	virtual Variant get_output(int p_slot) const; // returns the output data itself

	GDProcInReal();
};

#endif /* !GD_PROC_INREAL_H */
