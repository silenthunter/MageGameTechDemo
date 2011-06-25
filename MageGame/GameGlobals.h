#pragma once

#include <string>
#include <unordered_map>
#include <PolyVoxCore/Material.h>

typedef PolyVox::Material<uint16_t> VoxelMat;

typedef std::unordered_map<std::string, float> StrFloatMap;
typedef std::unordered_map<std::string, uint16_t> StrU16Map;

extern StrFloatMap WorldDataMap;
extern StrFloatMap ItemDataMap;
extern StrFloatMap CharacterDataMap;
extern StrFloatMap WorldGenerationMap;
extern StrFloatMap WaterDataMap;

extern StrU16Map MaterialIDMap;