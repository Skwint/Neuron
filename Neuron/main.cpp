#include "Neuron.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Neuron w;
	w.show();
	return a.exec();
}
