/*************************************************************************/
/*  gdprocsimplify.h                                                     */
/*************************************************************************/

#ifndef GD_PROC_SIMPLIFY_H
#define GD_PROC_SIMPLIFY_H

#include "src/gdprocnode.h"

class GDProcSimplify : public GDProcNode {
	GDCLASS(GDProcSimplify, GDProcNode)

private:
	Array surface_arr;
	float reduction;

	int find_vertice(float p_x, float p_y, float p_z);

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	void set_reduction(float p_reduction);
	float get_reduction() const;

	virtual bool update(bool p_inputs_updated, const Array &p_inputs);

	virtual int get_input_connector_count() const; // returns the number of input connectors this note has
	virtual Variant::Type get_input_connector_type(int p_slot) const; // returns the type of the data expected for this input
	virtual String get_input_connector_name(int p_slot) const; // returns the name for this input

	virtual String get_connector_property_name(int p_slot) const; // if we want an editable field for this slot, returns the name of the related property

	virtual int get_output_connector_count() const; // returns the number of output connectors this node has
	virtual Variant::Type get_output_connector_type(int p_slot) const; // returns the type of the data that is output by this output
	virtual String get_output_connector_name(int p_slot) const; // returns the name for this output

	// get our actual output for a connector, this should only be called after update has run!
	virtual Variant get_output(int p_slot) const; // returns the output data itself

	GDProcSimplify();
};

#endif /* !GD_PROC_SIMPLIFY_H */
