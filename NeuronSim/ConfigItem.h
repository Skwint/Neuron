#ifndef CONFIG_ITEM_H
#define CONFIG_ITEM_H

#include <string>

struct ConfigItem
{
	enum Type
	{
		INVALID,
		FLOAT,
		INT
	};
	ConfigItem() :
		mType(INVALID)
	{}
	ConfigItem(float value) :
		mType(FLOAT),
		mFloat(value)
	{}
	ConfigItem(int value) :
		mType(INT),
		mInt(value)
	{}
	Type mType;
	union
	{
		float mFloat;
		int mInt;
	};
};

std::ostream & operator<<(std::ostream & os, ConfigItem & item);
std::istream & operator>>(std::istream & is, ConfigItem & item);

#endif
