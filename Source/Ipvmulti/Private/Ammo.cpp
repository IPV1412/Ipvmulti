#include "Ammo.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Ipvmulti/IpvmultiCharacter.h"

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = false;

	// Habilitar replicación del actor (opcional, pero recomendado para multiplayer)
	bReplicates = true;

	// Crear y configurar el SphereComponent
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->InitSphereRadius(50.f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = SphereComp;

	// Crear y configurar el StaticMeshComponent (la malla visible del pickup)
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Registrar el evento de overlap solo en el servidor
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnOverlapBegin);
	}
}

void AAmmo::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						  AActor* OtherActor,
						  UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex,
						  bool bFromSweep,
						  const FHitResult& SweepResult)
{
	// Solo el servidor debe manejar la lógica de pickup
	if (!HasAuthority() || !OtherActor)
	{
		return;
	}

	// Intentar castear al personaje para llamar RestoreAmmo()
	AIpvmultiCharacter* MyPlayer = Cast<AIpvmultiCharacter>(OtherActor);
	if (MyPlayer)
	{
		MyPlayer->RestoreAmmo();
		Destroy();
	}
}

