#include "TypeManager.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

#include "IntType.h"
#include "FloatType.h"
#include "StringType.h"
#include "TypeNames.h"

TypeManager::TypeManager()
{
	m_types.insert(std::make_pair(g_IntTypeName, std::move(std::unique_ptr<BaseType>(new IntType))));
	m_types.insert(std::make_pair(g_FloatTypeName, std::move(std::unique_ptr<BaseType>(new FloatType))));
	m_types.insert(std::make_pair(g_StringTypeName, std::move(std::unique_ptr<BaseType>(new StringType))));
}

void TypeManager::GetTypeNames(std::vector<std::string>& names) const
{
	std::transform(m_types.begin(), m_types.end(),
				   std::back_inserter(names),
				   [](const pair_type& p) -> std::string { return p.first; });
}

const BaseType* TypeManager::GetTypeByName(const std::string& name) const
{
	map_type::const_iterator it = m_types.find(name);
	if (it == m_types.end())
	{
		throw std::runtime_error("GetTypeByName: Invalid type name");
	}
	return it->second.get();
}

const std::string& TypeManager::GetNameOfType(const BaseType* type) const
{
	map_type::const_iterator it = std::find_if(m_types.begin(), m_types.end(),
											   [type](const pair_type& p) ->
											   bool { return p.second.get() == type; });
	if (it == m_types.end())
	{
		throw std::runtime_error("GetNameOfType: Invalid type");
	}
	return it->first;
}