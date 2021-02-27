#include "TrueNorth.h"

#include "ConfigPresets.h"
#include "ConfigSet.h"

using namespace std;

static const string CFG_LEAKREVERSAL("leak_reversal");
static const string CFG_LEAKWEIGHT("leak_weight");
static const string CFG_POSITIVETHRESHOLD("positive_threshold");
static const string CFG_NEGATIVETHRESHOLD("negative_threshold");
static const string CFG_RESETVOLTAGE("reset_voltage");
static const string CFG_RESETORSATURATE("reset_or_saturate");
static const string CFG_RESETMODE("reset_mode");

static const vector<string> LEAK_REVERSAL_ENUM = { "monotonic", "oriented" };
static const vector<string> RESET_OR_SATURATE_ENUM = { "reset", "saturate" };
static const vector<string> RESET_MODE_ENUM = { "reset", "offset", "none" };

TrueNorth::TrueNorth(int width, int height) :
	Net<NeuronTrueNorth>(width, height),
	mLeakReversal(0),
	mLeakWeight(0),
	mPositiveThreshold(0),
	mNegativeThreshold(0),
	mResetVoltage(0),
	mResetOrSaturate(0),
	mResetMode(0)
{
	clear();
}

TrueNorth::~TrueNorth()
{
}

string TrueNorth::name()
{
	return "TrueNorth";
}

void TrueNorth::setConfig(const ConfigSet & config)
{
	mLeakReversal = config.items().at(CFG_LEAKREVERSAL).mInt;
	mLeakWeight = config.items().at(CFG_LEAKWEIGHT).mFloat;
	mPositiveThreshold = config.items().at(CFG_POSITIVETHRESHOLD).mFloat;
	mNegativeThreshold = config.items().at(CFG_NEGATIVETHRESHOLD).mFloat;
	mResetVoltage = config.items().at(CFG_RESETVOLTAGE).mFloat;
	mResetOrSaturate = config.items().at(CFG_RESETORSATURATE).mInt;
	mResetMode = config.items().at(CFG_RESETMODE).mInt;
}

const ConfigPresets & TrueNorth::presets()
{
	static ConfigPresets presets;
	if (presets.configs().empty())
	{
		presets.read(name());
	}
	return presets;
}

ConfigSet TrueNorth::getConfig()
{
	ConfigSet config;

	config.items()[CFG_LEAKREVERSAL] = ConfigItem(mLeakReversal, LEAK_REVERSAL_ENUM);
	config.items()[CFG_LEAKWEIGHT] = mLeakWeight;
	config.items()[CFG_POSITIVETHRESHOLD] = mPositiveThreshold;
	config.items()[CFG_NEGATIVETHRESHOLD] = mNegativeThreshold;
	config.items()[CFG_RESETVOLTAGE] = mResetVoltage;
	config.items()[CFG_RESETORSATURATE] = ConfigItem(mResetOrSaturate, RESET_OR_SATURATE_ENUM);
	config.items()[CFG_RESETMODE] = ConfigItem(mResetMode, RESET_MODE_ENUM);

	return config;
}

const ConfigPresets & TrueNorth::getPresets()
{
	return TrueNorth::presets();
}

void TrueNorth::clear()
{
	Net<NeuronTrueNorth>::clear();
	for (auto neuron = mNeurons.begin(); neuron != mNeurons.end(); ++neuron)
	{
		neuron->v = mResetVoltage;
	}
}

// sgn function courtesy of
// https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
// returns -1 for negative, +1 for positive and 0 for 0
template <typename T>
inline int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

void TrueNorth::preTick()
{
	auto * cell = &mNeurons[0];
	for (int num = mWidth * mHeight; num != 0; --num)
	{
		cell->v += cell->input;
		cell->input = 0.0f;

		int leakDir = (1 - mLeakReversal) + mLeakReversal * sgn(cell->v);
		cell->v += mLeakWeight * leakDir;

		cell->firing = cell->v >= mPositiveThreshold;
		if (cell->firing)
		{
			switch (mResetMode)
			{
			case 0:
				cell->v = mResetVoltage;
				break;
			case 1:
				cell->v -= mPositiveThreshold;
				break;
			case 2:
			default:
				break;
			}
		}
		else if (cell->v < -mNegativeThreshold)
		{
			if (mResetOrSaturate)
			{
				cell->v = -mNegativeThreshold;
			}
			else
			{
				switch (mResetMode)
				{
				case 0:
					cell->v = -mResetVoltage;
					break;
				case 1:
					cell->v += mNegativeThreshold;
					break;
				case 2:
				default:
					break;
				}
			}
		}
		++cell;
	}
}

#include "TrueNorth.h"
