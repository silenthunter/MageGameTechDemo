#pragma once

#include <string>
#include "GameGlobals.h"

bool ParseFile(const std::string& filepath, StrFloatMap& map);
void DataInput(StrFloatMap& map, const std::string& prefix, const std::string& data);