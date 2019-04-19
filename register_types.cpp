/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/

#include "register_types.h"

#include <core/class_db.h>
#include <core/engine.h>
#include <core/os/os.h>

#include "src/gdprocmesh.h"
#include "src/gdprocnode.h"

#include "src/input/gdprocincurve.h"
#include "src/input/gdprocinint.h"
#include "src/input/gdprocinmesh.h"
#include "src/input/gdprocinpoolvectors.h"
#include "src/input/gdprocinreal.h"
#include "src/input/gdprocinvector.h"

#include "src/primitives/gdproccount.h"
#include "src/primitives/gdproceuler.h"
#include "src/primitives/gdprocexec.h"
#include "src/primitives/gdprocrandom.h"
#include "src/primitives/gdprocsplitvector.h"
#include "src/primitives/gdprocvector.h"

#include "src/transforms/gdprocadd.h"
#include "src/transforms/gdprocbevel.h"
#include "src/transforms/gdprocdiv.h"
#include "src/transforms/gdprocgennormals.h"
#include "src/transforms/gdprocmult.h"
#include "src/transforms/gdprocredist.h"
#include "src/transforms/gdprocrotate.h"
#include "src/transforms/gdprocrotmult.h"
#include "src/transforms/gdprocscale.h"
#include "src/transforms/gdprocsub.h"
#include "src/transforms/gdproctranslate.h"

#include "src/shapes/gdprocbox.h"
#include "src/shapes/gdproccircle.h"
#include "src/shapes/gdprocline.h"
#include "src/shapes/gdprocrect.h"

#include "src/surfaces/gdprocextrudeshape.h"
#include "src/surfaces/gdprocsimplify.h"
#include "src/surfaces/gdprocsurface.h"

#include "src/modifiers/gdprocmerge.h"
#include "src/modifiers/gdprocmirror.h"
#include "src/modifiers/gdprocplaceonpath.h"
#include "src/modifiers/gdproctransform.h"

#include "src/output/gdprocoutput.h"

#ifdef TOOLS_ENABLED

#include <editor/editor_node.h>

#include "src/editor/gdprocmesheditor.h"

void gdprocmesh_module_init_callback() {

	EditorNode *editor = EditorNode::get_singleton();
	editor->add_editor_plugin(memnew(GDProcMeshEditorPlugin(editor)));
};

#endif

void register_gdprocmesh_module_types() {

	OS::get_singleton()->print("GDProcMesh Module: Init...\n");

	// register our procedural mesh class
	ClassDB::register_class<GDProcMesh>();

	// register all our nodes
	ClassDB::register_class<GDProcNode>();

	// inputs
	ClassDB::register_class<GDProcInCurve>();
	ClassDB::register_class<GDProcInInt>();
	ClassDB::register_class<GDProcInMesh>();
	ClassDB::register_class<GDProcInPoolVectors>();
	ClassDB::register_class<GDProcInReal>();
	ClassDB::register_class<GDProcInVector>();

	// primitives
	ClassDB::register_class<GDProcCount>();
	ClassDB::register_class<GDProcEuler>();
	ClassDB::register_class<GDProcRandom>();
	ClassDB::register_class<GDProcVector>();
	ClassDB::register_class<GDProcExec>();
	ClassDB::register_class<GDProcSplitVector>();

	// transforms (work on primitives)
	ClassDB::register_class<GDProcAdd>();
	ClassDB::register_class<GDProcBevel>();
	ClassDB::register_class<GDProcDiv>();
	ClassDB::register_class<GDProcGenNormals>();
	ClassDB::register_class<GDProcMult>();
	ClassDB::register_class<GDProcRedist>();
	ClassDB::register_class<GDProcRotate>();
	ClassDB::register_class<GDProcRotMult>();
	ClassDB::register_class<GDProcScale>();
	ClassDB::register_class<GDProcSub>();
	ClassDB::register_class<GDProcTranslate>();

	// shapes
	ClassDB::register_class<GDProcBox>();
	ClassDB::register_class<GDProcCircle>();
	ClassDB::register_class<GDProcLine>();
	ClassDB::register_class<GDProcRect>();

	// surfaces
	ClassDB::register_class<GDProcExtrudeShape>();
	ClassDB::register_class<GDProcSimplify>();
	ClassDB::register_class<GDProcSurface>();

	// modifiers (work on surfaces)
	ClassDB::register_class<GDProcMerge>();
	ClassDB::register_class<GDProcMirror>();
	ClassDB::register_class<GDProcPlaceOnPath>();
	ClassDB::register_class<GDProcTransform>();

	// output
	ClassDB::register_class<GDProcOutput>();

	// register editor tools
#ifdef TOOLS_ENABLED
	EditorNode::add_init_callback(&gdprocmesh_module_init_callback);
#endif
}

void unregister_gdprocmesh_module_types() {

	OS::get_singleton()->print("GDProcMesh Module: Finish...\n");
}
