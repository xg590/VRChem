// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "beakerActor.generated.h"

UCLASS()
class VRCHEM_API AbeakerActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* beaker;
	
public:	
	// Sets default values for this actor's properties
	AbeakerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
