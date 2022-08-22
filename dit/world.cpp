#include "world.hpp"
#include "ui_world.h"

World::World(etheriasaves::EtheriaSave* save,
	     QWidget* parent) :
	QWidget(parent),
	ui(new Ui::World)
{
	ui->setupUi(this);

	connect(ui->s_hour, SIGNAL(valueChanged(int)),
		this, SLOT(value_changed(int)));
	connect(ui->s_year, SIGNAL(valueChanged(int)),
		this, SLOT(value_changed(int)));
	connect(ui->s_day, SIGNAL(valueChanged(int)),
		this, SLOT(value_changed(int)));
	connect(ui->s_seed, SIGNAL(valueChanged(int)),
		this, SLOT(value_changed(int)));
				   	
	this->save = save;

	etheriasaves::EtheriaCulture culture = save->culture();
	etheriasaves::EtheriaRNG rng = save->rng();
	ui->s_hour->setValue(culture.hour());
	ui->s_year->setValue(culture.year());
	ui->s_day->setValue(culture.day());
	ui->s_seed->setValue(rng.world_seed());
}

void World::value_changed(int i)
{
	etheriasaves::EtheriaCulture culture = save->culture();
	etheriasaves::EtheriaRNG rng = save->rng();
	culture.set_year(ui->s_year->value());
	culture.set_hour(ui->s_hour->value());
	culture.set_day(ui->s_day->value());
	rng.set_world_seed(ui->s_seed->value());
	ui->p_end->setValue(culture.year());
}

World::~World()
{
	delete ui;	
}

		
