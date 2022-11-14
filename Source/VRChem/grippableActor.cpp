// Fill out your copyright notice in the Description page of Project Settings.

#include "grippableActor.h"

// Sets default values
AgrippableActor::AgrippableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("bottleActor"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> objMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> objMesh(TEXT("/Game/obj/bottle/SM_bottle"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetStaticMesh(objMesh.Object);
	SetRootComponent(StaticMesh);
} 
// Called when the game starts or when spawned
void AgrippableActor::BeginPlay()
{
	Super::BeginPlay();
	FVector loc = this->GetActorLocation();
	FString locAndRot = FString::Printf(TEXT("bottleLoc: %f,%f,%f "), loc.X, loc.Y, loc.Z);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *locAndRot);

}

// Called every frame
void AgrippableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
