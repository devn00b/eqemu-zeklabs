#include "KBStructs.h"

class KBFunctions
{
public:
	static bool GenerateRandomLootGroupResult(RandomLootDropResult_Struct* rldr, uint32 groupid);
	static uint32 CalculateBaseFormula(float baseValue,float dbValue, uint32 min, uint32 max);
	static sint32 CalculateBaseFormula(float baseValue,float dbValue, sint32 min, sint32 max);
	static sint16 CalculateBaseFormula(float baseValue,float dbValue, sint16 min, sint16 max);
	static uint8 CalculateBaseFormula(float baseValue,float dbValue, uint8 min, uint8 max);
	static float CalculateBaseFormula(float baseValue,float dbValue, float min, float max);
	static bool GenerateRandItemStruct(float baseValue, float dbValue, Item_Struct* itmStruct, const RandomItemTemplate_Struct* rndItem);
};