// Copyright Epic Games, Inc. All Rights Reserved.

#include "IpvmultiGameMode.h"
#include "Game/IpvmultiGameState.h"
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
	GameStateClass = AIpvmultiGameState::StaticClass();
	
}

void AIpvmultiGameMode::CompleteMission(APawn* Pawn)
{
	if (Pawn == nullptr) return;
	//Pawn->DisableInput(nullptr);
	if (SpectatorViewClass)
	{
		TArray<AActor*> ReturnActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpectatorViewClass, ReturnActors);
		if (ReturnActors.Num() > 0)
		{
			AActor* SpectatorActor = ReturnActors[0];
			for (FConstPlayerControllerIterator It=GetWorld()->GetPlayerControllerIterator(); It; It++)
			{
				APlayerController* PC = It->Get();
				if (PC)
				{
					PC->SetViewTargetWithBlend(SpectatorActor,1.0f,VTBlend_Cubic);
				}
			}

		}
	}
	AIpvmultiGameState* GS = GetGameState<AIpvmultiGameState>();
	if (GS)
	{
		GS->MulticastOnMissionComplete(Pawn, true);
	}
	OnMissionComplete(Pawn);
}

void AIpvmultiGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer)
	{
		FTransform SpawnTransform;
		AActor* StartSpot = FindPlayerStart(NewPlayer);
		if (StartSpot)
		{
			SpawnTransform = StartSpot->GetActorTransform();
		}
		else
		{
			SpawnTransform = FTransform();
		}
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnTransform, SpawnParams);
		
		if (NewPawn)
		{
			NewPlayer->Possess(NewPawn);
		}
	}
}