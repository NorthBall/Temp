// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerController_Main.h"
#include "DataPawn.h"
#include "SecondTaskGameModeBase.h"

ASecondTaskGameModeBase::ASecondTaskGameModeBase()
{
	PlayerControllerClass = APlayerController_Main::StaticClass();
	DefaultPawnClass = ADataPawn::StaticClass();
}
