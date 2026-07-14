#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalDemoMovingDoor.generated.h"

class UMaterialInterface;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS()
class PORTALOCCLUSIONDEMO_API APortalDemoMovingDoor : public AActor
{
	GENERATED_BODY()

public:
	APortalDemoMovingDoor();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "Portal Door")
	float GetOpenFraction() const { return OpenFraction; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door")
	bool bAutoAnimate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PreviewOpenFraction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door")
	FVector DoorSize = FVector(560.0f, 40.0f, 280.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door")
	FVector ClosedPanelOffset = FVector(-280.0f, 0.0f, -140.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door")
	FVector DoorTravelOffset = FVector(620.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door", meta = (ClampMin = "0.05"))
	float TravelSeconds = 1.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door", meta = (ClampMin = "0.0"))
	float ClosedHoldSeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door", meta = (ClampMin = "0.0"))
	float OpenHoldSeconds = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Door", meta = (ClampMin = "0.0"))
	float PhaseOffsetSeconds = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Door")
	float OpenFraction = 0.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorPanel;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> AccentMaterial;

	float RuntimeSeconds = 0.0f;

	float ComputeAnimatedOpenFraction(float TimeSeconds) const;
	void ApplyOpenFraction(float NewOpenFraction);
};
