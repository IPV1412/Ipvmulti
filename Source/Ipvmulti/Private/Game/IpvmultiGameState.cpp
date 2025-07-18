// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/IpvmultiGameState.h"

void AIpvmultiGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		if (APawn* Pawn = PC->GetPawn())
			Pawn->DisableInput(nullptr);
}
