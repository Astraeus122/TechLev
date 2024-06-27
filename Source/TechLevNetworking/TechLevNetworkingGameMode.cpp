// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechLevNetworkingGameMode.h"
#include "TechLevNetworkingCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATechLevNetworkingGameMode::ATechLevNetworkingGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
