// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ObjectiveActor.h"

#include "Components/SphereComponent.h"
#include "Ipvmulti/IpvmultiCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AObjectiveActor::AObjectiveActor()
{
	bReplicates = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //Solo es un registro de los actores
	SphereComp->SetCollisionResponseToChannels(ECR_Ignore); //Toma todos los canales de colición y los ignora
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap); //Solo el Pawn puede hacer overlab
	SphereComp->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AObjectiveActor::PlayEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(this,PickUpVFX,GetActorLocation());
}

// Called every frame
void AObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AObjectiveActor::NotifyActorBeginOverlap(class AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	PlayEffects();

	AIpvmultiCharacter* MyCharacter = Cast<AIpvmultiCharacter>(OtherActor);
	if (MyCharacter)
	{
		MyCharacter->bCarryObjetive = true;
		Destroy();
	}
}
