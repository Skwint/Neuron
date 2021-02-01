#include "Life.h"

#include "Log.h"

Life::Life(int width, int height) :
	Net(width, height),
	mLow(2.25),
	mHigh(3.75)
{
	mConfig = Life::defaultConfig();
}

Life::~Life()
{

}

std::string Life::name()
{
	return "Life";
}

void Life::setConfig(const ConfigSet & config)
{
	mConfig = config;
	for (auto & item : config)
	{
		if (item.name == "low threshold")
		{
			mLow = item.value;
		}
		else if (item.name == "high threshold")
		{
			mHigh = item.value;
		}
		else
		{
			LOG("Unexpected config item [" << item.name << "]");
		}
	}
}

const ConfigSet & Life::defaultConfig()
{
	static ConfigSet config;
	if (config.empty())
	{
		ConfigItem item;
		item.name = "low threshold";
		item.minimum = -1.0f;
		item.maximum = 100.0f;
		item.value = item.def = 2.25;
		config.push_back(item);
		item.name = "high threshold";
		item.value = item.def = 3.75;
		config.push_back(item);
	}
	return config;
}

void Life::tick()
{
	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			cell->input = 0.0f;
			++cell;
		}
	}

	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * src = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			for (int tr = -1; tr < 2; ++tr)
			{
				NeuronLife * dst = src + tr * rowStep() - 1;
				for (int tc = -1; tc < 2; ++tc)
				{
					if (tc || tr)
						dst->input += src->output;
					else
						dst->input += 0.5f * src->output;
					++dst;
				}
			}
			++src;
		}
	}

	for (int rr = 0; rr < mHeight; ++rr)
	{
		NeuronLife * cell = row(rr);
		for (int cc = 0; cc < mWidth; ++cc)
		{
			if (cell->input > mLow && cell->input < mHigh)
			{
				cell->output = 1.0f;
			}
			else
			{
				cell->output = 0.0f;
			}
			++cell;
		}
	}
}
