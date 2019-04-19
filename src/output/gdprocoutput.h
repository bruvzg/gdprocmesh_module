/*************************************************************************/
/*  gdprocoutput.h                                                       */
/*************************************************************************/

#ifndef GD_PROC_OUTPUT_H
#define GD_PROC_OUTPUT_H

#include "src/gdprocnode.h"

class GDProcOutput : public GDProcNode {
	GDCLASS(GDProcOutput, GDProcNode)

private:
	Array output_arr;

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	virtual bool update(bool p_inputs_updated, const Array &p_inputs);

	virtual int get_input_connector_count() const; // returns the number of input connectors this note has
	virtual Variant::Type get_input_connector_type(int p_slot) const; // returns the type of the data expected for this input
	virtual String get_input_connector_name(int p_slot) const; // returns the name for this input

	// Here we make a special exception.
	// We don't declare our outputs because our user can't link up the output
	// but we do need to access our final surface array.
	virtual int get_output_connector_count() const;
	virtual Variant get_output(int p_slot) const;
};

#endif /* !GD_PROC_OUTPUT_H */
