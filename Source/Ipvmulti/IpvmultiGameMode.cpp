// Copyright Epic Games, Inc. All Rights Reserved.

#include "IpvmultiGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AIpvmultiGameMode::AIpvmultiGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AIpvmultiGameMode::CompleteMission(APawn* Pawn)
{
	if (Pawn == nullptr) return;
	Pawn->DisableInput(nullptr);
	OnMissionComplete(Pawn);

	/*
	TArray<AActor*> FoundSpectators;
	UGameplayStatics::GetAllActorsOfClass(this, SpectatorViewClass, FoundSpectators);

	if (FoundSpectators.Num() > 0 && FoundSpectators[0])
	{
		PC->SetViewTargetWithBlend(FoundSpectators[0], 1.0f, EViewTargetBlendFunction::VTBlend_Linear);
	}
	*/

	if (SpectatorViewClass)
	{
		TArray<AActor*> ReturnActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpectatorViewClass, ReturnActors);
		if (ReturnActors.Num() > 0)
		{
			AActor* SpectatorActor = ReturnActors[0];
			APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			if (PC)
			{
				PC->SetViewTargetWithBlend(ReturnActors[0], 1.0f, EViewTargetBlendFunction::VTBlend_Cubic);
			}
		}
	}
}