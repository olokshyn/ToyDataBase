#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "BaseType.h"

class TypeManager
{
public:
	TypeManager();

	void GetTypeNames(std::vector<std::string>& names) const;
	const BaseType* GetTypeByName(const std::string& name) const;
	const std::string& GetNameOfType(const BaseType* type) const;

private:
	typedef std::pair< const std::string, std::unique_ptr<BaseType> > pair_type;
	typedef std::map< std::string, std::unique_ptr<BaseType> > map_type;

private:
	TypeManager(const TypeManager& other);
	TypeManager& operator=(const TypeManager& right);

private:
	map_type m_types;
};

