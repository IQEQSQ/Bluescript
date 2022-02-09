#pragma once
#include "configor/json.hpp"

class BlueprintParse
{
public:
	static void StartParse();
private:
	static void ParseGraphs(configor::basic_config<configor::json_args>* arr2, TArray<UEdGraph*> EdGraphs);
	static void ParseGraphs(configor::basic_config<configor::json_args>* arr2, TArray<FBPVariableDescription> EdGraphs) ;
};
