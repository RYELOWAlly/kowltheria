#ifndef WORLDVIEW_HPP
#define WORLDVIEW_HPP
#include <QWidget>
#include "etheriasaves.pb.h"
#include <rng.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
	class Worldview;
}
QT_END_NAMESPACE

class Worldview : public QWidget
{
	Q_OBJECT
public:
	explicit Worldview(etheriasaves::EtheriaSave* save, QWidget*
			   parent = nullptr);
	~Worldview();
public slots:
	void refresh();
private:
	Ui::Worldview *ui;
	etheriasaves::EtheriaSave* save;
	RNG rng;
};
#endif
