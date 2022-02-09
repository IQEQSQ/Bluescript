﻿#include "BlueprintParse.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"
#include "K2Node_FunctionEntry.h"
#include "EdGraph/EdGraphNode.h"
#include "K2Node_MacroInstance.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Interfaces/IPluginManager.h"

using namespace std;


using namespace configor;

// const char* FStringTOChar(FString SourceValue)
// {
// 	// std::string s = std::string(TCHAR_TO_UTF8(ToCStr(SourceValue)));
// 	// const char* ThePointerYouWant = s.c_str();
// 	const char* ThePointerYouWant = TCHAR_TO_ANSI(*SourceValue);
// 	return ThePointerYouWant;
// }

const bool IsContainNoLinkedPins = false;
// #define IS_CONTAIN_NO_LINKED_PINS

FString GetNodeType(UEdGraphNode* Node)
{
	if (!Node)
	{
		return TEXT("");
	}

	UK2Node_Event* K2Node_Event = Cast<UK2Node_Event>(Node);
	FString SourceValue = TEXT("");
	if (K2Node_Event)
	{
		SourceValue = K2Node_Event->GetFunctionName().ToString();
	}
	
	UK2Node_CallFunction* K2Node_CallFunction = Cast<UK2Node_CallFunction>(Node);
	if (K2Node_CallFunction)
	{
		SourceValue = K2Node_CallFunction->GetFunctionName().ToString();
	}
	
	UK2Node_MacroInstance* K2Node_MacroInstance = Cast<UK2Node_MacroInstance>(Node);
	if (K2Node_MacroInstance)
	{
		SourceValue = K2Node_MacroInstance->GetMacroGraph()->GetName();
	}
	
	UK2Node_FunctionEntry* K2Node_FunctionEntry = Cast<UK2Node_FunctionEntry>(Node);
	if (K2Node_FunctionEntry)
	{
		SourceValue = K2Node_FunctionEntry->GetName();
	}
	
	const FText Title = Node->GetNodeTitle(ENodeTitleType::FullTitle);
	FString NodeType = Title.ToString();
	return NodeType;
}

FString GetNodeName(UEdGraphNode* Node)
{
	if (!Node)
	{
		return TEXT("");
	}
	
	FString NodeName = Node->GetName();
	return NodeName;
}

FString GetPinName(UEdGraphPin* Pin)
{
	if (!Pin)
	{
		return TEXT("");
	}

	FString PinName = Pin->PinName.ToString();
	return PinName;
}

void BlueprintParse::ParseGraphs(configor::basic_config<json_args>* arr2, TArray<UEdGraph*> EdGraphs)
{
	for (UEdGraph* EdGraph : EdGraphs)
	{
		json arr = json::array({});

		for (UEdGraphNode* Node : EdGraph->Nodes)
		{
			TArray<UEdGraphPin*> Pins = Node->Pins;
			json pinArr = json::array({});
			for (UEdGraphPin* Pin : Pins)
			{
				json jsonLinkedToArray = json::array({});
				for (UEdGraphPin* PinLInkedToNode : Pin->LinkedTo)
				{
					// FString PinNameF = GetPinName(PinLInkedToNode);
					
					// const char* PinName = FStringTOChar(Pin->PinName.ToString());
					UEdGraphNode* OwingNode = PinLInkedToNode->GetOwningNode();
					FString PinNodeNameF = GetNodeName(OwingNode);
					FString PinNodeTypeF = GetNodeType(OwingNode);
					const char* PinOwningNodeType = TCHAR_TO_ANSI(*PinNodeTypeF);
					const char* PinOwningNodeName = TCHAR_TO_ANSI(*PinNodeNameF);
					// const char* PinNode = FStringTOChar(PinNodeF);
					// std::string s = std::string();
					const char* PinName = TCHAR_TO_ANSI(*PinLInkedToNode->PinName.ToString());
					
					if(OwingNode)
					{
						jsonLinkedToArray.push_back({{"PinName", PinName}, {"PinOwningNodeName",  PinOwningNodeName}, {"PinOwningNodeType", PinOwningNodeType}});	//
					}
					
					// const char* k = jsonLinkedToArray.dump(4, ' ').c_str();
						// FString TextPath = FPaths::ProjectDir() + TEXT("Data/bluescript.json");
      //               	TextPath = FPaths::ConvertRelativePathToFull(TextPath);
      //               		FFileHelper::SaveStringToFile(FString(k), *TextPath, FFileHelper::EEncodingOptions::ForceUTF8);
				}
				// FString PinName2F = GetPinName(Pin);
				// const char* PinName2 = FStringTOChar(Pin->PinName.ToString());
				// std::string s = std::string(TCHAR_TO_UTF8(ToCStr(Pin->PinName.ToString())));
				const char* PinName2 = TCHAR_TO_ANSI(*Pin->PinName.ToString());
				if(jsonLinkedToArray.size() == 0){
// #ifdef IS_CONTAIN_NO_LINKED_PINS
					if(IsContainNoLinkedPins)
					{
						pinArr.push_back({{"Name", PinName2}});
					}

// #endif
					
				
				}else
				{
					pinArr.push_back({{"Name", PinName2}, {"LinkedTo", jsonLinkedToArray}});	//	
				}
				
			}

			FString NodeNameF = GetNodeName(Node);
			FString NodeTypeF = GetNodeType(Node);
			const char* NodeName = TCHAR_TO_ANSI(*NodeNameF);
			const char* NodeType = TCHAR_TO_ANSI(*NodeTypeF);
			if(pinArr.size() == 0){
				arr.push_back({{"Name", NodeName}, {"Type", NodeType}});
			}else
			{
				arr.push_back({{"Name", NodeName}, {"Type", NodeType},{"Pins", pinArr}});	
			}
			
		}

		const char* EdGraphName = TCHAR_TO_ANSI(*EdGraph->GetName());
		arr2->push_back({{"Name", EdGraphName},{"Nodes", arr}});
	}
}


void BlueprintParse::ParseGraphs(configor::basic_config<json_args>* arr2, TArray<FBPVariableDescription> BPVariableDescriptions)
{
	for (FBPVariableDescription BPVariableDescription : BPVariableDescriptions)
	{

		FString SourceValue = BPVariableDescription.VarName.ToString();
		const char* ThePointerYouWant = TCHAR_TO_ANSI(*SourceValue);
		
		arr2->push_back({{"Name", ThePointerYouWant}});
	}
}

void BlueprintParse::StartParse()
{
#if WITH_EDITOR
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray< FAssetData > AssetList;

	FARFilter BPFilter;
	FString ConfigString = FString();
	FString PluginPath = IPluginManager::Get().FindPlugin("Bluescript")->GetBaseDir();
	FString ConfigPath = PluginPath + TEXT("/Config/config.json");
	ConfigPath = FPaths::ConvertRelativePathToFull(ConfigPath);
	FFileHelper::LoadFileToString(ConfigString, *ConfigPath);
	const auto ConfigTChar = TCHAR_TO_ANSI(*ConfigString);
	json ConfigData = json::parse(ConfigTChar);
	auto root = ConfigData["root"];
	auto data3 = root.as_string();
	FString FilterPath = data3.c_str();
	BPFilter.PackagePaths.Add(FName(FilterPath));
	BPFilter.bRecursivePaths = true;
	BPFilter.bRecursiveClasses = true;
	BPFilter.ClassNames.Add(FName(TEXT("Blueprint")));

	AssetRegistry.GetAssets(BPFilter, AssetList);
	for (FAssetData const& Asset : AssetList)
	{
		const FName ObjectPath = Asset.ObjectPath;
		FString ObjectPathString = ObjectPath.ToString();
		// const wchar_t* BP_Path = ToCStr(ObjectPathString);
		
		const UBlueprint* Blueprint = Cast<UBlueprint>(StaticLoadObject(UObject::StaticClass(), nullptr, *ObjectPathString));
		if (Blueprint == NULL)
		{
			continue;
		}
		const TArray<UEdGraph*> EventGraphs = Blueprint->EventGraphs;
		const TArray<UEdGraph*> FunctionGraphs = Blueprint->FunctionGraphs;
		const TArray<UEdGraph*> MacroGraphs = Blueprint->MacroGraphs;
		const TArray<UEdGraph*> UbergraphPages = Blueprint->UbergraphPages;
		const TArray<UEdGraph*> DelegateSignatureGraphs = Blueprint->DelegateSignatureGraphs;
		const TArray<UEdGraph*> IntermediateGeneratedGraphs = Blueprint->IntermediateGeneratedGraphs;
		const TArray<FBPVariableDescription> NewVariables = Blueprint->NewVariables;
		json j;
		// json arr = json::array({ });
		//  = arr;
		j["EventGraphs"] = json::array({});
		j["FunctionGraphs"] = json::array({});
		j["MacroGraphs"] = json::array({});
		j["UbergraphPages"] = json::array({});
		j["DelegateSignatureGraphs"] = json::array({});
		j["IntermediateGeneratedGraphs"] = json::array({});
		j["NewVariables"] = json::array({});
		ParseGraphs(&(j["EventGraphs"]), EventGraphs);
		ParseGraphs(&(j["FunctionGraphs"]), FunctionGraphs);
		ParseGraphs(&(j["MacroGraphs"]), MacroGraphs);
		ParseGraphs(&(j["UbergraphPages"]), UbergraphPages);
		ParseGraphs(&(j["DelegateSignatureGraphs"]), DelegateSignatureGraphs);
		ParseGraphs(&(j["IntermediateGeneratedGraphs"]), IntermediateGeneratedGraphs);
		ParseGraphs(&(j["NewVariables"]), NewVariables);

		const auto k = j.dump(4, ' ');
		const auto s = k.c_str();
		const FString FileData = FString(s);
		FString TextPath = PluginPath + TEXT("/Data/") + Asset.AssetName.ToString() + TEXT(".json");
		TextPath = FPaths::ConvertRelativePathToFull(TextPath);
		FFileHelper::SaveStringToFile(FileData, *TextPath, FFileHelper::EEncodingOptions::ForceUTF8);
		
	}
#endif
}