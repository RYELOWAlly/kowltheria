#ifndef META_HPP
#define META_HPP

#include <QWidget>
#include "etheriasaves.pb.h"

QT_BEGIN_NAMESPACE
namespace Ui {
	class Meta;
}
QT_END_NAMESPACE

class Meta : public QWidget
{
	Q_OBJECT
	
public:
	explicit Meta(etheriasaves::EtheriaSave* save, QWidget* parent = nullptr);
	~Meta();
public slots:
	void tick_changed(int i);
	void text_changed(const QString &q);
	void num_changed(int i);
private:
	Ui::Meta* ui;
	etheriasaves::EtheriaSave* save;
};

#endif
