// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IpvmultiGameState.h"

#include "Player/IpvmultiPlayerController.h"

void AIpvmultiGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AIpvmultiPlayerController* PC = Cast<AIpvmultiPlayerController>(It->Get());
		if (PC)
		{
			PC->OnMisionComplete(InstigatorPawn, bMissionSuccess);
			APawn* Pawn = PC->GetPawn();
			if (Pawn)
			{
				Pawn->DisableInput(nullptr);
			}
		}
	}

	/*
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		if (APawn* Pawn = PC->GetPawn())
			Pawn->DisableInput(nullptr);
	*/
}
