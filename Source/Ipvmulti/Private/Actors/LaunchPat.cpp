// Fill out your copyright notice in the Description page of Project Settings.


#include "Ipvmulti/Public/Actors/LaunchPat.h"


// Sets default values
ALaunchPat::ALaunchPat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

