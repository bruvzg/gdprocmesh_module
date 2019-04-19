/*************************************************************************/
/*  gdprocredist.h                                                       */
/*************************************************************************/

#ifndef GD_PROC_REDIST_H
#define GD_PROC_REDIST_H

#include "src/gdprocnode.h"

class GDProcRedist : public GDProcNode {
	GDCLASS(GDProcRedist, GDProcNode)

private:
	// input
	float default_offset;
	float default_strafe;
	bool default_is_closed;
	bool default_follow;

	// output
	PoolVector<Vector3> vectors;
	PoolVector<Color> rotations; // we don't have a PoolQuadArray so abuse Color

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	void set_is_closed(bool p_is_closed);
	bool get_is_closed() const;

	void set_follow(bool p_follow);
	bool get_follow() const;

	// properties
	void set_offset(float p_offset);
	float get_offset();

	void set_strafe(float p_strafe);
	float get_strafe();

	virtual bool update(bool p_inputs_updated, const Array &p_inputs); // checks if our node has to be updated and if so, applies our calculations

	virtual int get_input_connector_count() const; // returns the number of input connectors this note has
	virtual Variant::Type get_input_connector_type(int p_slot) const; // returns the type of the data expected for this input
	virtual String get_input_connector_name(int p_slot) const; // returns the name for this input

	virtual String get_connector_property_name(int p_slot) const; // if we want an editable field for this slot, returns the name of the related property

	virtual int get_output_connector_count() const; // returns the number of output connectors this node has
	virtual Variant::Type get_output_connector_type(int p_slot) const; // returns the type of the data that is output by this output
	virtual String get_output_connector_name(int p_slot) const; // returns the name for this output

	virtual Variant get_output(int p_slot) const; // returns the output data itself

	GDProcRedist();
};

#endif /* !GD_PROC_REDIST_H */
