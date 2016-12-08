#pragma once

#include "BaseType.h"

class IntType : public BaseType
{
public:
	bool IsValid(const std::string& str) const override;
	int Compare(const std::string& left, 
				const std::string& right) const override;

private:
	int Convert(const std::string& str) const;
};

