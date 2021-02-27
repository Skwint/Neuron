#include "ConfigItem.h"

#include <sstream>
#include <string>

using namespace std;

std::ostream & operator<<(std::ostream & os, ConfigItem & item)
{
	switch (item.mType)
	{
	case ConfigItem::INT:
		os << "int " << item.mInt;
		break;
	case ConfigItem::FLOAT:
		os << "float " << item.mFloat;
		break;
	case ConfigItem::ENUM:
	{
		os << "enum " << item.mInt;
		bool first = true;
		for (auto & name : item.mEnumNames)
		{
			if (first)
			{
				os << " ";
				first = false;
			}
			else
			{
				os << ",";
			}
			os << name;
		}
		break;
	}
	default:
		break;
	}
	return os;
}

std::istream & operator>>(std::istream & is, ConfigItem & item)
{
	std::string typeStr;
	is >> typeStr;
	if (typeStr == "float")
	{
		item.mType = ConfigItem::FLOAT;
		is >> item.mFloat;
	}
	else if (typeStr == "int")
	{
		item.mType = ConfigItem::INT;
		is >> item.mInt;
	}
	else if (typeStr == "enum")
	{
		item.mType = ConfigItem::ENUM;
		is >> item.mInt;
		string data;
		is >> data;
		stringstream str(data);
		string value;
		do
		{
			value.clear();
			getline(str, value, ',');
			if (!value.empty())
				item.mEnumNames.push_back(value);
		} while (!value.empty());
	}
	return is;
}

