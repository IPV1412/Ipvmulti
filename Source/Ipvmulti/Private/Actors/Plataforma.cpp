#include "Actors/Plataforma.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

APlataforma::APlataforma()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true); 

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	MoveDistance = 300.f;
	MoveSpeed = 100.f;
	bMovingToTarget = true;
}

void APlataforma::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) 
	{
		StartLocation = GetActorLocation();
		TargetLocation = StartLocation + FVector(MoveDistance, 0, 0); 
	}
}

void APlataforma::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority()) 
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Destination = bMovingToTarget ? TargetLocation : StartLocation;

		// Mover hacia destino
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, Destination, DeltaTime, MoveSpeed);
		SetActorLocation(NewLocation);

		// Si llegó, invertir dirección
		if (FVector::Dist(NewLocation, Destination) < 5.f)
		{
			bMovingToTarget = !bMovingToTarget;
		}
	}
}

void APlataforma::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlataforma, MoveDistance);
	DOREPLIFETIME(APlataforma, MoveSpeed);
	DOREPLIFETIME(APlataforma, StartLocation);
	DOREPLIFETIME(APlataforma, TargetLocation);
}
