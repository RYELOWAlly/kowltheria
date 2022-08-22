#ifndef WORLD_HPP
#define WORLD_HPP

#include <QWidget>
#include "etheriasaves.pb.h"

QT_BEGIN_NAMESPACE
namespace Ui {
	class World;
}
QT_END_NAMESPACE

class World : public QWidget
{
	Q_OBJECT
public:
	explicit World(etheriasaves::EtheriaSave* save, QWidget* parent = nullptr);
	~World();
public slots:
	void value_changed(int i);
private:
	Ui::World *ui;
	etheriasaves::EtheriaSave* save;
};

#endif
