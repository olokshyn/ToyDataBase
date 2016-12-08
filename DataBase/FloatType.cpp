#include "FloatType.h"

#include <cstdlib>
#include <cctype>
#include <climits>
#include <cerrno>

bool FloatType::IsValid(const std::string& str) const
{
	if (str.empty() || 
		(!isdigit(str[0]) && str[0] != '-' && str[0] != '+' && str[0] != '.'))
	{
		return false;
	}
	bool dot_found = false;
	if (str[0] == '.')
	{
		dot_found = true;
	}
	for (std::string::size_type i = 1; i < str.size(); ++i)
	{
		if (!isdigit(str[i]) && (dot_found || str[i] != '.'))
		{
			return false;
		}
		if (str[i] == '.')
		{
			dot_found = true;
		}
	}
	char* p = NULL;
	strtod(str.c_str(), &p);
	if (errno == ERANGE)
	{
		return false;
	}
	return *p == '\0';
}

int FloatType::Compare(const std::string& left, const std::string& right) const
{
	double d_left = Convert(left);
	double d_right = Convert(right);
	double diff = d_left - d_right;
	return (diff < 0) - (diff > 0);
}

double FloatType::Convert(const std::string& str) const
{
	return strtod(str.c_str(), NULL);
}