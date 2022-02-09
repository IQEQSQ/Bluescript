#pragma once
#include "configor/json.hpp"

class BlueprintParse
{
public:
	static void StartParse();
private:
	static void ParseGraphs(configor::json* j, const char* name, TArray<UEdGraph*> EdGraphs);
	static void ParseGraphs(configor::json* j, const char* name, TArray<FBPVariableDescription> EdGraphs) ;
};
