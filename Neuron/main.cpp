#include "App.h"
#include "Neuron.h"

int main(int argc, char *argv[])
{
	App a(argc, argv);
	Neuron w;
	w.show();
	return a.exec();
}
