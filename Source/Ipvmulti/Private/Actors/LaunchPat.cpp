// Fill out your copyright notice in the Description page of Project Settings.


#include "Ipvmulti/Public/Actors/LaunchPat.h"

#include "Components/BoxComponent.h"

// Sets default values
ALaunchPat::ALaunchPat()
{
	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	OverlapComp= CreateDefaultSubobject<UBoxComponent>("BoxComp");
	RootComponent= OverlapComp;
	MeshComp->SetupAttachment(OverlapComp);
}

// Called when the game starts or when spawned
void ALaunchPat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaunchPat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

