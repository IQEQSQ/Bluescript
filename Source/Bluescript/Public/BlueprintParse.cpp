#include "BlueprintParse.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"

#include "K2Node_MacroInstance.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Interfaces/IPluginManager.h"

using namespace std;


using namespace configor;

const char* FStringTOChar(FString SourceValue)
{
	// std::string s = std::string(TCHAR_TO_UTF8(ToCStr(SourceValue)));
	// const char* ThePointerYouWant = s.c_str();
	const char* ThePointerYouWant = TCHAR_TO_ANSI(*SourceValue);
	return ThePointerYouWant;
}


FString GetNameFromNode(UEdGraphNode* Node)
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

	return SourceValue;
}

FString GetNameFromPin(UEdGraphPin* Pin)
{
	if (!Pin)
	{
		return TEXT("");
	}

	FString SourceValue = Pin->PinName.ToString();
	return SourceValue;
}

void BlueprintParse::ParseGraphs(configor::basic_config<json_args>* arr2, TArray<UEdGraph*> EdGraphs)
{
	for (UEdGraph* EventGraph : EdGraphs)
	{
		json arr = json::array({});

		for (UEdGraphNode* Node : EventGraph->Nodes)
		{
			TArray<UEdGraphPin*> Pins = Node->Pins;
			json pinArr = json::array({});
			for (UEdGraphPin* Pin : Pins)
			{
				json jsonLinkedToArray = json::array({});
				for (UEdGraphPin* PinLInkedToNode : Pin->LinkedTo)
				{
					// FString PinNameF = GetNameFromPin(PinLInkedToNode);
					
					// const char* PinName = FStringTOChar(Pin->PinName.ToString());
					FString PinNodeF = GetNameFromNode(PinLInkedToNode->GetOwningNode());
					const char* PinNode = TCHAR_TO_ANSI(*PinNodeF);
					// const char* PinNode = FStringTOChar(PinNodeF);
					// std::string s = std::string();
					const char* PinName = TCHAR_TO_ANSI(*PinLInkedToNode->PinName.ToString());
					if(!PinNodeF.IsEmpty())
					{
						jsonLinkedToArray.push_back({{"PinName", PinName}, {"PinNode",  PinNode}});	//
					}
					
					// const char* k = jsonLinkedToArray.dump(4, ' ').c_str();
						// FString TextPath = FPaths::ProjectDir() + TEXT("Data/bluescript.json");
      //               	TextPath = FPaths::ConvertRelativePathToFull(TextPath);
      //               		FFileHelper::SaveStringToFile(FString(k), *TextPath, FFileHelper::EEncodingOptions::ForceUTF8);
				}
				// FString PinName2F = GetNameFromPin(Pin);
				// const char* PinName2 = FStringTOChar(Pin->PinName.ToString());
				// std::string s = std::string(TCHAR_TO_UTF8(ToCStr(Pin->PinName.ToString())));
				const char* PinName2 = TCHAR_TO_ANSI(*Pin->PinName.ToString());
				if(jsonLinkedToArray.size() == 0){
					pinArr.push_back({{"Name", PinName2}});
				}else
				{
					pinArr.push_back({{"Name", PinName2}, {"LinkedTo", jsonLinkedToArray}});	//	
				}
				
			}

			FString NodeNameF = GetNameFromNode(Node);
			const char* NodeName = TCHAR_TO_ANSI(*NodeNameF);//(NodeNameF);
			if(pinArr.size() == 0){
				arr.push_back({{"Name", NodeName}});
			}else
			{
				arr.push_back({{"Name", NodeName}, {"Pins", pinArr}});	
			}
			
		}

		arr2->push_back({{"Nodes", arr}});
	}
}


void BlueprintParse::ParseGraphs(configor::basic_config<json_args>* arr2, TArray<FBPVariableDescription> EdGraphs)
{
	for (FBPVariableDescription EventGraph : EdGraphs)
	{
		json arr = json::array({});

		FString SourceValue = EventGraph.VarName.ToString();
		const char* ThePointerYouWant = TCHAR_TO_ANSI(*SourceValue);
		arr.push_back({{"Name", ThePointerYouWant}});

		arr2->push_back({{"Nodes", arr}});
	}
}

void BlueprintParse::StartParse()
{
#if WITH_EDITOR
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray< FAssetData > AssetList;

	FARFilter BPFilter;
	BPFilter.PackagePaths.Add(FName(TEXT("/Game")));
	BPFilter.bRecursivePaths = true;
	BPFilter.bRecursiveClasses = true;
	BPFilter.ClassNames.Add(FName(TEXT("Blueprint")));

	AssetRegistry.GetAssets(BPFilter, AssetList);
	for (FAssetData const& Asset : AssetList)
	{
		const TCHAR* BP_Path = ToCStr(Asset.ObjectPath.ToString());
		const UBlueprint* Blueprint = Cast<UBlueprint>(StaticLoadObject(UObject::StaticClass(), nullptr, BP_Path));
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

		const char* k = j.dump(4, ' ').c_str();
	
		FString TextPath = IPluginManager::Get().FindPlugin("Puerts")->GetBaseDir() + TEXT("Data/") + Asset.AssetClass.ToString() + TEXT(".json");
		TextPath = FPaths::ConvertRelativePathToFull(TextPath);
		FFileHelper::SaveStringToFile(FString(k), *TextPath, FFileHelper::EEncodingOptions::ForceUTF8);
	
	}
#endif
	return;

	// TArray<FString> TextLines;
	// TextLines.Add(TEXT("钢铁为身，而火焰为血。"));
	// TextLines.Add(TEXT("手制之剑已达千余，纵横无数战场而不败。"));
	// TextLines.Add(TEXT("不知生。"));
	// TextLines.Add(TEXT("亦不知死。"));
	// TextLines.Add(TEXT("未曾一次败退。"));
	// TextLines.Add(TEXT("常独自一人立于剑丘之巅独醉于胜利中。"));
	// TextLines.Add(TEXT("然而 留下的只有虚无。"));
	// TextLines.Add(TEXT("故此 此生已无意义。"));
	// TextLines.Add(TEXT("则此躯 注定为剑而生。"));
	// TextLines.Add(TEXT("故我祈求——无限之剑制(Unlimited Blade Works)"));	
	// FFileHelper::SaveStringArrayToFile(TextLines, *TextPath,FFileHelper::EEncodingOptions::ForceUTF8);




	// j.push_back("aa");
	// j.push_back({"k","aa"});
	// json obj = {
	// 				{
	// 					"user", {
	// 		            { "id", 10 },
	// 					{ "name", "Nomango" }
	// 					}
	// 				}
	// 			};
	// j.push_back({"k",obj});
	// auto k = j.dump();
	// DefaultPawnClass = PlayerPawnBPClass.Class;
	// std::stringstream s;
	//
	// // 把 obj 序列化，并输入到 s 流中
	// s << json::wrap(GeneratedBP);

	// json j;
	// 		json arr = json::array({ });
	// 		arr.();    // 3
	// arr.empty();   // false
	// arr.erase(0);  // 第一个元素被删除
	// arr.clear();
	// 		// j["number"] = 1;
	// 		// j["float"] = 1.5;
	// 		// j["string"] = "this is a string";
	// 		// j["boolean"] = true;
	// 		// j["user"]["id"] = 10;
	// 		// j["user"]["name"] = "Nomango";
	// 		// 使用初始化列表构造数组
	// 		json arr = { 1, 2, 3 };
	// 		// 使用初始化列表构造对象
	// 		json obj = {
	// 			{
	// 				"user", {
	// 	            { "id", 10 },
	// 				{ "name", "Nomango" }
	// 				}
	// 			}
	// 		};
	// 第二个对象
	// json obj2 = {
	// 	{ "nul", nullptr },
	// 	{ "number", 1 },
	// 	{ "float", 1.3 },
	// 	{ "boolean", false },
	// 	{ "string", "中文测试" },
	// 	{ "array", { 1, 2, true, 1.4 } },
	// 	{ "object", {
	//        { "key", "value" },
	// 	{ "key2", "value2" }
	// 	}
	// 	};

	// std::string pretty_str = j.dump(4, ' ');
	// GraphPages[0]->Nodes[0]->Pins[0].
}



// int main() {
//     // json字符串
//     string jsonStr = "{\"name\": \"Cloudox\", \"age\": 18}";
//
//     // 转成json对象
//     char* json = (char*)jsonStr.c_str();
//     Document document;
//     document.Parse(json);
//     cout << "Hello World, I'm " << document["name"].GetString() << ", age " << document["age"].GetInt() <<  endl;
//     
//     // 添加字符串值的节点
//     Value str;
//     str = "male";
//     document.AddMember("gender", str, document.GetAllocator());// 参数：key、value
//
//     // 添加double节点
//     Value doub;
//     doub =  188.8;
//     document.AddMember("height", doub, document.GetAllocator());
//
//     cout << "my gender is " << document["gender"].GetString() << " and height is " << document["height"].GetDouble() <<  endl;
//
//     // 添加数组
//     Value arr(kArrayType);
//     Document::AllocatorType& allocator = document.GetAllocator();
//     vector<string> vec = {"read", "code", "movie", "game", "walk"};
//     for (int i = 0; i < 5; i++) {
//         // 这里很奇怪的是直接放vec[i]编译不通过，不得不转char*再转StringRef
//         arr.PushBack(StringRef(vec[i].c_str()), allocator);   // 可能需要调用 realloc() 所以需要 allocator
//     }
//     // arr.PushBack("read", allocator).PushBack("code", allocator); // 另一种组装数组的方式
//     document.AddMember("hobby", arr, allocator);
//     
//     cout << "my hobby:" << endl;
//     // 使用引用来连续访问，方便之余还更高效。
//     const Value& a = document["hobby"];
//     assert(a.IsArray());// 所有类型的值都可以先进行判断
//     for (SizeType i = 0; i < a.Size(); i++) // PS: Array 与 std::vector 相似，除了使用索引，也可使用迭代器来访问所有元素。
//         cout << a[i].GetString() << endl;
//
//     // 添加一个json子对象
//     Value uni(kObjectType);
//     uni.AddMember("name", "HUST", allocator);
//     uni.AddMember("location", "wuhan", allocator);
//     document.AddMember("university", uni, allocator);
//
//     cout << "My university is " << document["university"]["name"].GetString() << " which locate in " << document["university"]["location"].GetString() << endl;
//
//     // 转成字符串
//     StringBuffer strBuffer;  
//     Writer<StringBuffer> writer(strBuffer);  
//     document.Accept(writer);  
//     cout << strBuffer.GetString() << endl;
//     
//     system("pause");// 暂停以显示终端窗口
//     return 0;
// }
