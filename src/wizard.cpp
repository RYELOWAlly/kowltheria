#include "wizard.hpp"

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
