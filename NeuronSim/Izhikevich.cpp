#include "Izhikevich.h"

Izhikevich::Izhikevich(int width, int height) :
	Net<NeuronIzhikevich>(width, height)
{

}

Izhikevich::~Izhikevich()
{
}

std::string Izhikevich::name()
{
	return "Izhikevich";
}

void Izhikevich::tick()
{
}
