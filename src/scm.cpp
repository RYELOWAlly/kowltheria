// LIBGUILE *generic* SCM FUNCTIONS
// Abandon all hope!

// SCM functions related to a specific system of Etheria should be
// defined within their own source files, ex: a RNG function should be
// defined within 'src/rng.cpp' rather then here (although, rng.cpp in
// src shouldnt exist anymore as rng should be in tier0)

#include "scm.hpp"
#include <cbase.hpp>
#include <libguile.h>
#include <rng.hpp>
#include "wizard.hpp"

SCM scm::scm_etheria_log(SCM text)
{
	char* ctext = scm_to_locale_string(text);
	etheria_log("elisp: %s\n", ctext);
	return scm_from_bool(suppress_output);
}

SCM scm::etheria_set_system_message(SCM text)
{
	char* ctext = scm_to_locale_string(text);
	etheria_log("elisp setting message to %s\n", ctext);
	set_system_message(ctext);
	return scm_from_bool(true);
}

SCM scm::rng_get_height(SCM pointer,
			SCM slongitude,
			SCM slatitude)
{
	ASSERT(pointer);
	int longitude = scm_to_int(slongitude);
	int latitude  = scm_to_int(slatitude);
	RNG* rng = (RNG*)scm_to_pointer(pointer);
	return scm_from_double(rng->GetHeightAt(latitude,
						longitude));
}

SCM scm::rng_get_heat(SCM pointer,
		      SCM slongitude,
		      SCM slatitude)
{
	ASSERT(pointer);
	int longitude = scm_to_int(slongitude);
	int latitude  = scm_to_int(slatitude);
	RNG* rng = (RNG*)scm_to_pointer(pointer);
	return scm_from_double(rng->GetHeatAt(latitude,
					      longitude));
}

SCM scm::rng_get_biome(SCM pointer,
		       SCM slongitude,
		       SCM slatitude)
{
	ASSERT(pointer);
	int longitude = scm_to_int(slongitude);
	int latitude  = scm_to_int(slatitude);
	RNG* rng = (RNG*)scm_to_pointer(pointer);
	return scm_from_int(rng->GetWorldBiomeAt(latitude,
						 longitude));
}
