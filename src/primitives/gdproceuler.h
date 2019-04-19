/*************************************************************************/
/*  gdproceuler.h                                                        */
/*************************************************************************/

#ifndef GD_PROC_EULER_H
#define GD_PROC_EULER_H

#include "src/gdprocnode.h"

class GDProcEuler : public GDProcNode {
	GDCLASS(GDProcEuler, GDProcNode)

private:
	Vector3 defaults;
	PoolVector<Color> value; // we don't have a PoolQuadArray so abuse Color

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	// properties
	void set_x(float x);
	float get_x() const;

	void set_y(float y);
	float get_y() const;

	void set_z(float z);
	float get_z() const;

	virtual bool update(bool p_inputs_updated, const Array &p_inputs); // checks if our node has to be updated and if so, applies our calculations

	virtual int get_input_connector_count() const; // returns the number of input connectors this note has
	virtual Variant::Type get_input_connector_type(int p_slot) const; // returns the type of the data expected for this input
	virtual String get_input_connector_name(int p_slot) const; // returns the name for this input

	virtual String get_connector_property_name(int p_slot) const; // if we want an editable field for this slot, returns the name of the related property

	virtual int get_output_connector_count() const; // returns the number of output connectors this node has
	virtual Variant::Type get_output_connector_type(int p_slot) const; // returns the type of the data that is output by this output
	virtual String get_output_connector_name(int p_slot) const; // returns the name for this output

	virtual Variant get_output(int p_slot) const; // returns the output data itself

	GDProcEuler();
};

#endif /* !GD_PROC_EULER_H */
