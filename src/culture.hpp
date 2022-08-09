#ifndef CULTURE_HPP
#define CULTURE_HPP

#include "actor.hpp"
#include <string>

#define AGE_OF_MAJORITY 19

enum HistoryEventType
{
	HET_NULL_ALL,
	HET_NULL = 0,
	HET_CREATION,
	HET_DEATH,
	HET_BIRTH,
	HET_BIRTHDAY,
	HET_ARRIVAL,
	HET_CHILDMAKING
};

struct Soul
{
	int age;
	int generation;
	
};

struct HistoryFigure
{
	std::string name;
	std::string uiname;

	Soul soul;
};

struct HistoryEvent
{
	HistoryEventType type;
	bool histfig;
	int histfig_id;

	int day;
	int hour;
	int year;

	int latitude;
	int longitude;

	int histfig_parenta;
	int histfig_parentb;
};

#define MAX_HISTFIGS 65535 * 2

class Culture
{
private:
	friend class SaveFile;
	HistoryFigure histfigs[MAX_HISTFIGS];
	std::vector<HistoryEvent> historyevents;
	TCODRandom *rng;
public:
	int amount_of_histfigs;
	int amount_of_histevents;
	static void Init();
	Culture();
	void AscendHour();
	void AscendStartTime();

	int MkActorHistfig(Actor* actor);
	HistoryFigure& GetHistFig(int id);
	void NewHistoryEvent(HistoryEvent event);
	int GenRandomHistfig();
	int GenRandomHistfig(int parentA, int parentB);
	HistoryEvent GetEvent(HistoryEventType type, int figure);

	int current_year;
	int current_day;
	int current_hour;
	char* age;
};

#endif
