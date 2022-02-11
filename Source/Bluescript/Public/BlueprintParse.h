#pragma once
#include "configor/json.hpp"
using namespace configor;
class BlueprintParse
{
public:
	static void StartParse();
private:
	static basic_config<json_args> ParseGraphs(TArray<UEdGraph*> EdGraphs);
	static basic_config<json_args> ParseGraphs(TArray<FBPVariableDescription> EdGraphs) ;
	static basic_config<json_args> GetPinTypeJson(FEdGraphPinType PinType);
	static void AddJsonProperty(json* j, const char* propertyName, basic_config<json_args> JsonData);
	static void AddJsonPropertyString(json* j, const char* propertyName, FString str);
};
