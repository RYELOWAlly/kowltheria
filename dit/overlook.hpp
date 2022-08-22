#ifndef OVERLOOK_HPP
#define OVERLOOK_HPP

#include <QMainWindow>
#include "etheriasaves.pb.h"

#include "worldview.hpp"
#include "world.hpp"
#include "meta.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
	class Overlook;
}
QT_END_NAMESPACE

class Overlook : public QMainWindow
{
	Q_OBJECT

public:
	explicit Overlook(QWidget* parent = nullptr);
	~Overlook();
public slots:
	void load_file();
	void save_file();
	void close_file();
private:
	void update_with_save();
	
	Ui::Overlook *ui;

	etheriasaves::EtheriaSave save;
	bool save_loaded;

	World* subui_world;
	Meta* subui_meta;
	Worldview* subui_worldview;
};

#endif
