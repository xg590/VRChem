#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Components/SplineMeshComponent.h"
#include "vrMotionController.generated.h"

UCLASS()
class VRCHEM_API AvrMotionController : public AActor
{
	GENERATED_BODY()

		UPROPERTY(VisibleAnywhere)
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
		class UMotionControllerComponent* MotionController;

	UPROPERTY(VisibleAnywhere)
		class USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere)
		class USplineComponent* Arc;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* CollisionSphere;



public:
	// Sets default values for this actor's properties
	AvrMotionController();

	EControllerHand hand;

	bool TeleportActive;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Arc vars
	FPredictProjectilePathResult Result;
	bool HitResult;
	UStaticMesh* beamMesh;
	TArray<USplineMeshComponent*> SplineMeshes;
	TArray<FVector> SplinePoints;
	float ArcVelocity = 900.0f;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DrawArc();
	void ClearArc();
	void TeleportToLocation();

	void Grab();
	FString Release();

};
