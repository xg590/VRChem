// Fill out your copyright notice in the Description page of Project Settings.


#include "beakerActor.h"
//#include "Containers/UnrealString.h"

// Sets default values
AbeakerActor::AbeakerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> beakerMesh(TEXT("/Game/obj/beaker/SM_Beaker"));
	beaker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("beaker")); 
	beaker->SetupAttachment(Root);
	beaker->SetStaticMesh(beakerMesh.Object);  

}

// Called when the game starts or when spawned
void AbeakerActor::BeginPlay()
{
	Super::BeginPlay();  

	FVector loc = this->GetActorLocation(); 
	FString locAndRot = FString::Printf(TEXT("beakerLoc: %f,%f,%f "), loc.X, loc.Y, loc.Z); 
	UE_LOG(LogTemp, Warning, TEXT("%s"), *locAndRot);
	
}

// Called every frame
void AbeakerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

