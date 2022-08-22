#include "wizard.hpp"
#include <libguile.h>
#include "scm.hpp"

bool wizard_importantevent(HistoryEventType type)
{
#ifdef WIZARD	
	switch(type)
	{
	case HET_CHILDMAKING:
		return true;
	case HET_BIRTH:
		return true;
	default:
		return false;
	}
#endif
	return false;
}

SCM scm::wizard_important(SCM stype)
{
	HistoryEventType type = (HistoryEventType)scm_to_int(stype);
	bool ret = wizard_importantevent(type);
	return scm_from_bool(ret);	
}

SCM scm::wizard_enabled()
{
	#ifdef WIZARD
	return scm_from_bool(true);
	#else
	return scm_from_bool(false);
	#endif
}
