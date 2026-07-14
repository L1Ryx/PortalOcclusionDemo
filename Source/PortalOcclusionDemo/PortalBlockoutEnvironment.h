#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalBlockoutEnvironment.generated.h"

class UMaterialInterface;
class UStaticMesh;

UCLASS()
class PORTALOCCLUSIONDEMO_API APortalBlockoutEnvironment : public AActor
{
	GENERATED_BODY()

public:
	APortalBlockoutEnvironment();

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, Category = "Blockout")
	bool bBuildCeilingGuides = false;

	UPROPERTY(EditAnywhere, Category = "Blockout")
	bool bBuildLabels = false;

	UPROPERTY(EditAnywhere, Category = "Blockout", meta = (ClampMin = "200.0"))
	float WallHeight = 260.0f;

	UPROPERTY(EditAnywhere, Category = "Blockout", meta = (ClampMin = "10.0"))
	float WallThickness = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Blockout", meta = (ClampMin = "0.1"))
	float EnvironmentScale = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> PlaneMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> GridMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> DarkGridMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> AccentMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> SecondaryGridMaterial;

	int32 GeneratedComponentIndex = 0;

	void ClearGeneratedComponents();
	void BuildEnvironment();

	void AddBox(const FString& Label, const FVector& Location, const FVector& Size, UMaterialInterface* Material, bool bCollision = true);
	void AddFloor(const FString& Label, const FVector2D& Center, const FVector2D& Size, UMaterialInterface* Material);
	void AddWallX(const FString& Label, float Y, float XMin, float XMax);
	void AddWallY(const FString& Label, float X, float YMin, float YMax);
	void AddPortalOnXWall(const FString& Label, float X, float YCenter, float Width);
	void AddPortalOnYWall(const FString& Label, float Y, float XCenter, float Width);
	void AddMarker(const FString& Label, const FVector& Location, const FVector& Size);
	void AddLabel(const FString& Text, const FVector& Location, float YawDegrees = 0.0f, float Size = 70.0f);
	void AddPointLight(const FString& Label, const FVector& Location, FLinearColor Color, float Intensity, float Radius);
};
