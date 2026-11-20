#include "Kibble.h"
#include "PawsGameMode.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

AKibble::AKibble()
{
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->InitSphereRadius(60.f);
	Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Sphere;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Sphere);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.25f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Mat(TEXT("/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold"));
	if (Mat.Succeeded())
	{
		Mesh->SetMaterial(0, Mat.Object);
	}

	Spin = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Spin"));
	Spin->RotationRate = FRotator(0.f, 120.f, 0.f);

	static ConstructorHelpers::FObjectFinder<USoundBase> Sound(TEXT("/Engine/VREditor/Sounds/UI/Click_on_Button.Click_on_Button"));
	PickupSound = Sound.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FX(TEXT("/Game/StarterContent/Particles/P_Sparks.P_Sparks"));
	PickupFX = FX.Object;
}

void AKibble::NotifyActorBeginOverlap(AActor* Other)
{
	const APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn || !Pawn->IsPlayerControlled())
	{
		return;
	}
	if (APawsGameMode* GM = GetWorld()->GetAuthGameMode<APawsGameMode>())
	{
		GM->AddPoints(1);
		GM->SpawnKibble();
	}
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	if (PickupFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupFX, GetActorLocation(), FRotator::ZeroRotator, FVector(0.5f));
	}
	Destroy();
}
