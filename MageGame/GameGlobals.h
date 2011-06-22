#pragma once

#include <string>
#include <unordered_map>
#include "PolyVoxCore/Material.h"

typedef std::unordered_map<std::string, float> StrFloatMap;

typedef PolyVox::Material<unsigned int> VoxelMat;

extern StrFloatMap WorldDataMap;
extern StrFloatMap ItemDataMap;
extern StrFloatMap CharacterDataMap;
extern StrFloatMap WorldGenerationMap;
extern StrFloatMap WaterDataMap;