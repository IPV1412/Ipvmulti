// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IpvmultiGameMode.generated.h"

UCLASS(minimalapi)
class AIpvmultiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIpvmultiGameMode();

	void CompleteMission(APawn* Pawn);
	
	UFUNCTION(BlueprintImplementableEvent, Category = GM)
	void OnMissionComplete(APawn* Pawn);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<AActor> SpectatorViewClass;
};



