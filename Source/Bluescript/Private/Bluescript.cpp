// Copyright Epic Games, Inc. All Rights Reserved.

#include "Bluescript.h"

#include "BlueprintParse.h"
#include "BluescriptStyle.h"
#include "BluescriptCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName BluescriptTabName("Bluescript");

#define LOCTEXT_NAMESPACE "FBluescriptModule"

void FBluescriptModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FBluescriptStyle::Initialize();
	FBluescriptStyle::ReloadTextures();

	FBluescriptCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FBluescriptCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FBluescriptModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FBluescriptModule::RegisterMenus));
}

void FBluescriptModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FBluescriptStyle::Shutdown();

	FBluescriptCommands::Unregister();
}

void FBluescriptModule::PluginButtonClicked()
{
	BlueprintParse::StartParse();
	// Put your "OnButtonClicked" stuff here
	// FText DialogText = FText::Format(
	// 						LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
	// 						FText::FromString(TEXT("FBluescriptModule::PluginButtonClicked()")),
	// 						FText::FromString(TEXT("Bluescript.cpp"))
	// 				   );
	// FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FBluescriptModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FBluescriptCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FBluescriptCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBluescriptModule, Bluescript)