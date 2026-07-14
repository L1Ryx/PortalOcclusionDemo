#include "PortalDemoMovingDoor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
float Smooth01(float Alpha)
{
	const float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	return ClampedAlpha * ClampedAlpha * (3.0f - 2.0f * ClampedAlpha);
}
}

APortalDemoMovingDoor::APortalDemoMovingDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneRoot->SetMobility(EComponentMobility::Movable);
	SetRootComponent(SceneRoot);

	DoorPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanel"));
	DoorPanel->SetupAttachment(SceneRoot);
	DoorPanel->SetMobility(EComponentMobility::Movable);
	DoorPanel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		CubeMesh = CubeMeshFinder.Object;
		DoorPanel->SetStaticMesh(CubeMesh);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> AccentMaterialFinder(TEXT("/Game/FirstPerson/MI_FirstPersonColorway.MI_FirstPersonColorway"));
	if (AccentMaterialFinder.Succeeded())
	{
		AccentMaterial = AccentMaterialFinder.Object;
		DoorPanel->SetMaterial(0, AccentMaterial);
	}
}

void APortalDemoMovingDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DoorPanel->SetRelativeScale3D(DoorSize / 100.0f);
	ApplyOpenFraction(bAutoAnimate ? PreviewOpenFraction : OpenFraction);
}

void APortalDemoMovingDoor::BeginPlay()
{
	Super::BeginPlay();

	RuntimeSeconds = PhaseOffsetSeconds;
	ApplyOpenFraction(bAutoAnimate ? ComputeAnimatedOpenFraction(RuntimeSeconds) : PreviewOpenFraction);
}

void APortalDemoMovingDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bAutoAnimate)
	{
		ApplyOpenFraction(PreviewOpenFraction);
		return;
	}

	RuntimeSeconds += DeltaSeconds;
	ApplyOpenFraction(ComputeAnimatedOpenFraction(RuntimeSeconds));
}

float APortalDemoMovingDoor::ComputeAnimatedOpenFraction(float TimeSeconds) const
{
	const float SafeTravelSeconds = FMath::Max(TravelSeconds, 0.05f);
	const float TotalSeconds = FMath::Max(ClosedHoldSeconds + SafeTravelSeconds + OpenHoldSeconds + SafeTravelSeconds, 0.1f);
	const float WrappedSeconds = FMath::Fmod(FMath::Max(TimeSeconds, 0.0f), TotalSeconds);

	if (WrappedSeconds < ClosedHoldSeconds)
	{
		return 0.0f;
	}

	const float OpeningStart = ClosedHoldSeconds;
	const float OpeningEnd = OpeningStart + SafeTravelSeconds;
	if (WrappedSeconds < OpeningEnd)
	{
		return Smooth01((WrappedSeconds - OpeningStart) / SafeTravelSeconds);
	}

	const float ClosingStart = OpeningEnd + OpenHoldSeconds;
	if (WrappedSeconds < ClosingStart)
	{
		return 1.0f;
	}

	return 1.0f - Smooth01((WrappedSeconds - ClosingStart) / SafeTravelSeconds);
}

void APortalDemoMovingDoor::ApplyOpenFraction(float NewOpenFraction)
{
	OpenFraction = FMath::Clamp(NewOpenFraction, 0.0f, 1.0f);

	if (DoorPanel)
	{
		DoorPanel->SetRelativeLocation(ClosedPanelOffset + DoorTravelOffset * OpenFraction);
		DoorPanel->SetRelativeScale3D(DoorSize / 100.0f);
	}
}
