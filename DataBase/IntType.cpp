#include "IntType.h"

#include <cstdlib>
#include <cctype>
#include <climits>
#include <cerrno>

bool IntType::IsValid(const std::string& str) const
{
	if (str.empty() || (!isdigit(str[0]) && str[0] != '-' && str[0] != '+'))
	{
		return false;
	}
	for (std::string::size_type i = 1; i < str.size(); ++i)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	char* p = NULL;
	long int value = strtol(str.c_str(), &p, 10);
	if (errno == ERANGE)
	{
		return false;
	}
	if (value > INT_MAX || value < INT_MIN)
	{
		return false;
	}
	return *p == '\0';
}

int IntType::Compare(const std::string& left, const std::string& right) const
{
	int i_left = Convert(left);
	int i_right = Convert(right);
	int diff = i_left - i_right;
	return (diff < 0) - (diff > 0);
}

int IntType::Convert(const std::string& str) const
{
	return static_cast<int>(strtol(str.c_str(), NULL, 10));
}