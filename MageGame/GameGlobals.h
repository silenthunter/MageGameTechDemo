#pragma once

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, float> StrFloatMap;

extern StrFloatMap WorldDataMap;
extern StrFloatMap ItemDataMap;
extern StrFloatMap CharacterDataMap;
extern StrFloatMap WorldGenerationMap;
extern StrFloatMap WaterDataMap;