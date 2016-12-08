#pragma once

#include "BaseType.h"

class FloatType : public BaseType
{
public:
	bool IsValid(const std::string& str) const override;
	int Compare(const std::string& left, 
				const std::string& right) const override;

private:
	double Convert(const std::string& str) const;
};

