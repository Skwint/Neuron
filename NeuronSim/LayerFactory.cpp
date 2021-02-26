#include "LayerFactory.h"

#include "Exception.h"
#include "Life.h"
#include "LinearLif.h"
#include "Izhikevich.h"
#include "Kumar.h"

#define MAKE_ALLOCATOR(x) static LayerFactory::Allocator<x> x##Allocator; mAllocators[x::name()] = &x##Allocator;

LayerFactory::LayerFactory()
{
	MAKE_ALLOCATOR(Life);
	MAKE_ALLOCATOR(LinearLif);
	MAKE_ALLOCATOR(Izhikevich);
	MAKE_ALLOCATOR(Kumar);
}

LayerFactory::~LayerFactory()
{

}

std::vector<std::string> LayerFactory::getNames() const
{
	std::vector<std::string> names;
	for (auto alloc : mAllocators)
	{
		names.push_back(alloc.first);
	}
	return names;
}

std::shared_ptr<Layer> LayerFactory::create(const std::string & name, int width, int height)
{
	if (mAllocators.count(name))
	{
		return mAllocators[name]->create(width, height);
	}
	NEURONTHROW("Unknown network type [" << name << "]");
}

const ConfigPresets & LayerFactory::presets(const std::string & name)
{
	if (mAllocators.count(name))
	{
		return mAllocators[name]->presets();
	}
	NEURONTHROW("Unknown network type [" << name << "]");
}

