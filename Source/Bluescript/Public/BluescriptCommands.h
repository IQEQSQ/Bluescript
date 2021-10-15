// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "BluescriptStyle.h"

class FBluescriptCommands : public TCommands<FBluescriptCommands>
{
public:

	FBluescriptCommands()
		: TCommands<FBluescriptCommands>(TEXT("Bluescript"), NSLOCTEXT("Contexts", "Bluescript", "Bluescript Plugin"), NAME_None, FBluescriptStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
