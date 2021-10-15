// Copyright Epic Games, Inc. All Rights Reserved.

#include "BluescriptCommands.h"

#define LOCTEXT_NAMESPACE "FBluescriptModule"

void FBluescriptCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Bluescript", "Execute Bluescript action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
