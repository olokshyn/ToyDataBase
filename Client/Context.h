#pragma once

#include <memory>

#include "DataBase.h"

struct Context
{
	std::unique_ptr<DataBase> database;
};