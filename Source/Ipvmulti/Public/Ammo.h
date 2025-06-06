#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ammo.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class IPVMULTI_API AAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	AAmmo();

protected:
	virtual void BeginPlay() override;

	/** Componente de colisión para detectar el overlap con el personaje */
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* SphereComp;

	/** Malla visible del pickup (puede cambiarse en el Editor) */
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	UStaticMeshComponent* MeshComp;

	/** Función que se llama cuando otro actor entra en el SphereComp */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);
};
