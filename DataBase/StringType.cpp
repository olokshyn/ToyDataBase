#include "StringType.h"


bool StringType::IsValid(const std::string& str) const
{
	return true;
}

int StringType::Compare(const std::string& left, const std::string& right) const
{
	if (left < right)
	{
		return 1;
	}
	else if (left == right)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}