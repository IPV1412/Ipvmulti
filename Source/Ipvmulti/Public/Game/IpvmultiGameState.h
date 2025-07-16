// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IpvmultiGameState.generated.h"

UCLASS()
class IPVMULTI_API AIpvmultiGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	public:
	void MulticastOnMissionComplete(APawn* InstigatorPawn, bool bMissionSuccess);
	
};
