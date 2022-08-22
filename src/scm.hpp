#ifndef SCM_HPP
#define SCM_HPP
#include <libguile.h>

// defined in scm.cpp
namespace scm {
	SCM scm_etheria_log(SCM text);
	SCM etheria_set_system_message(SCM text);

	SCM world_get_biome(SCM pointer);
	SCM world_get_latitude(SCM pointer);
	SCM world_get_longitude(SCM pointer);
	
	SCM save_set_modified(SCM pointer,
			      SCM modified);
	SCM save_set_modified_content(SCM pointer,
				      SCM modified);
	SCM save_force_load(SCM pointer,
			    SCM planet);
	SCM save_force_save(SCM pointer,
			    SCM planet);
	
	SCM main_get_planet();
	
	SCM planet_get_rng(SCM pointer);
	SCM planet_get_culture(SCM pointer);
	
	SCM rng_get_height(SCM pointer,
			   SCM longitude,
			   SCM latitude);
	SCM rng_get_heat(SCM pointer,
			 SCM longitude,
			 SCM latitude);
	SCM rng_get_biome(SCM pointer,
			  SCM longitude,
			  SCM latitude);
	SCM wizard_enabled();
	SCM wizard_important(SCM type);
}

#endif
