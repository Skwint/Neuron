#ifndef IZHIKEVICH_H
#define IZHIKEVICH_H

#include "Net.h"

#include "NeuronIzhikevich.h"

class Izhikevich : public Net<NeuronIzhikevich>
{
public:
	struct Data
	{
		inline Data()
		{
			v2 = 0.04f;
			v1 = 5.0f;
			v0 = 140.0f;
			a = 0.02f;
			b = 0.2f;
			c = -65.0f;
			d = 2.0f;
		}
		float v2;
		float v1;
		float v0;
		float a;
		float b;
		float c;
		float d;
	};

public:
	Izhikevich(int width = 512, int height = 512);
	virtual ~Izhikevich();

	static std::string name();
	virtual void tick();
	std::string typeName() { return name(); }

	Data & data() { return mData; }
	const Data & data() const { return mData; }
	void setData(const Data & data) { mData = data; }

private:
	Data mData;
};

#endif
