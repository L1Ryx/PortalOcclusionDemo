#include "PortalBlockoutEnvironment.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

APortalBlockoutEnvironment::APortalBlockoutEnvironment()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneRoot->SetMobility(EComponentMobility::Static);
	SetRootComponent(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		CubeMesh = CubeMeshFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshFinder(TEXT("/Game/LevelPrototyping/Meshes/SM_Plane.SM_Plane"));
	if (PlaneMeshFinder.Succeeded())
	{
		PlaneMesh = PlaneMeshFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GridMaterialFinder(TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray.MI_PrototypeGrid_Gray"));
	if (GridMaterialFinder.Succeeded())
	{
		GridMaterial = GridMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DarkGridMaterialFinder(TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_TopDark.MI_PrototypeGrid_TopDark"));
	if (DarkGridMaterialFinder.Succeeded())
	{
		DarkGridMaterial = DarkGridMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> AccentMaterialFinder(TEXT("/Game/LevelPrototyping/Materials/MI_DefaultColorway.MI_DefaultColorway"));
	if (AccentMaterialFinder.Succeeded())
	{
		AccentMaterial = AccentMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SecondaryGridMaterialFinder(TEXT("/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray_02.MI_PrototypeGrid_Gray_02"));
	if (SecondaryGridMaterialFinder.Succeeded())
	{
		SecondaryGridMaterial = SecondaryGridMaterialFinder.Object;
	}
}

void APortalBlockoutEnvironment::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ClearGeneratedComponents();
	BuildEnvironment();
}

void APortalBlockoutEnvironment::ClearGeneratedComponents()
{
	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Component : Components)
	{
		if (Component && Component->CreationMethod == EComponentCreationMethod::UserConstructionScript)
		{
			Component->DestroyComponent();
		}
	}

	GeneratedComponentIndex = 0;
}

void APortalBlockoutEnvironment::BuildEnvironment()
{
	if (!CubeMesh)
	{
		return;
	}

	const float S = EnvironmentScale;
	const float H = WallHeight * S;
	const float T = WallThickness * S;

	// Clean single-floor acoustic lab layout. Every wall sits on the same slab
	// and each wall span is authored once to avoid overlapping coplanar faces.
	AddFloor(TEXT("MainWalkableFloor"), FVector2D(400.0f, 0.0f) * S, FVector2D(5400.0f, 3600.0f) * S, GridMaterial);

	// Control room.
	AddWallY(TEXT("ControlWest"), -2100.0f * S, -500.0f * S, 500.0f * S);
	AddWallX(TEXT("ControlNorth"), 500.0f * S, -2100.0f * S, -900.0f * S);
	AddWallX(TEXT("ControlSouth"), -500.0f * S, -2100.0f * S, -900.0f * S);

	// Atrium shell with openings west/east/north/south.
	AddWallY(TEXT("AtriumWestSouth"), -900.0f * S, -700.0f * S, -160.0f * S);
	AddWallY(TEXT("AtriumWestNorth"), -900.0f * S, 160.0f * S, 700.0f * S);
	AddWallY(TEXT("AtriumEastSouth"), 500.0f * S, -700.0f * S, -160.0f * S);
	AddWallY(TEXT("AtriumEastNorth"), 500.0f * S, 160.0f * S, 700.0f * S);
	AddWallX(TEXT("AtriumNorthWest"), 700.0f * S, -900.0f * S, -240.0f * S);
	AddWallX(TEXT("AtriumNorthEast"), 700.0f * S, 240.0f * S, 500.0f * S);
	AddWallX(TEXT("AtriumSouthWest"), -700.0f * S, -900.0f * S, -240.0f * S);
	AddWallX(TEXT("AtriumSouthEast"), -700.0f * S, 240.0f * S, 500.0f * S);

	// East corridor and listening bay.
	AddWallX(TEXT("CorridorSouth"), -300.0f * S, 500.0f * S, 2100.0f * S);
	AddWallX(TEXT("CorridorNorthLeft"), 300.0f * S, 500.0f * S, 900.0f * S);
	AddWallX(TEXT("CorridorNorthMachineryWest"), 300.0f * S, 900.0f * S, 1350.0f * S);
	AddWallX(TEXT("CorridorNorthMachineryEast"), 300.0f * S, 1650.0f * S, 2100.0f * S);
	AddWallY(TEXT("ListeningDividerSouth"), 2100.0f * S, -350.0f * S, -120.0f * S);
	AddWallY(TEXT("ListeningDividerNorth"), 2100.0f * S, 120.0f * S, 350.0f * S);
	AddWallX(TEXT("ListeningNorth"), 350.0f * S, 2100.0f * S, 3000.0f * S);
	AddWallX(TEXT("ListeningSouth"), -350.0f * S, 2100.0f * S, 3000.0f * S);
	AddWallY(TEXT("ListeningEast"), 3000.0f * S, -350.0f * S, 350.0f * S);

	// Reverb chamber.
	AddWallY(TEXT("ReverbWest"), -700.0f * S, 700.0f * S, 1600.0f * S);
	AddWallY(TEXT("ReverbEast"), 500.0f * S, 700.0f * S, 1600.0f * S);
	AddWallX(TEXT("ReverbNorth"), 1600.0f * S, -700.0f * S, 500.0f * S);

	// Machinery hall.
	AddWallY(TEXT("MachineryWest"), 900.0f * S, 300.0f * S, 1300.0f * S);
	AddWallY(TEXT("MachineryEast"), 2100.0f * S, 300.0f * S, 1300.0f * S);
	AddWallX(TEXT("MachineryNorth"), 1300.0f * S, 900.0f * S, 2100.0f * S);

	// Courtyard / outdoor-ish test space.
	AddWallY(TEXT("CourtyardWest"), -300.0f * S, -1600.0f * S, -700.0f * S);
	AddWallY(TEXT("CourtyardEast"), 1500.0f * S, -1600.0f * S, -700.0f * S);
	AddWallX(TEXT("CourtyardSouth"), -1600.0f * S, -300.0f * S, 1500.0f * S);
	AddWallX(TEXT("CourtyardNorthWest"), -700.0f * S, -300.0f * S, -240.0f * S);
	AddWallX(TEXT("CourtyardNorthEast"), -700.0f * S, 240.0f * S, 1500.0f * S);

	// Small, non-overlapping portal indicators and partial doors. These are
	// visual placeholders for the dynamic portal components we will add next.
	AddBox(TEXT("P01_AtriumReverbThreshold"), FVector(0.0f, 700.0f, 8.0f) * S, FVector(440.0f, 22.0f, 16.0f) * S, AccentMaterial, false);
	AddBox(TEXT("P02_AtriumCourtyardThreshold"), FVector(0.0f, -700.0f, 8.0f) * S, FVector(440.0f, 22.0f, 16.0f) * S, AccentMaterial, false);
	AddBox(TEXT("P03_ControlAtriumThreshold"), FVector(-900.0f, 0.0f, 8.0f) * S, FVector(22.0f, 320.0f, 16.0f) * S, AccentMaterial, false);
	AddBox(TEXT("P04_AtriumCorridorThreshold"), FVector(500.0f, 0.0f, 8.0f) * S, FVector(22.0f, 320.0f, 16.0f) * S, AccentMaterial, false);
	AddBox(TEXT("P05_CorridorMachineryThreshold"), FVector(1500.0f, 300.0f, 8.0f) * S, FVector(300.0f, 22.0f, 16.0f) * S, AccentMaterial, false);
	AddBox(TEXT("P06_CorridorListeningThreshold"), FVector(2100.0f, 0.0f, 8.0f) * S, FVector(22.0f, 240.0f, 16.0f) * S, AccentMaterial, false);

	AddBox(TEXT("Door_P01_HalfOpen"), FVector(135.0f, 716.0f, H * 0.42f), FVector(150.0f, 18.0f, H * 0.84f), AccentMaterial, true);
	AddBox(TEXT("Door_P03_Sliding"), FVector(-916.0f, 105.0f, H * 0.42f), FVector(18.0f, 110.0f, H * 0.84f), AccentMaterial, true);
	AddBox(TEXT("Door_P05_Partial"), FVector(1600.0f, 316.0f, H * 0.42f), FVector(105.0f, 18.0f, H * 0.84f), AccentMaterial, true);

	// Small source/listener markers. Labels remain off unless explicitly enabled.
	AddMarker(TEXT("Source_A_Courtyard"), FVector(600.0f, -1250.0f, 38.0f) * S, FVector(90.0f, 90.0f, 76.0f) * S);
	AddMarker(TEXT("Source_B_Machinery"), FVector(1500.0f, 940.0f, 38.0f) * S, FVector(90.0f, 90.0f, 76.0f) * S);
	AddMarker(TEXT("Listener_Start"), FVector(-1500.0f, 0.0f, 32.0f) * S, FVector(100.0f, 100.0f, 64.0f) * S);
	AddMarker(TEXT("Listener_ComparePoint"), FVector(2550.0f, 0.0f, 32.0f) * S, FVector(100.0f, 100.0f, 64.0f) * S);

	for (int32 i = 0; i < 6; ++i)
	{
		const float Alpha = static_cast<float>(i) / 5.0f;
		const FVector PathLocation = FMath::Lerp(FVector(600.0f, -1250.0f, 18.0f), FVector(1500.0f, 940.0f, 18.0f), Alpha) * S;
		AddBox(FString::Printf(TEXT("MovingSourcePath_%02d"), i), PathLocation, FVector(42.0f, 42.0f, 18.0f) * S, AccentMaterial, false);
	}

	AddPointLight(TEXT("AtriumLight"), FVector(-100.0f, 0.0f, 430.0f) * S, FLinearColor(0.95f, 0.88f, 0.72f), 1400.0f, 1400.0f);
	AddPointLight(TEXT("ReverbLight"), FVector(-100.0f, 1120.0f, 420.0f) * S, FLinearColor(0.55f, 0.72f, 1.0f), 900.0f, 1100.0f);
	AddPointLight(TEXT("MachineryLight"), FVector(1500.0f, 850.0f, 420.0f) * S, FLinearColor(1.0f, 0.58f, 0.32f), 950.0f, 1100.0f);
	AddPointLight(TEXT("CourtyardLight"), FVector(600.0f, -1200.0f, 450.0f) * S, FLinearColor(0.72f, 0.86f, 1.0f), 1200.0f, 1500.0f);

	return;

#if 0
	// Room floor plates: arranged to exercise direct, indirect, multi-portal, and courtyard propagation paths.
	AddFloor(TEXT("AtriumFloor"), FVector2D(0.0f, 0.0f) * S, FVector2D(1400.0f, 1200.0f) * S, GridMaterial);
	AddFloor(TEXT("ControlFloor"), FVector2D(-1200.0f, 0.0f) * S, FVector2D(1000.0f, 900.0f) * S, SecondaryGridMaterial ? SecondaryGridMaterial : GridMaterial);
	AddFloor(TEXT("EastCorridorFloor"), FVector2D(1600.0f, 0.0f) * S, FVector2D(1800.0f, 500.0f) * S, GridMaterial);
	AddFloor(TEXT("ReverbChamberFloor"), FVector2D(0.0f, 1050.0f) * S, FVector2D(1000.0f, 900.0f) * S, DarkGridMaterial ? DarkGridMaterial : GridMaterial);
	AddFloor(TEXT("MachineryHallFloor"), FVector2D(1400.0f, 1150.0f) * S, FVector2D(1000.0f, 900.0f) * S, SecondaryGridMaterial ? SecondaryGridMaterial : GridMaterial);
	AddFloor(TEXT("CourtyardFloor"), FVector2D(450.0f, -1050.0f) * S, FVector2D(1700.0f, 900.0f) * S, GridMaterial);
	AddFloor(TEXT("ListeningBayFloor"), FVector2D(2100.0f, -600.0f) * S, FVector2D(800.0f, 700.0f) * S, DarkGridMaterial ? DarkGridMaterial : GridMaterial);

	// Atrium shell with four major openings.
	AddWallX(TEXT("AtriumNorthWest"), 600.0f * S, -700.0f * S, -220.0f * S);
	AddWallX(TEXT("AtriumNorthEast"), 600.0f * S, 220.0f * S, 700.0f * S);
	AddWallX(TEXT("AtriumSouthWest"), -600.0f * S, -700.0f * S, -120.0f * S);
	AddWallX(TEXT("AtriumSouthEast"), -600.0f * S, 320.0f * S, 700.0f * S);
	AddWallY(TEXT("AtriumWestSouth"), -700.0f * S, -600.0f * S, -180.0f * S);
	AddWallY(TEXT("AtriumWestNorth"), -700.0f * S, 180.0f * S, 600.0f * S);
	AddWallY(TEXT("AtriumEastSouth"), 700.0f * S, -600.0f * S, -160.0f * S);
	AddWallY(TEXT("AtriumEastNorth"), 700.0f * S, 160.0f * S, 600.0f * S);

	// Adjacent spaces.
	AddWallX(TEXT("ControlNorth"), 450.0f * S, -1700.0f * S, -700.0f * S);
	AddWallX(TEXT("ControlSouth"), -450.0f * S, -1700.0f * S, -700.0f * S);
	AddWallY(TEXT("ControlWest"), -1700.0f * S, -450.0f * S, 450.0f * S);

	AddWallX(TEXT("CorridorNorth"), 250.0f * S, 700.0f * S, 2500.0f * S);
	AddWallX(TEXT("CorridorSouthWest"), -250.0f * S, 700.0f * S, 1770.0f * S);
	AddWallX(TEXT("CorridorSouthEast"), -250.0f * S, 2230.0f * S, 2500.0f * S);
	AddWallY(TEXT("CorridorEastCap"), 2500.0f * S, -250.0f * S, 250.0f * S);

	AddWallY(TEXT("ReverbWest"), -500.0f * S, 600.0f * S, 1500.0f * S);
	AddWallY(TEXT("ReverbEast"), 500.0f * S, 600.0f * S, 1500.0f * S);
	AddWallX(TEXT("ReverbNorthWest"), 1500.0f * S, -500.0f * S, -80.0f * S);
	AddWallX(TEXT("ReverbNorthEast"), 1500.0f * S, 260.0f * S, 500.0f * S);

	AddWallY(TEXT("MachineryWestSouth"), 900.0f * S, 700.0f * S, 1030.0f * S);
	AddWallY(TEXT("MachineryWestNorth"), 900.0f * S, 1270.0f * S, 1600.0f * S);
	AddWallY(TEXT("MachineryEast"), 1900.0f * S, 700.0f * S, 1600.0f * S);
	AddWallX(TEXT("MachineryNorth"), 1600.0f * S, 900.0f * S, 1900.0f * S);
	AddWallX(TEXT("MachinerySouth"), 700.0f * S, 900.0f * S, 1900.0f * S);

	AddWallX(TEXT("CourtyardSouth"), -1500.0f * S, -400.0f * S, 1300.0f * S);
	AddWallY(TEXT("CourtyardWest"), -400.0f * S, -1500.0f * S, -600.0f * S);
	AddWallY(TEXT("CourtyardEast"), 1300.0f * S, -1500.0f * S, -600.0f * S);
	AddWallX(TEXT("CourtyardNorthWest"), -600.0f * S, -400.0f * S, -120.0f * S);
	AddWallX(TEXT("CourtyardNorthEast"), -600.0f * S, 320.0f * S, 1300.0f * S);

	AddWallX(TEXT("ListeningBayNorthWest"), -250.0f * S, 1700.0f * S, 1780.0f * S);
	AddWallX(TEXT("ListeningBayNorthEast"), -250.0f * S, 2220.0f * S, 2500.0f * S);
	AddWallY(TEXT("ListeningBayEast"), 2500.0f * S, -950.0f * S, -250.0f * S);
	AddWallY(TEXT("ListeningBayWest"), 1700.0f * S, -950.0f * S, -250.0f * S);
	AddWallX(TEXT("ListeningBaySouth"), -950.0f * S, 1700.0f * S, 2500.0f * S);

	// Portal frames and future dynamic-door panels.
	AddPortalOnXWall(TEXT("P01 AtriumToReverb"), 600.0f * S, 0.0f * S, 440.0f * S);
	AddPortalOnXWall(TEXT("P02 AtriumToCourtyard"), -600.0f * S, 100.0f * S, 440.0f * S);
	AddPortalOnYWall(TEXT("P03 ControlToAtrium"), -700.0f * S, 0.0f * S, 360.0f * S);
	AddPortalOnYWall(TEXT("P04 AtriumToCorridor"), 700.0f * S, 0.0f * S, 320.0f * S);
	AddPortalOnYWall(TEXT("P05 CorridorToMachinery"), 900.0f * S, 1150.0f * S, 260.0f * S);
	AddPortalOnXWall(TEXT("P06 CorridorToListeningBay"), -250.0f * S, 2000.0f * S, 440.0f * S);
	AddPortalOnXWall(TEXT("P07 ReverbToUpperVent"), 1500.0f * S, 90.0f * S, 340.0f * S);

	AddBox(TEXT("DoorPanel_P01_HalfOpen"), FVector(180.0f, 615.0f, H * 0.42f), FVector(170.0f, T * 0.55f, H * 0.84f), AccentMaterial, true);
	AddBox(TEXT("DoorPanel_P03_Sliding"), FVector(-715.0f, 130.0f, H * 0.42f), FVector(T * 0.55f, 140.0f, H * 0.84f), AccentMaterial, true);
	AddBox(TEXT("DoorPanel_P05_Closing"), FVector(915.0f, 1060.0f, H * 0.42f), FVector(T * 0.55f, 150.0f, H * 0.84f), AccentMaterial, true);

	// Source, listener, and motion-path markers for later audio-system work.
	AddMarker(TEXT("Source_A_Courtyard"), FVector(450.0f, -1180.0f, 45.0f) * S, FVector(110.0f, 110.0f, 90.0f) * S);
	AddMarker(TEXT("Source_B_Machinery"), FVector(1540.0f, 1220.0f, 45.0f) * S, FVector(110.0f, 110.0f, 90.0f) * S);
	AddMarker(TEXT("Listener_Start"), FVector(-1200.0f, -120.0f, 35.0f) * S, FVector(130.0f, 130.0f, 70.0f) * S);
	AddMarker(TEXT("Listener_ComparePoint"), FVector(2140.0f, -610.0f, 35.0f) * S, FVector(130.0f, 130.0f, 70.0f) * S);

	for (int32 i = 0; i < 7; ++i)
	{
		const float Alpha = static_cast<float>(i) / 6.0f;
		const FVector PathLocation = FMath::Lerp(FVector(450.0f, -1180.0f, 28.0f), FVector(1540.0f, 1220.0f, 28.0f), Alpha) * S;
		AddBox(FString::Printf(TEXT("MovingSourcePath_%02d"), i), PathLocation, FVector(55.0f, 55.0f, 24.0f) * S, AccentMaterial, false);
	}

	// Overhead guides help sell scale while keeping the audio sightlines clear.
	if (bBuildCeilingGuides)
	{
		AddBox(TEXT("AtriumCeilingGuide"), FVector(0.0f, 0.0f, H + 24.0f), FVector(1360.0f, 24.0f, 16.0f), AccentMaterial, false);
		AddBox(TEXT("CorridorCeilingGuide"), FVector(1600.0f, 0.0f, H + 24.0f), FVector(1760.0f, 24.0f, 16.0f), AccentMaterial, false);
		AddBox(TEXT("NorthCeilingGuide"), FVector(700.0f, 1150.0f, H + 24.0f), FVector(1700.0f, 24.0f, 16.0f), AccentMaterial, false);
	}

	if (bBuildLabels)
	{
		AddLabel(TEXT("A Atrium"), FVector(-260.0f, -250.0f, H + 30.0f) * S, 0.0f, 46.0f * S);
		AddLabel(TEXT("B Control"), FVector(-1350.0f, -250.0f, H + 30.0f) * S, 0.0f, 42.0f * S);
		AddLabel(TEXT("C Corridor"), FVector(1540.0f, -80.0f, H + 30.0f) * S, 0.0f, 42.0f * S);
		AddLabel(TEXT("D Reverb"), FVector(-210.0f, 1020.0f, H + 30.0f) * S, 0.0f, 42.0f * S);
		AddLabel(TEXT("E Machinery"), FVector(1220.0f, 1180.0f, H + 30.0f) * S, 0.0f, 42.0f * S);
		AddLabel(TEXT("F Courtyard"), FVector(260.0f, -1130.0f, H + 30.0f) * S, 0.0f, 42.0f * S);
		AddLabel(TEXT("P01"), FVector(0.0f, 610.0f, H + 35.0f) * S, 0.0f, 32.0f * S);
		AddLabel(TEXT("P03"), FVector(-710.0f, 0.0f, H + 35.0f) * S, 90.0f, 32.0f * S);
		AddLabel(TEXT("P05"), FVector(910.0f, 1150.0f, H + 35.0f) * S, 90.0f, 32.0f * S);
	}

	AddPointLight(TEXT("AtriumLight"), FVector(0.0f, 0.0f, 520.0f) * S, FLinearColor(0.95f, 0.88f, 0.72f), 2200.0f, 1500.0f);
	AddPointLight(TEXT("ReverbLight"), FVector(0.0f, 1050.0f, 520.0f) * S, FLinearColor(0.55f, 0.72f, 1.0f), 1500.0f, 1100.0f);
	AddPointLight(TEXT("MachineryLight"), FVector(1500.0f, 1150.0f, 500.0f) * S, FLinearColor(1.0f, 0.58f, 0.32f), 1600.0f, 1100.0f);
	AddPointLight(TEXT("CourtyardLight"), FVector(450.0f, -1050.0f, 560.0f) * S, FLinearColor(0.72f, 0.86f, 1.0f), 2000.0f, 1500.0f);

	#undef S
	#undef H
	#undef T
#endif
}

void APortalBlockoutEnvironment::AddBox(const FString& Label, const FVector& Location, const FVector& Size, UMaterialInterface* Material, bool bCollision)
{
	if (!CubeMesh || Size.IsNearlyZero())
	{
		return;
	}

	UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(
		this,
		*FString::Printf(TEXT("Generated_%03d_%s"), GeneratedComponentIndex++, *Label));

	MeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	MeshComponent->SetStaticMesh(CubeMesh);
	MeshComponent->SetMobility(EComponentMobility::Static);
	MeshComponent->SetRelativeLocation(Location);
	MeshComponent->SetRelativeScale3D(Size / 100.0f);
	MeshComponent->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);

	if (Material)
	{
		MeshComponent->SetMaterial(0, Material);
	}

	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AddInstanceComponent(MeshComponent);
	MeshComponent->RegisterComponent();
}

void APortalBlockoutEnvironment::AddFloor(const FString& Label, const FVector2D& Center, const FVector2D& Size, UMaterialInterface* Material)
{
	AddBox(Label, FVector(Center.X, Center.Y, -6.0f * EnvironmentScale), FVector(Size.X, Size.Y, 12.0f * EnvironmentScale), Material, true);
}

void APortalBlockoutEnvironment::AddWallX(const FString& Label, float Y, float XMin, float XMax)
{
	const float Length = FMath::Abs(XMax - XMin);
	if (Length <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	AddBox(Label, FVector((XMin + XMax) * 0.5f, Y, WallHeight * EnvironmentScale * 0.5f), FVector(Length, WallThickness * EnvironmentScale, WallHeight * EnvironmentScale), GridMaterial, true);
}

void APortalBlockoutEnvironment::AddWallY(const FString& Label, float X, float YMin, float YMax)
{
	const float Length = FMath::Abs(YMax - YMin);
	if (Length <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	AddBox(Label, FVector(X, (YMin + YMax) * 0.5f, WallHeight * EnvironmentScale * 0.5f), FVector(WallThickness * EnvironmentScale, Length, WallHeight * EnvironmentScale), GridMaterial, true);
}

void APortalBlockoutEnvironment::AddPortalOnXWall(const FString& Label, float Y, float XCenter, float Width)
{
	const float S = EnvironmentScale;
	const float PostWidth = 70.0f * S;
	const float H = WallHeight * S;
	const float T = WallThickness * S;
	const float HalfWidth = Width * 0.5f;

	AddBox(Label + TEXT("_LeftPost"), FVector(XCenter - HalfWidth, Y, H * 0.5f), FVector(PostWidth, T * 1.5f, H), AccentMaterial, true);
	AddBox(Label + TEXT("_RightPost"), FVector(XCenter + HalfWidth, Y, H * 0.5f), FVector(PostWidth, T * 1.5f, H), AccentMaterial, true);
	AddBox(Label + TEXT("_Lintel"), FVector(XCenter, Y, H - 35.0f * S), FVector(Width + PostWidth, T * 1.5f, 70.0f * S), AccentMaterial, true);
	AddBox(Label + TEXT("_SensorPlate"), FVector(XCenter, Y, 12.0f * S), FVector(Width, T * 2.0f, 24.0f * S), AccentMaterial, false);
}

void APortalBlockoutEnvironment::AddPortalOnYWall(const FString& Label, float X, float YCenter, float Width)
{
	const float S = EnvironmentScale;
	const float PostWidth = 70.0f * S;
	const float H = WallHeight * S;
	const float T = WallThickness * S;
	const float HalfWidth = Width * 0.5f;

	AddBox(Label + TEXT("_LowerPost"), FVector(X, YCenter - HalfWidth, H * 0.5f), FVector(T * 1.5f, PostWidth, H), AccentMaterial, true);
	AddBox(Label + TEXT("_UpperPost"), FVector(X, YCenter + HalfWidth, H * 0.5f), FVector(T * 1.5f, PostWidth, H), AccentMaterial, true);
	AddBox(Label + TEXT("_Lintel"), FVector(X, YCenter, H - 35.0f * S), FVector(T * 1.5f, Width + PostWidth, 70.0f * S), AccentMaterial, true);
	AddBox(Label + TEXT("_SensorPlate"), FVector(X, YCenter, 12.0f * S), FVector(T * 2.0f, Width, 24.0f * S), AccentMaterial, false);
}

void APortalBlockoutEnvironment::AddMarker(const FString& Label, const FVector& Location, const FVector& Size)
{
	AddBox(Label, Location, Size, AccentMaterial, false);
	if (bBuildLabels)
	{
		AddLabel(Label.Replace(TEXT("_"), TEXT(" ")), Location + FVector(-Size.X * 0.8f, -Size.Y * 0.8f, Size.Z + 55.0f * EnvironmentScale), 0.0f, 28.0f * EnvironmentScale);
	}
}

void APortalBlockoutEnvironment::AddLabel(const FString& Text, const FVector& Location, float YawDegrees, float Size)
{
	UTextRenderComponent* TextComponent = NewObject<UTextRenderComponent>(
		this,
		*FString::Printf(TEXT("Generated_%03d_Label"), GeneratedComponentIndex++));

	TextComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	TextComponent->SetMobility(EComponentMobility::Static);
	TextComponent->SetRelativeLocation(Location);
	TextComponent->SetRelativeRotation(FRotator(0.0f, YawDegrees, 0.0f));
	TextComponent->SetText(FText::FromString(Text));
	TextComponent->SetTextRenderColor(FColor(255, 170, 70));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComponent->SetWorldSize(Size);

	TextComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AddInstanceComponent(TextComponent);
	TextComponent->RegisterComponent();
}

void APortalBlockoutEnvironment::AddPointLight(const FString& Label, const FVector& Location, FLinearColor Color, float Intensity, float Radius)
{
	UPointLightComponent* LightComponent = NewObject<UPointLightComponent>(
		this,
		*FString::Printf(TEXT("Generated_%03d_%s"), GeneratedComponentIndex++, *Label));

	LightComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	LightComponent->SetMobility(EComponentMobility::Static);
	LightComponent->SetRelativeLocation(Location);
	LightComponent->SetLightColor(Color);
	LightComponent->SetIntensity(Intensity);
	LightComponent->SetAttenuationRadius(Radius);
	LightComponent->bUseInverseSquaredFalloff = true;

	LightComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AddInstanceComponent(LightComponent);
	LightComponent->RegisterComponent();
}
