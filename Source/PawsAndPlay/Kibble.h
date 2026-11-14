#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kibble.generated.h"

// Collectible food pellet: +1 point on touch, respawns elsewhere.
UCLASS()
class AKibble : public AActor
{
	GENERATED_BODY()

public:
	AKibble();
	virtual void NotifyActorBeginOverlap(AActor* Other) override;

private:
	UPROPERTY(VisibleAnywhere) TObjectPtr<class USphereComponent> Sphere;
	UPROPERTY(VisibleAnywhere) TObjectPtr<class UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere) TObjectPtr<class URotatingMovementComponent> Spin;
};
