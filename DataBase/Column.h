#pragma once

#include <string>

#include "BaseType.h"

class Column
{
	friend bool operator==(const Column& left, const Column& right);
	friend bool operator!=(const Column& left, const Column& right);

public:
	Column(const std::string& name, const BaseType* type)
		: m_name(name), m_type(type) {}

	std::string GetName() const { return m_name;  }
	const BaseType* GetType() const { return m_type; }

private:
	std::string m_name;
	const BaseType* m_type;
};

inline bool operator==(const Column& left, const Column& right)
{
	return left.m_name == right.m_name && left.m_type == right.m_type;
}

inline bool operator!=(const Column& left, const Column& right)
{
	return !operator==(left, right);
}