// Fill out your copyright notice in the Description page of Project Settings.


#include "Ipvmulti/Public/Actors/LaunchPat.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ALaunchPat::ALaunchPat()
{
	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	OverlapComp= CreateDefaultSubobject<UBoxComponent>("BoxComp");
	RootComponent= OverlapComp;
	MeshComp->SetupAttachment(OverlapComp);
	Launchforce=10000;
	LaunchAngle=90.0; //trampolin
}

// Called when the game starts or when spawned
void ALaunchPat::BeginPlay()
{
	Super::BeginPlay();
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this,&ALaunchPat::OverlapLaunchpat);	
}

void ALaunchPat::OverlapLaunchpat(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	FRotator LaunchDirection=GetActorRotation();
	LaunchDirection.Pitch+=LaunchAngle; //Suma a si mismo
	FVector LaunchVelocity=LaunchDirection.Vector()*Launchforce;
	
	ACharacter*MyCharacter = Cast<ACharacter>(OtherActor);
	if (MyCharacter)
	{
		MyCharacter->LaunchCharacter(LaunchVelocity,true,true);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Overlap"));
		}
	}
}

// Called every frame
void ALaunchPat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

