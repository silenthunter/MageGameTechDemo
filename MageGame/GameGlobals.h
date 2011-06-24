#pragma once

#include <string>
#include <unordered_map>
#include <PolyVoxCore/Material.h>

typedef std::unordered_map<std::string, float> StrFloatMap;

extern StrFloatMap WorldDataMap;
extern StrFloatMap ItemDataMap;
extern StrFloatMap CharacterDataMap;
extern StrFloatMap WorldGenerationMap;
extern StrFloatMap WaterDataMap;

typedef PolyVox::Material<unsigned char> VoxelMat;