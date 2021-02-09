#include "ConfigItem.h"

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
	return is;
}

