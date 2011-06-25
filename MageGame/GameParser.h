#pragma once

#include <string>
#include "GameGlobals.h"

bool ParseFile(const std::string& filepath, StrFloatMap& map);
bool ParseIDFile(const std::string& filepath, StrU16Map& map);
void DataInput(StrFloatMap& map, const std::string& prefix, const std::string& data);
void DataIDInput(StrU16Map& map, const std::string& data);
void DataIDInput(StrU16Map& map, const std::string& data, const std::string& id);