#include "save.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <libguile.h>
#include "scm.hpp"
#ifdef WIZARD
#include <google/protobuf/util/json_util.h>
#endif

SCM scm::save_set_modified(SCM pointer, SCM modified)
{
	ASSERT(pointer);
	SaveFile* file = (SaveFile*)scm_to_pointer(pointer);
	bool mod = scm_to_bool(modified);
	etheriasaves::EtheriaSave::Meta* meta =
		file->save.mutable_meta();
	meta->set_modified(mod);
	return scm_from_bool(mod);
}

SCM scm::save_set_modified_content(SCM pointer, SCM modified)
{
	ASSERT(pointer);
	SaveFile* file = (SaveFile*)scm_to_pointer(pointer);
	bool mod = scm_to_bool(modified);
	etheriasaves::EtheriaSave::Meta* meta =
		file->save.mutable_meta();
	meta->set_modified_content(mod);
	return scm_from_bool(mod);
}

SCM scm::save_force_load(SCM pointer, SCM planet)
{	
	ASSERT(pointer);
	ASSERT(planet);
	SaveFile* file = (SaveFile*)scm_to_pointer(pointer);
	Planet* destination = (Planet*)scm_to_pointer(planet);
	file->UpdateWorld(*destination);
	return scm_from_bool(true);
}

SCM scm::save_force_save(SCM pointer, SCM planet)
{
	ASSERT(pointer);
	ASSERT(planet);
	SaveFile* file = (SaveFile*)scm_to_pointer(pointer);
	Planet* source = (Planet*)scm_to_pointer(planet);
	file->SaveWorld(*source);
	return scm_from_bool(true);
}

SaveFile::SaveFile()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	save.set_save_name("Etheria Save");
	save.set_save_version(0x7a7a);
}

bool SaveFile::LoadFile(char* path)
{
	etheriasaves::EtheriaSave newsave;
	std::fstream input(path,
			   std::ios::in |
			   std::ios::binary);

	if(!input)
	{
		etheria_log("cannot open file %s\n", path);
	}
	else if(!newsave.ParseFromIstream(&input))
	{
		etheria_log("couldnt parse save\n");
	}
	else
	{
		save = newsave;
		etheriasaves::EtheriaSave::Meta* meta = save.mutable_meta();
		meta->set_times_saved(meta->times_saved() + 1);
		meta->set_last_load(time(NULL));
		return true;
	}
	return false;
}

void SaveFile::WriteFile(char* path)
{
	etheriasaves::EtheriaSave::Meta* meta = save.mutable_meta();
	meta->set_times_loaded(meta->times_loaded() + 1);
	meta->set_last_save(time(NULL));
	std::fstream output(path,
			    std::ios::out |
			    std::ios::trunc |
			    std::ios::binary);
	if(!save.SerializeToOstream(&output))
	{
		etheria_log("couldnt save\n");
	}
	else
	{
		etheria_log("saved to %s\n", path);
	}
#ifdef WIZARD
	std::string json;
	google::protobuf::util::JsonPrintOptions options;
	options.add_whitespace = true;
	options.preserve_proto_field_names = true;
	google::protobuf::util::MessageToJsonString(save,&json,options);
	etheria_log("json data:\n%s\n",json.c_str());
#endif
}

void SaveFile::UpdateWorld(Planet& planet)
{
	etheriasaves::EtheriaSave::Meta* meta = save.mutable_meta();
	meta->set_context_reads(meta->context_reads() + 1);
	meta->set_last_context_read(time(NULL));
	if(save.has_culture())
	{
		etheriasaves::EtheriaCulture culture = save.culture();
		planet.playerCulture->current_year = culture.year();
		planet.playerCulture->current_day  = culture.day();
		planet.playerCulture->current_hour = culture.hour();
	}
	else
	{
		etheria_log("warning: no culture info found in save\n");
	}
	if(save.has_world())
	{
		etheriasaves::EtheriaWorld world = save.world();
		int locations = world.locations_size();
		for(int i = 0; i < locations; i++)
		{
			etheriasaves::EtheriaWorld::DungeonLocation
				dungeon = world.locations(i);
			planet.locations[i].latitude = dungeon.latitude();
			planet.locations[i].longitude = dungeon.longitude();
			planet.locations[i].biome = (WorldBiome)dungeon.type();
		}
		planet.location_count = locations;
	}
	else
	{
		etheria_log("warning: no dungeon info found in save\n");
	}
	if(save.has_rng())
	{
		planet.world_seed = save.rng().world_seed();
		planet.planetRNG->seed = save.rng().world_seed();
	}
	else
	{
		etheria_log("warning: no rng info found in save\n");
	}
	if(save.has_last_system_message())
		set_system_message(save.last_system_message().c_str());
	currentMode = (GameMode)save.last_mode();
	SCM lfunc = scm_variable_ref(
		scm_c_lookup("save_load_context"));
	scm_call_1(lfunc, scm_from_pointer(this, NULL));
}

void SaveFile::SaveWorld(Planet planet)
{
	SCM lfunc = scm_variable_ref(
		scm_c_lookup("save_save_context"));
	scm_call_1(lfunc, scm_from_pointer(this, NULL));
	etheriasaves::EtheriaRNG* rng = save.mutable_rng();
	etheriasaves::EtheriaWorld* world = save.mutable_world();
	etheriasaves::EtheriaSave::Meta* meta = save.mutable_meta();
	meta->set_context_writes(meta->context_writes() + 1);
	meta->set_last_context_write(time(NULL));
	etheriasaves::EtheriaCulture* culture = save.mutable_culture();
	rng->set_world_seed(planet.world_seed);
	world->clear_locations();
	for(int i = 0; i < planet.location_count; i++)
	{
		DungeonLocation location = planet.locations[i];
		etheriasaves::EtheriaWorld::DungeonLocation* slocation
			= world->add_locations();
		slocation->set_longitude(location.longitude);
		slocation->set_latitude(location.latitude);
		slocation->set_type(location.biome);
	}
	for(int i = 0;
	    i < planet.playerCulture->amount_of_histfigs;
	    i++)
	{
		HistoryFigure histfig =
			planet.playerCulture->histfigs[i];
		etheriasaves::SavedHistoryFigure* figure =
			culture->add_figures();
		figure->set_game_name(histfig.name);
		figure->set_ui_name(histfig.uiname);
		etheriasaves::HistoryFigureSoul* soul = figure->mutable_soul();
		soul->set_age(histfig.soul.age);
		soul->set_generation(histfig.soul.generation);
	}
	culture->set_amount_of_histfigs(planet.playerCulture->amount_of_histfigs);
	for(int i = 0;
	    i < planet.playerCulture->amount_of_histevents;
	    i++)
	{
		HistoryEvent histevent =
			planet.playerCulture->historyevents.at(i);
		etheriasaves::SavedHistoryEvent* event =
			culture->add_events();
		event->set_type(
			(etheriasaves::SavedHistoryEvent::HET)histevent.type);
		event->set_histfig(histevent.histfig);
		event->set_histfig_id(histevent.histfig_id);
		event->set_day(histevent.day);
		event->set_hour(histevent.hour);
		event->set_year(histevent.year);
		event->set_latitude(histevent.latitude);
		event->set_longitude(histevent.longitude);
		event->set_histfig_parenta(histevent.histfig_parenta);
		event->set_histfig_parentb(histevent.histfig_parentb);
	}
	culture->set_amount_of_histevents(planet.playerCulture->amount_of_histevents);
	culture->set_year(planet.playerCulture->current_year);
	culture->set_day(planet.playerCulture->current_day);
	culture->set_hour(planet.playerCulture->current_hour);
	save.set_last_mode((etheriasaves::EtheriaSave::Mode)currentMode);
	save.set_last_system_message(get_system_message());
}
