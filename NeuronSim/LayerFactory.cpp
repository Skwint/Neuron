#include "LayerFactory.h"

#include "Life.h"
#include "Izhikevich.h"

#define MAKE_ALLOCATOR(x) static LayerFactory::Allocator<x> x##Allocator; mAllocators[x::name()] = &x##Allocator;

LayerFactory::LayerFactory()
{
	MAKE_ALLOCATOR(Life);
	MAKE_ALLOCATOR(Izhikevich);
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
	return 0;
}
