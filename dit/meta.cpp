#include "meta.hpp"
#include "ui_meta.h"

Meta::Meta(etheriasaves::EtheriaSave* save,
	   QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Meta)
{
	ui->setupUi(this);

	etheriasaves::EtheriaSave::Meta meta = save->meta();
	ui->i_lmessage->setText(
		QString::fromStdString(save->last_system_message()));
	ui->n_creads->display(meta.context_reads());
	ui->n_cwrites->display(meta.context_writes());
	ui->n_cloads->display(meta.times_loaded());
	ui->n_csaves->display(meta.times_saved());
	ui->b_modify->setCheckState(meta.modified() ?
				    Qt::Checked : Qt::Unchecked);
	ui->b_modify_2->setCheckState(meta.modified_content() ?
				    Qt::Checked : Qt::Unchecked);
	ui->m_mode->setValue((int)save->last_mode());
	ui->n_version->display((int)save->save_version());
	ui->i_savename->setText(
		QString::fromStdString(save->save_name()));

	connect(ui->m_mode, SIGNAL(valueChanged(int)),
		this, SLOT(num_changed(int)));
	connect(ui->i_lmessage, SIGNAL(textChanged(const QString&)),
		this, SLOT(text_changed(const QString&)));
	connect(ui->i_savename, SIGNAL(textChanged(const QString&)),
		this, SLOT(text_changed(const QString&)));
	connect(ui->b_modify, SIGNAL(stateChanged(int)),
		this, SLOT(tick_changed(int)));
	connect(ui->b_modify_2, SIGNAL(stateChanged(int)),
		this, SLOT(tick_changed(int)));

	this->save = save;
}

void Meta::tick_changed(int i)
{
	etheriasaves::EtheriaSave::Meta* meta = save->mutable_meta();
	meta->set_modified(ui->b_modify->isChecked());
	meta->set_modified_content(ui->b_modify_2->isChecked());
}

void Meta::text_changed(const QString &q)
{
	save->set_save_name(ui->i_savename->text().toStdString());
	save->set_last_system_message(ui->i_lmessage->text().toStdString());
}

void Meta::num_changed(int i)
{
	save->set_last_mode((etheriasaves::EtheriaSave::Mode)ui->m_mode->value());
}

Meta::~Meta()
{
	delete ui;
}
