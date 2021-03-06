/*************************************************************************/
/*  gdprocgennormals.h                                                   */
/*************************************************************************/

#ifndef GD_PROC_GENNORMALS_H
#define GD_PROC_GENNORMALS_H

#include "src/gdprocnode.h"

class GDProcGenNormals : public GDProcNode {
	GDCLASS(GDProcGenNormals, GDProcNode)

private:
	PoolVector<Vector3> normals;

protected:
	static void _bind_methods();

public:
	virtual String get_type_name() const;
	virtual String get_description() const;

	virtual bool update(bool p_inputs_updated, const Array &p_inputs);

	virtual int get_input_connector_count() const; // returns the number of input connectors this note has
	virtual Variant::Type get_input_connector_type(int p_slot) const; // returns the type of the data expected for this input
	virtual String get_input_connector_name(int p_slot) const; // returns the name for this input

	virtual int get_output_connector_count() const; // returns the number of output connectors this node has
	virtual Variant::Type get_output_connector_type(int p_slot) const; // returns the type of the data that is output by this output
	virtual String get_output_connector_name(int p_slot) const; // returns the name for this output

	virtual Variant get_output(int p_slot) const; // returns the output data itself
};

#endif /* !GD_PROC_BOX_H */
