// Copyright Epic Games, Inc. All Rights Reserved.

#include "KickBoxKinectGameMode.h"
#include "KickBoxKinectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AKickBoxKinectGameMode::AKickBoxKinectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
