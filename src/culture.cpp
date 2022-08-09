#include "cbase.hpp"
#include "culture.hpp"
#include "wizard.hpp"
#include <libtcod.hpp>

void Culture::Init()
{
	TCODNamegen::parse((system_data_dir() / "culture_names.txt").c_str(),
			   TCODRandom::getInstance());
}

Culture::Culture()
{
	rng = TCODRandom::getInstance();
	current_year = rng->getInt(0,1000);
	current_day = 0;
	age = "Development";
}

void Culture::AscendStartTime()
{
	// add a sort of randomness to it
	for(int l = 0; l < 4; l++)
	{
		for(int j = 0; j < 31; j++)
		{
			for(int i = 0; i < 12; i++)
			{
				AscendHour();
			}
		}
	}
}

HistoryEvent Culture::GetEvent(HistoryEventType type, int figure)
{
	for(int i = 0; i < amount_of_histevents; i++)
	{
		HistoryEvent event = historyevents.at(i);
		if((event.type == type || type == HET_NULL_ALL)
		   && event.histfig_id == figure)
			return event;
	}
	return HistoryEvent();
}

void Culture::AscendHour()
{
	for(int i = 0; i < amount_of_histfigs; i++)
	{
		HistoryFigure* currentfigure =
			&histfigs[i];
		// if(strcmp(currentfigure->name.c_str(),"player")==0)
		//	continue; // dont do history for player

		if(current_hour == 23)
		{
			HistoryEvent bevent = GetEvent(HET_BIRTH, i);

			if(bevent.histfig == true)
			{
				if(bevent.day == current_day)
				{
					currentfigure->soul.age++;
					HistoryEvent birthdayevent = HistoryEvent();
					birthdayevent.histfig = true;
					birthdayevent.histfig_id = i;
					birthdayevent.day = current_day;
					birthdayevent.hour = current_hour;
					birthdayevent.year = current_year;
					birthdayevent.type =
						HET_BIRTHDAY;
					NewHistoryEvent(birthdayevent);
				}
			}

			if(currentfigure->soul.age >= AGE_OF_MAJORITY)
			{
				// adult only stuff
				int makekidchance =
					rng->getInt(1,
						    amount_of_histfigs * 2);
				
				if(makekidchance == 1)
				{

					int kidstomake =
						rng->getInt(1,8);
					// find a suitable mate
					int parentid = -1;
					for(int i = 0;
					    i < amount_of_histfigs;
					    i++)
					{
						int okchance =
							rng->getInt(1,10);
						if(okchance == 1 &&
							histfigs[i].soul.age
						   >= AGE_OF_MAJORITY)
						{
							parentid = i;
							break;
						}
					}
					for(int i = 0; i <
						    kidstomake;
					    i++)
					{
						GenRandomHistfig(i,
								 parentid);
					}
				}
			}
		}
	}
	
	current_hour++;
	if(current_hour >= 24)
	{
		current_hour = 0;
		current_day++;
	}
	if(current_day >= 365)
	{
		current_day = 0;
		current_year++;
	}
}

HistoryFigure& Culture::GetHistFig(int id)
{
	return histfigs[id];
}

void Culture::NewHistoryEvent(HistoryEvent event)
{
	if(wizard_importantevent(event.type))
	{
		etheria_log("new history event, hi: %b hid: %i"
			    ", type: %i, %iEY/%i %ih\n",
			    event.histfig,
			    event.histfig_id,
			    event.type,
			    event.year,
			    event.day,
			    event.hour);
		if(event.histfig)
		{
			HistoryFigure fig = GetHistFig(event.histfig_id);
			etheria_log("fig name: %s %s\n", fig.uiname.c_str(), fig.name.c_str());
		}
	}
	event.day = current_day;
	event.hour = current_hour;
	event.year = current_year;
	historyevents.push_back(event);
	amount_of_histevents++;
}

int Culture::MkActorHistfig(Actor* actor)
{
	etheria_log("making actor %p into a histfig %i\n",
		    actor,
		    amount_of_histfigs);
	HistoryFigure* newfigure = &histfigs[amount_of_histfigs++];
	newfigure->name = std::string(actor->name);
	newfigure->uiname = std::string(actor->uiname);
	HistoryEvent figureaddedevent = HistoryEvent();
	figureaddedevent.histfig = true;
	figureaddedevent.histfig_id = amount_of_histfigs - 1;
	figureaddedevent.type = HET_CREATION;
	figureaddedevent.day = current_day;
	figureaddedevent.year = current_year;
	figureaddedevent.hour = current_hour;
	actor->histfig_id = amount_of_histfigs - 1;
	NewHistoryEvent(figureaddedevent);
}

// they're made as a baby
int Culture::GenRandomHistfig()
{
	
	HistoryFigure* newfigure = &histfigs[amount_of_histfigs++];
	newfigure->name = "histfig";
	//newfigure->uiname = TCODNamegen::generate("Celtic male");
	newfigure->uiname = "Friend";
	etheria_log("generating random histfig %s\n",
		    newfigure->uiname.c_str());
	
	HistoryEvent figureaddedevent = HistoryEvent();
	figureaddedevent.histfig = true;
	figureaddedevent.histfig_id = amount_of_histfigs - 1;
	figureaddedevent.type = HET_CREATION;
	figureaddedevent.day = current_day;
	figureaddedevent.year = current_year;
	figureaddedevent.hour = current_hour;
	HistoryEvent figurebirthevent = HistoryEvent();
	figurebirthevent.histfig = true;
	figurebirthevent.histfig_id = amount_of_histfigs - 1;
	figurebirthevent.type = HET_BIRTH;
	figurebirthevent.day = current_day;
	figurebirthevent.year = current_year;
	figurebirthevent.hour = current_hour;
	figurebirthevent.histfig_parenta = -1;
	figurebirthevent.histfig_parentb = -1;

	NewHistoryEvent(figureaddedevent);
	NewHistoryEvent(figurebirthevent);
}

int Culture::GenRandomHistfig(int parentA, int parentB)
{
	
	HistoryFigure* newfigure = &histfigs[amount_of_histfigs++];
	newfigure->name = "histfig";
	//newfigure->uiname = TCODNamegen::generate("Celtic male");
	newfigure->uiname = "Friend";
	
	HistoryEvent figureaddedevent = HistoryEvent();
	figureaddedevent.histfig = true;
	figureaddedevent.histfig_id = amount_of_histfigs - 1;
	figureaddedevent.type = HET_CREATION;
	figureaddedevent.day = current_day;
	figureaddedevent.year = current_year;
	figureaddedevent.hour = current_hour;
	HistoryEvent figurebirthevent = HistoryEvent();
	figurebirthevent.histfig = true;
	figurebirthevent.histfig_id = amount_of_histfigs - 1;
	figurebirthevent.type = HET_BIRTH;
	figurebirthevent.day = current_day;
	figurebirthevent.year = current_year;
	figurebirthevent.hour = current_hour;
	figurebirthevent.histfig_parenta = parentA;
	figurebirthevent.histfig_parentb = parentB;

	HistoryFigure parentAFig = histfigs[parentA];

	newfigure->soul.generation = parentAFig.soul.generation + 1;
	etheria_log("generating random histfig %s, gen %i\n",
		    newfigure->uiname.c_str(),newfigure->soul.generation);

	NewHistoryEvent(figureaddedevent);
	NewHistoryEvent(figurebirthevent);
}
