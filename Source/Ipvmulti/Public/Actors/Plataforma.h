#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Plataforma.generated.h"

UCLASS()
class IPVMULTI_API APlataforma : public AActor
{
	GENERATED_BODY()
	
public:	
	APlataforma();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// Malla de la plataforma
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plataforma")
	class UStaticMeshComponent* Mesh;

	// Posición inicial (se replicará)
	UPROPERTY(Replicated)
	FVector StartLocation;

	// Posición destino (se calcula con Offset)
	UPROPERTY(Replicated)
	FVector TargetLocation;

	// Dirección de movimiento
	bool bMovingToTarget;

	// === Variables públicas y replicables ===
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Plataforma")
	float MoveDistance; // cuanto se mueve

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Plataforma")
	float MoveSpeed; // velocidad

	// Función para replicación
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
