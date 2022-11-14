// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "playerPawn.generated.h"

UCLASS()
class VRCHEM_API AplayerPawn : public APawn
{
	GENERATED_BODY()
		UPROPERTY(VisibleAnywhere)
		class UCameraComponent* VRCamera;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
		class AvrMotionController* LController;

	UPROPERTY(VisibleAnywhere)
		class AvrMotionController* RController;


public:
	// Sets default values for this pawn's properties
	AplayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void TeleportLeft();
	void TeleportLeftRelease();
	void TeleportRight();
	void TeleportRightRelease();

	void GrabLeft();
	void ReleaseLeft();
	void GrabRight();
	void ReleaseRight();

	void Homing();
	void ReportActor(FString);
	FSocket* sock;

};
