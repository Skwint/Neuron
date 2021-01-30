#ifndef LAYER_FACTORY_H
#define LAYER_FACTORY_H

#include <map>
#include <memory>
#include <string>
#include <vector>

class Layer;

class LayerFactory
{
private:
	class Alloc
	{
	public:
		virtual std::string name() = 0;
		virtual std::shared_ptr<Layer> create(int width, int height) = 0;
	};
	template<typename L>
	class Allocator : public Alloc
	{
	public:
		std::string name()
		{
			return L::name();
		}
		std::shared_ptr<Layer> create(int width, int height)
		{
			return std::make_shared<L>(width, height);
		}
	};
public:
	LayerFactory();
	~LayerFactory();

	std::vector<std::string> getNames() const;
	std::shared_ptr<Layer> create(const std::string & name, int width, int height);

private:
	std::map<std::string, Alloc *> mAllocators;
};

#endif
