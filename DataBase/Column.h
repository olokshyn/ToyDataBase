#pragma once

#include <string>

#include "BaseType.h"

class Column
{
public:
	Column(const std::string& name, const BaseType* type)
		: m_name(name), m_type(type) {}

	std::string GetName() const { return m_name;  }
	const BaseType* GetType() const { return m_type; }

private:
	std::string m_name;
	const BaseType* m_type;
};

