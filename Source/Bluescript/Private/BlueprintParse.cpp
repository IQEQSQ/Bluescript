#include "BlueprintParse.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"
#include "K2Node_FunctionEntry.h"
#include "EdGraph/EdGraphNode.h"
#include "K2Node_MacroInstance.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Interfaces/IPluginManager.h"

using namespace std;


using namespace configor;

// const char* FStringTOChar(FString SourceValue)
// {
// 	// std::string s = std::string(TCHAR_TO_UTF8(ToCStr(SourceValue)));
// 	// const char* ThePointerYouWant = s.c_str();
// 	const char* ThePointerYouWant = TCHAR_TO_UTF8(*SourceValue);
// 	return ThePointerYouWant;
// }

// constexpr bool IsContainNoLinkedPins = false;
// #define IS_CONTAIN_NO_LINKED_PINS

FString GetNodeTitle(UEdGraphNode* Node)
{
	if (!Node)
	{
		return TEXT("");
	}
	
	const FText Title = Node->GetNodeTitle(ENodeTitleType::ListView);
	FString NodeTitle = Title.ToString().Replace(TEXT(" "), TEXT(""));
	return NodeTitle;
}

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
	
	return SourceValue;
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

FString GetNodeClass(UEdGraphNode* Node)
{
	if (!Node)
	{
		return TEXT("");
	}
	
	FString NodeClass = Node->GetClass()->GetName();
	return NodeClass;
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

basic_config<json_args>  BlueprintParse::ParseGraphs(TArray<UEdGraph*> EdGraphs)
{
	json arr2= json::array({});
	for (UEdGraph* EdGraph : EdGraphs)
	{
		json JsonGraph;
		
		json JsonNodes = json::array({});

		AddJsonPropertyString(&JsonGraph, "GraphName", *EdGraph->GetName());
		
		for (UEdGraphNode* Node : EdGraph->Nodes)
		{
			TArray<UEdGraphPin*> Pins = Node->Pins;
			json pinArr = json::array({});
			UK2Node_FunctionEntry* K2Node_FunctionEntry = Cast<UK2Node_FunctionEntry>(Node);
			for (UEdGraphPin* Pin : Pins)
			{
				json jsonLinkedToArray = json::array({});
				for (UEdGraphPin* PinLinked : Pin->LinkedTo)
				{
					// FString PinNameF = GetPinName(PinLInkedToNode);
					
					// const char* PinName = FStringTOChar(Pin->PinName.ToString());
					UEdGraphNode* OwingNode = PinLinked->GetOwningNode();
					FString PinNodeNameF = GetNodeName(OwingNode);
					FString PinNodeTitleF = GetNodeTitle(OwingNode);
					const char* PinOwningNodeTitle = TCHAR_TO_UTF8(*PinNodeTitleF);
					FString PinNodeTypeF = GetNodeType(OwingNode);
					const char* PinOwningNodeType = TCHAR_TO_UTF8(*PinNodeTypeF);
					const char* PinOwningNodeName = TCHAR_TO_UTF8(*PinNodeNameF);
					FString PinOwningNodeClassF = GetNodeClass(OwingNode);
					const char* PinOwningNodeClass = TCHAR_TO_UTF8(*PinOwningNodeClassF);
					// const char* PinNode = FStringTOChar(PinNodeF);
					// std::string s = std::string();
					FString PinLinkedNameF = GetPinName(PinLinked);
					const char* PinName = TCHAR_TO_UTF8(*PinLinkedNameF);
					
					if(OwingNode)
					{
						jsonLinkedToArray.push_back({{"LinkedPin_Name", PinName}, {"LinkedPin_OwningNodeName",  PinOwningNodeName}, {"LinkedPin_OwningNodeTitle", PinOwningNodeTitle}, {"LinkedPin_OwningNodeClass",  PinOwningNodeClass}, {"LinkedPin_OwningNodeType",  PinOwningNodeType}});	//
					}else
					{
						jsonLinkedToArray.push_back({{"LinkedPin_Name", PinName}});
					}
					
					// const char* k = jsonLinkedToArray.dump(4, ' ').c_str();
						// FString TextPath = FPaths::ProjectDir() + TEXT("Data/bluescript.json");
      //               	TextPath = FPaths::ConvertRelativePathToFull(TextPath);
      //               		FFileHelper::SaveStringToFile(FString(k), *TextPath, FFileHelper::EEncodingOptions::ForceUTF8);
				}
				FString PinName2F = GetPinName(Pin);
			
				
				// const char* PinName2 = FStringTOChar(Pin->PinName.ToString());
				// std::string s = std::string(TCHAR_TO_UTF8(ToCStr(Pin->PinName.ToString())));
				const char* PinName2 = TCHAR_TO_UTF8(*PinName2F);
				
				json dataPin;
				dataPin["PinName"] = PinName2;
				dataPin["PinType"] = GetPinTypeJson(Pin->PinType);
				switch (Pin->Direction.GetValue())
				{
					case EGPD_Input:
						dataPin["PinDirection"] = "EGPD_Input";
						break;
					case EGPD_Output:
						dataPin["PinDirection"] = "EGPD_Output";
						break;
					case EGPD_MAX:
						dataPin["PinDirection"] = "EGPD_MAX";
						break;;	
				}
				
				AddJsonPropertyString(&dataPin, "DefaultValue", Pin->DefaultValue);
				
				if(jsonLinkedToArray.size() != 0){
					dataPin["PinLinkedTo"] = jsonLinkedToArray;
					pinArr.push_back(dataPin);
					
				}else// if(K2Node_FunctionEntry && strcmp(PinName2, "then"))
				{
					pinArr.push_back(dataPin);
				}
				
			}

			FString NodeNameF = GetNodeName(Node);
			const char* NodeName = TCHAR_TO_UTF8(*NodeNameF);
			
			FString NodeTitleF = GetNodeTitle(Node);
			const char* NodeTitle = TCHAR_TO_UTF8(*NodeTitleF);
			
			FString NodeClassF = GetNodeClass(Node);
			const char* NodeClass = TCHAR_TO_UTF8(*NodeClassF);

			
			json JsonNode;
			JsonNode["NodeName"] = NodeName;
			JsonNode["NodeTitle"] = NodeTitle;
			JsonNode["NodeClass"] = NodeClass;
			AddJsonPropertyString(&JsonNode, "NodeType", GetNodeType(Node));
			if (K2Node_FunctionEntry)
			{
				auto LocalVariables = K2Node_FunctionEntry->LocalVariables;
				auto LocalVariablesJson = ParseGraphs(LocalVariables);
				AddJsonProperty(&JsonGraph, "LocalVariables", ParseGraphs(LocalVariables));
			}
			
			if(pinArr.size() != 0){
				
				JsonNode["NodePins"] = pinArr;
			}
			
			JsonNodes.push_back(JsonNode);
		}
		if(JsonNodes.size() != 0)
		{
			JsonGraph["GraphNodes"] = JsonNodes;
		}
		
		arr2.push_back(JsonGraph);

	}
	if(arr2.size()==0)
	{
		return nullptr;
	}
	return arr2;
}


basic_config<json_args>  BlueprintParse::ParseGraphs(TArray<FBPVariableDescription> BPVariableDescriptions)
{
	json arr2= json::array({});
	for (FBPVariableDescription BPVariableDescription : BPVariableDescriptions)
	{

		FString VarNameF = BPVariableDescription.VarName.ToString();
		const char* VarName = TCHAR_TO_UTF8(*VarNameF);
		json data;
		data["VarName"] = VarName;
		data["VarType"] = GetPinTypeJson(BPVariableDescription.VarType);
		AddJsonPropertyString(&data, "DefaultValue", BPVariableDescription.DefaultValue);
		data["ReplicationCondition"] = (int)BPVariableDescription.ReplicationCondition.GetValue();
		AddJsonPropertyString(&data, "RepNotifyFunc", BPVariableDescription.RepNotifyFunc.ToString());
		arr2.push_back(data);
	}
	if(arr2.size()==0)
	{
		return nullptr;
	}
	return  arr2;
}

void BlueprintParse::AddJsonProperty(json* j, const char* propertyName, basic_config<json_args> JsonData)
{
	if(JsonData == nullptr)
	{
		return;
	}
	(*j)[propertyName] = JsonData;
}

void BlueprintParse::AddJsonPropertyString(json* j, const char* propertyName, FString str)
{
	if(str.IsEmpty())
	{
		return;
	}
	const char* PinSubCategory = TCHAR_TO_UTF8(*str);
	(*j)[propertyName] = PinSubCategory;
}

basic_config<json_args> BlueprintParse::GetPinTypeJson(FEdGraphPinType PinType)
{
	const auto PinCategoryF = PinType.PinCategory.ToString();
	const char* PinCategoryType = TCHAR_TO_UTF8(*PinCategoryF);
		
	auto JsonPinType = json::object({{"PinCategory",PinCategoryType}});
	// if (strcmp(PinCategoryType,"object") == 0)
	// {
	// 	const auto PinSubCategoryObjectF = BPVariableDescription.VarType.PinSubCategoryObject->GetName();
	// 	const char* PinSubCategoryObject = TCHAR_TO_UTF8(*PinSubCategoryObjectF);
	// 	_o["PinSubCategoryObject"] = PinSubCategoryObject;
	// }
	if(PinType.PinSubCategoryObject != nullptr)
	{
		const auto PinSubCategoryObjectF = PinType.PinSubCategoryObject->GetName();
		const char* PinSubCategoryObject = TCHAR_TO_UTF8(*PinSubCategoryObjectF);
		JsonPinType["PinSubCategoryObject"] = PinSubCategoryObject;
	}
	if (!(PinType.PinSubCategory.IsNone()) && (PinType.PinSubCategory.IsValid()))
	{
		const auto PinSubCategoryF = PinType.PinSubCategory.ToString();
		const char* PinSubCategory = TCHAR_TO_UTF8(*PinSubCategoryF);
		JsonPinType["PinSubCategory"] = PinSubCategory;
	}

	if(PinType.IsArray())
	{
		JsonPinType["IsArray"] = PinType.IsArray();
	}
	if(PinType.IsContainer())
	{
		JsonPinType["IsContainer"] = PinType.IsContainer();
	}
	if(PinType.IsMap())
	{
		JsonPinType["IsMap"] = PinType.IsMap();
		auto JsonValueType = json::object({});
		if(PinType.PinValueType.bTerminalIsConst)
		{
			JsonValueType["bTerminalIsConst"] = PinType.PinValueType.bTerminalIsConst == 1;
		}
		if(PinType.PinValueType.bTerminalIsWeakPointer)
		{
			JsonValueType["bTerminalIsWeakPointer"] = PinType.PinValueType.bTerminalIsWeakPointer == 1;
		}
		if(PinType.PinValueType.bTerminalIsUObjectWrapper)
		{
			JsonValueType["bTerminalIsUObjectWrapper"] = PinType.PinValueType.bTerminalIsUObjectWrapper == 1;
		}
		if(!(PinType.PinValueType.TerminalCategory.IsNone()) && (PinType.PinValueType.TerminalCategory.IsValid()))
		{
			const auto PinSubCategoryF = PinType.PinValueType.TerminalCategory.ToString();
			const char* TerminalCategory = TCHAR_TO_UTF8(*PinSubCategoryF);
			JsonValueType["TerminalCategory"] = TerminalCategory;
		}
		if(!(PinType.PinValueType.TerminalSubCategory.IsNone()) && (PinType.PinValueType.TerminalSubCategory.IsValid()))
		{
			const auto PinSubCategoryF = PinType.PinValueType.TerminalSubCategory.ToString();
			const char* TerminalSubCategory = TCHAR_TO_UTF8(*PinSubCategoryF);
			JsonValueType["TerminalSubCategory"] = TerminalSubCategory;
		}
		if(PinType.PinValueType.TerminalSubCategoryObject != nullptr)
		{
			const auto PinSubCategoryObjectF = PinType.PinValueType.TerminalSubCategoryObject->GetName();
			const char* TerminalSubCategoryObject = TCHAR_TO_UTF8(*PinSubCategoryObjectF);
			JsonValueType["TerminalSubCategoryObject"] = TerminalSubCategoryObject;
		}
		JsonPinType["PinValueType"] = JsonValueType;
	}
	if(PinType.IsSet())
	{
		JsonPinType["IsSet"] = PinType.IsSet();
	}
	if(PinType.bIsConst)
	{
		JsonPinType["bIsConst"] = PinType.bIsConst == 1;
	}
	if(PinType.bIsReference)
	{
		JsonPinType["bIsReference"] = PinType.bIsReference == 1;
	}
	if(PinType.bIsWeakPointer)
	{
		JsonPinType["bIsWeakPointer"] = PinType.bIsWeakPointer == 1;
	}
	if(PinType.bIsUObjectWrapper)
	{
		JsonPinType["bIsUObjectWrapper"] = PinType.bIsUObjectWrapper == 1;
	}

	return JsonPinType;
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
	// FString ConfigPath = PluginPath + TEXT("/Config/config.json");
	// ConfigPath = FPaths::ConvertRelativePathToFull(ConfigPath);
	// FFileHelper::LoadFileToString(ConfigString, *ConfigPath);
	// const auto ConfigTChar = TCHAR_TO_UTF8(*ConfigString);
	// json ConfigData = json::parse(ConfigTChar);
	// auto root = ConfigData["root"];
	// auto data3 = root.as_string();
	// FString FilterPath = data3.c_str();
	///Game/Blueprints/ 这样写必定会出问题，我也是无语了！@！
	FString FilterPath = "/Game/Blueprints";//data3.c_str();
	BPFilter.PackagePaths.Add(FName(FilterPath));
	BPFilter.bRecursivePaths = true;
	BPFilter.bRecursiveClasses = true;
	BPFilter.ClassNames.Add(FName(TEXT("Blueprint")));
	
	AssetRegistry.GetAssets(BPFilter, AssetList);
	IFileManager::Get().DeleteDirectory(*(PluginPath + TEXT("/Data/")), false, true);

	for (FAssetData const& Asset : AssetList)
	{
		const FName ObjectPath = Asset.ObjectPath;
		FString ObjectPathString = ObjectPath.ToString();
		// const wchar_t* BP_Path = ToCStr(ObjectPathString);
		
		const UBlueprint* Blueprint = Cast<UBlueprint>(StaticLoadObject(UObject::StaticClass(), nullptr, *ObjectPathString));
		if (Blueprint == nullptr)
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
		// j["EventGraphs"] = json::array({});
		// j["FunctionGraphs"] = json::array({});
		// j["MacroGraphs"] = json::array({});
		// j["UbergraphPages"] = json::array({});
		// j["DelegateSignatureGraphs"] = json::array({});
		// j["IntermediateGeneratedGraphs"] = json::array({});
		// j["NewVariables"] = json::array({});
		AddJsonProperty(&j, "EventGraphs", ParseGraphs(EventGraphs));
		AddJsonProperty(&j, "FunctionGraphs", ParseGraphs(FunctionGraphs));
		AddJsonProperty(&j, "MacroGraphs", ParseGraphs(MacroGraphs));
		AddJsonProperty(&j, "UbergraphPages", ParseGraphs(UbergraphPages));
		AddJsonProperty(&j, "DelegateSignatureGraphs", ParseGraphs(DelegateSignatureGraphs));
		AddJsonProperty(&j, "IntermediateGeneratedGraphs", ParseGraphs(IntermediateGeneratedGraphs));
		AddJsonProperty(&j, "NewVariables", ParseGraphs(NewVariables));
	

		const FString ParentClassF = Blueprint->ParentClass->GetName();
		const char* ParentClass = TCHAR_TO_UTF8(*ParentClassF);
		j["ParentClass"] = ParentClass;
		json arr2= json::array({});
		for(const auto blueInter: Blueprint->ImplementedInterfaces)
		{
			FString interStr = blueInter.Interface->GetName();
			const char* interChar = TCHAR_TO_UTF8(*interStr);
			arr2.push_back(interChar);
		}
		j["ImplementedInterfaces"] = arr2;
		try
		{
			const auto k = j.dump<encoding::utf8>(4, ' ');
			const auto s = k.c_str();
			const FString FileData = FString(s);
			FString TextPath = PluginPath + TEXT("/Data/") + Asset.AssetName.ToString() + TEXT(".json");
			TextPath = FPaths::ConvertRelativePathToFull(TextPath);
			FFileHelper::SaveStringToFile(FileData, *TextPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
		}
		catch (...)
		{
			printf_s(TCHAR_TO_UTF8(*(Asset.AssetName.ToString())));
		}
		
		
	}
#endif
}
