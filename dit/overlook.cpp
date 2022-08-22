#include "overlook.hpp"
#include "ui_overlook.h"
#include <fstream>
#include <iostream>

#include <QFileDialog>
#include <QMessageBox>

Overlook::Overlook(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::Overlook)
{
	save_loaded = false;
	ui->setupUi(this);
	connect(ui->a_load, SIGNAL(triggered()),
		this, SLOT(load_file()));
	connect(ui->a_save, SIGNAL(triggered()),
		this, SLOT(save_file()));
	connect(ui->a_close, SIGNAL(triggered()),
		this, SLOT(close_file()));
}

Overlook::~Overlook()
{
	delete ui;
}

void Overlook::load_file()
{
	QString fileName = QFileDialog::getOpenFileName(this,
							tr("Open Save"),
							tr("save.dat"),
							tr("Save Files (save.dat)"));
	etheriasaves::EtheriaSave newsave;
	std::fstream input(fileName.toStdString(),
			   std::ios::in |
			   std::ios::binary);
	if(!input)
	{
		QMessageBox msgBox;
		msgBox.setText("Failed loading save.");
		msgBox.exec();
	}
	else if(!newsave.ParseFromIstream(&input))
	{
		QMessageBox msgBox;
		msgBox.setText("Failed parsing save.");
		msgBox.exec();
		input.close();
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("We're chilling!");
		msgBox.exec();
		save = newsave;
		update_with_save();
		save_loaded = true;
		input.close();
	}
}

void Overlook::save_file()
{
	QString fileName = QFileDialog::getSaveFileName(this,
							tr("Write to Save"),
							tr("save.dat"),
							tr("Save Files (save.dat)"));

	std::fstream output(fileName.toStdString(),
			    std::ios::out |
			    std::ios::trunc |
			    std::ios::binary);
	if(!save.SerializeToOstream(&output))
	{
		QMessageBox msgBox;
		msgBox.setText("Save failed.");
		msgBox.exec();
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("Saved.");
		msgBox.exec();
		output.close();
	}
}

void Overlook::update_with_save()
{
	if(save_loaded)
	{
		QMessageBox msgBox;
		msgBox.setText("Close the file first.");
		msgBox.exec();
		return;
	}
	statusBar()->showMessage(tr("Ready"));
	subui_world = new World(&save,this);
	ui->mdi->addSubWindow(subui_world);
	subui_world->show();
	subui_meta = new Meta(&save,this);
	ui->mdi->addSubWindow(subui_meta);
	subui_meta->show();
	subui_worldview = new Worldview(&save,this);
	ui->mdi->addSubWindow(subui_worldview);
	subui_worldview->show();
}

void Overlook::close_file()
{
	if(!save_loaded)
	{
		return;
	}
}
