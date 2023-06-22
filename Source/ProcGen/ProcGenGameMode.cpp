// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProcGenGameMode.h"
#include "ProcGenCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProcGenGameMode::AProcGenGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
