// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjetiveZone.h"
#include "Components/BoxComponent.h"
#include "Ipvmulti/IpvmultiCharacter.h"
#include "Ipvmulti/IpvmultiGameMode.h"

AObjetiveZone::AObjetiveZone()
{
	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap Component")); OverlapComp->SetCollisionEnabled( ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToChannels ( ECR_Overlap); OverlapComp->SetCollisionResponseToChannel (ECC_Pawn, ECR_Overlap);
	RootComponent = OverlapComp;
	OverlapComp->SetHiddenInGame(false);
}
void AObjetiveZone::BeginPlay()
{
	Super::BeginPlay();
}
void AObjetiveZone::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOverlap);
}

void AObjetiveZone::HandleOverlap (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,10.f,FColor:: Green,FString::Printf(  TEXT("Overlapped")));
	}
	AIpvmultiCharacter* MyPawn = Cast<AIpvmultiCharacter>(OtherActor);
		if (MyPawn == nullptr) return;
		if (MyPawn->bCarryObjetive)
		{
			AIpvmultiGameMode* GM = Cast<AIpvmultiGameMode>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				GM->CompleteMission(MyPawn);
			}
		
		}
}

