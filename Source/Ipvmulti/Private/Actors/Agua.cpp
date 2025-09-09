// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Agua.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/Controller.h"

// Sets default values
AAgua::AAgua()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
	RootComponent = DamageBox;

	DamageBox->InitBoxExtent(FVector(200.f, 200.f, 100.f)); 
	DamageBox->SetCollisionProfileName(TEXT("Trigger"));

	//overlap
	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &AAgua::OnOverlapBegin);
	DamageBox->OnComponentEndOverlap.AddDynamic(this, &AAgua::OnOverlapEnd);


	DamageType = UDamageType::StaticClass();
	DamageAmount = 5.f;
	DamageInterval = 2.f;
	OverlappingActor = nullptr;
}

void AAgua::BeginPlay()
{
	Super::BeginPlay();
}

void AAgua::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								 bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		OverlappingActor = OtherActor;
		GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &AAgua::ApplyDamage, DamageInterval, true, 0.0f);
	}
}

void AAgua::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor == OverlappingActor)
	{
		GetWorldTimerManager().ClearTimer(DamageTimerHandle);
		OverlappingActor = nullptr;
	}
}

void AAgua::ApplyDamage()
{
	if (OverlappingActor)
	{
		UGameplayStatics::ApplyDamage(OverlappingActor, DamageAmount, GetInstigatorController(), this, DamageType);
	}
}
