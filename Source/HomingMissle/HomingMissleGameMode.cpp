// Copyright Epic Games, Inc. All Rights Reserved.

#include "HomingMissleGameMode.h"
#include "HomingMissleCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHomingMissleGameMode::AHomingMissleGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
