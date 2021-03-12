#ifndef CONFIG_ITEM_H
#define CONFIG_ITEM_H

#include <string>
#include <vector>

// A ConfigItem is a single item within the configuration data for
// a neuron. What items exist, what type they are, and what range of
// reasonable values they can take, all depends on the type of neuron
// being used.
// ConfigItem objects are mostly accessed through an instance of ConfigSet.
struct ConfigItem
{
	// The type of this config item
	enum Type
	{
		INVALID, //< This is an error condition
		FLOAT,   //< The data is a floating point
		INT,     //< The data is an integer
		ENUM     //< The data is an integer index into an array of strings
	};
	// Default constructor. The resulting item is not valid and has no value.
	ConfigItem() :
		mType(INVALID)
	{}
	// Copy constructor.
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
	// Assignment operator
	ConfigItem operator=(ConfigItem other)
	{
		swap(*this, other);
		return *this;
	}
	// Constructor for floating point values. Sets type and value.
	ConfigItem(float value) :
		mType(FLOAT),
		mFloat(value)
	{}
	// Constructor for integer values. Sets type and value.
	ConfigItem(int value) :
		mType(INT),
		mInt(value)
	{}
	// Constructor for enum values. Sets type and value.
	// enumNames is a vector of strings, which are the display
	// names of the values this item can take, and index is an
	// index into that array.
	ConfigItem(int index, std::vector<std::string> enumNames) :
		mType(ENUM),
		mInt(index),
		mEnumNames(enumNames)
	{}
	// Destructor
	virtual ~ConfigItem()
	{}
	// Standard swap implementation for operator=
	friend void swap(ConfigItem & first, ConfigItem & second)
	{
		static_assert(sizeof(mInt) == sizeof(mFloat));
		std::swap(first.mType, second.mType);
		std::swap(first.mInt, second.mInt);
		std::swap(first.mEnumNames, second.mEnumNames);
	}

	// The type of this item
	Type mType;
	union
	{
		float mFloat; //< If the type is float use this
		int mInt;     //< If the type is integer or enum use this
	};
	std::vector<std::string> mEnumNames; //< Possible enum values
};

// stream operator overload for writing human readable config files
std::ostream & operator<<(std::ostream & os, ConfigItem & item);
// stream operator overload for reading config files
std::istream & operator>>(std::istream & is, ConfigItem & item);

#endif
