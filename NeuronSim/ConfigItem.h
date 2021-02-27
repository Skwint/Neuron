#ifndef CONFIG_ITEM_H
#define CONFIG_ITEM_H

#include <string>
#include <vector>

struct ConfigItem
{
	enum Type
	{
		INVALID,
		FLOAT,
		INT,
		ENUM
	};
	ConfigItem() :
		mType(INVALID)
	{}
	ConfigItem(const ConfigItem & other) :
		mType(other.mType),
		mEnumNames(other.mEnumNames)
	{
		switch (mType)
		{
		case INT:
			mInt = other.mInt;
			break;
		case FLOAT:
			mFloat = other.mFloat;
			break;
		case ENUM:
			mInt = other.mInt;
			break;
		}
	}
	ConfigItem operator=(ConfigItem other)
	{
		swap(*this, other);
		return *this;
	}
	ConfigItem(float value) :
		mType(FLOAT),
		mFloat(value)
	{}
	ConfigItem(int value) :
		mType(INT),
		mInt(value)
	{}
	ConfigItem(int index, std::vector<std::string> enumNames) :
		mType(ENUM),
		mInt(index),
		mEnumNames(enumNames)
	{}
	virtual ~ConfigItem()
	{}
	friend void swap(ConfigItem & first, ConfigItem & second)
	{
		static_assert(sizeof(mInt) == sizeof(mFloat));
		std::swap(first.mType, second.mType);
		std::swap(first.mInt, second.mInt);
		std::swap(first.mEnumNames, second.mEnumNames);
	}

	Type mType;
	union
	{
		float mFloat;
		int mInt;
	};
	std::vector<std::string> mEnumNames;
};

std::ostream & operator<<(std::ostream & os, ConfigItem & item);
std::istream & operator>>(std::istream & is, ConfigItem & item);

#endif
