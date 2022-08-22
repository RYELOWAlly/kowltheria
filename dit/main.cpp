#include <cstdio>
#include <QApplication>
#include "overlook.hpp"

int main(int argc, char** argv)
{
	QApplication a(argc, argv);
	Overlook m;
	m.show();	
	return a.exec();
}
