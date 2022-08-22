#include "lisp.hpp"
#include <libguile.h>

#include "scm.hpp"

// libguile binding functions
static void* __elisp_register_fun(void* data)
{
	return ELisp::RegisterFun();
}

void ELisp::Initialize()
{
	scm_with_guile(&__elisp_register_fun, NULL);

	scm_c_primitive_load(
		std::filesystem::absolute(system_data_dir() /
					  "env" / "main.scm").c_str());
}

#define SCM_REGISTER(name,args)				\
	scm_c_define_gsubr(#name,			\
			   args, 0, 0,			\
			   (void*)&scm::name);		\
	etheria_log("registered scm %s\n",#name);	\

void* ELisp::RegisterFun()
{
	scm_c_define_gsubr("etheria_log",
			   1, 0, 0,
			   (void*)&scm::scm_etheria_log);
	SCM_REGISTER(etheria_set_system_message, 1);
	//SCM_REGISTER(world_get_biome, 1);
	//SCM_REGISTER(world_get_latitude, 1);
	//SCM_REGISTER(world_get_longitude, 1);
	SCM_REGISTER(save_set_modified, 2);
	SCM_REGISTER(save_set_modified_content, 2);
	SCM_REGISTER(save_force_load, 2);
	SCM_REGISTER(save_force_save, 2);
	SCM_REGISTER(main_get_planet, 0);
	SCM_REGISTER(planet_get_rng, 1);
	SCM_REGISTER(planet_get_culture, 1);
	SCM_REGISTER(rng_get_height, 3);
	SCM_REGISTER(rng_get_heat, 3);
	SCM_REGISTER(rng_get_biome, 3);
	SCM_REGISTER(wizard_enabled, 0);
	SCM_REGISTER(wizard_important, 1);
	return nullptr;
}
