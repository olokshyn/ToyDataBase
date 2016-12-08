#pragma once

#include <string>

class BaseType
{
public:
	BaseType() {}
	virtual ~BaseType() {}

	virtual bool IsValid(const std::string& str) const = 0;
	virtual int Compare(const std::string& left, 
						const std::string& right) const = 0;

private:
	BaseType(const BaseType& other);
	BaseType& operator=(const BaseType& right);
};