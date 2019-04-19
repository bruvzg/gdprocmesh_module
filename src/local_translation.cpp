/*************************************************************************/
/*  local_translation.cpp                                                */
/*************************************************************************/

#include <core/translation.h>

#include "src/local_translation.h"

#ifdef TOOLS_ENABLED

#include "src/editor/gdprocmesheditor.h"

#endif

String RTR_LOCAL(const String &p_text) {

	String rtr = p_text;
#ifdef TOOLS_ENABLED
	if (GDProcMeshEditorPlugin::get_singleton()) {
		rtr = GDProcMeshEditorPlugin::get_singleton()->tool_translate(p_text);
	}
	if (rtr == String() || rtr == p_text) {
		if (TranslationServer::get_singleton()) {
			rtr = TranslationServer::get_singleton()->translate(p_text);
		}
	}
#else
	if (TranslationServer::get_singleton()) {
		rtr = TranslationServer::get_singleton()->translate(p_text);
	}
#endif
	return rtr;
}
