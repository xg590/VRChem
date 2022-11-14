// Fill out your copyright notice in the Description page of Project Settings.

#include "vrMotionController.h"
#include "MotionControllerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h" 


#include "grippableActor.h"
#include "Containers/UnrealString.h"
#include "XRMotionControllerBase.h"


// Sets default values
AvrMotionController::AvrMotionController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root); 
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMesh(TEXT("/Game/obj/hand/QK_CustomHand")); 
	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh")); 
	HandMesh->SetupAttachment(MotionController);
	HandMesh->SetSkeletalMesh(SkelMesh.Object);
	HandMesh->SetRelativeLocation(FVector(-10.0f, 0.0f, 0.0f));

	Arc = CreateDefaultSubobject<USplineComponent>(TEXT("Arc"));
	Arc->SetupAttachment(HandMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Beam(TEXT("/Game/obj/projectile/projectile"));
	beamMesh = Beam.Object; 

	//StaticMesh'/Game/StarterContent/Shapes/teleport_ring.teleport_ring'

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(50.0f);
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionSphere->SetupAttachment(HandMesh);

}

// Called when the game starts or when spawned
void AvrMotionController::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void AvrMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	if (hand == EControllerHand::Right) {
		MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
		MotionController->AttachToComponent(this->Root, rules);

		HandMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
		hand = EControllerHand::AnyHand;

	}
	else if (hand == EControllerHand::Left) {
		MotionController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
		MotionController->AttachToComponent(this->Root, rules);

		HandMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
		HandMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, -1.0f));
		hand = EControllerHand::AnyHand;

	}

	if (Arc) {
		ClearArc();
	}
	if (TeleportActive) {
		DrawArc();
	}
	if (!TeleportActive && HitResult) {
		TeleportToLocation();
	}


}


void AvrMotionController::DrawArc()
{
	FVector ArcLocation = HandMesh->GetComponentLocation();

	FPredictProjectilePathParams Params = FPredictProjectilePathParams(0.0f, ArcLocation, HandMesh->GetForwardVector() * ArcVelocity, 2.0f);
	Params.bTraceWithCollision = true;
	Params.bTraceComplex = false;
	Params.TraceChannel = ECollisionChannel::ECC_WorldStatic;

	HitResult = UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result);

	if (HitResult) {
		for (const FPredictProjectilePathPointData result : Result.PathData) {
			SplinePoints.Add(result.Location);
			Arc->AddSplinePoint(result.Location, ESplineCoordinateSpace::World);
		}
	}

	if (Arc) {
		Arc->SetSplinePointType(Result.PathData.Num() - 1, ESplinePointType::CurveClamped);

		for (int32 i = 0; i < SplinePoints.Num() - 2; i++) {
			USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>();
			SplineMesh->RegisterComponentWithWorld(GetWorld());
			SplineMesh->SetStaticMesh(beamMesh);
			SplineMeshes.Add(SplineMesh);

			FVector Start = SplinePoints[i];
			FVector End = SplinePoints[i + 1];
			FVector sTan = FVector::ZeroVector;
			FVector eTan = FVector::ZeroVector;

			SplineMesh->SetStartAndEnd(Start, sTan, End, eTan);
		}
	}

}

void AvrMotionController::ClearArc()
{
	Arc->ClearSplinePoints();
	SplinePoints.Empty();

	for (int32 i = SplineMeshes.Num() - 1; i >= 0; i--) {
		if (SplineMeshes[i]) {
			SplineMeshes[i]->DestroyComponent();
		}
	}

	SplineMeshes.Empty();
}

void AvrMotionController::TeleportToLocation()
{
	HitResult = false;
	//FString abc = Result.LastTraceDestination.Location.ToString();
	//UE_LOG(LogTemp, Warning, TEXT("Output: %s"), *abc); 
	GetOwner()->TeleportTo(Result.LastTraceDestination.Location, GetOwner()->GetActorRotation(), false, false);
}

void AvrMotionController::Grab()
{

	AActor* cActor = NULL;
	TArray<AActor*> Overlapping;
	CollisionSphere->GetOverlappingActors(Overlapping);

	for (AActor* oActor : Overlapping) {
		if (cActor == NULL && oActor->GetClass()->IsChildOf(AgrippableActor::StaticClass())) {
			cActor = oActor;
		}
		else if (oActor->GetClass()->IsChildOf(AgrippableActor::StaticClass())) {
			float competingDistance = FVector::Distance(oActor->GetActorLocation(), HandMesh->GetComponentLocation());
			float originalDistance = FVector::Distance(cActor->GetActorLocation(), HandMesh->GetComponentLocation());

			if (competingDistance < originalDistance) {
				cActor = oActor;
			}
		}
	}

	if (cActor != NULL) {
		AgrippableActor* gActor = Cast<AgrippableActor>(cActor);

		gActor->StaticMesh->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform);
		gActor->StaticMesh->SetSimulatePhysics(false);
	}
}

FString AvrMotionController::Release() {
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	FString locAndRot;
	for (AActor* aActors : AttachedActors) {
		if (aActors->GetClass()->IsChildOf(AgrippableActor::StaticClass())) {
			AgrippableActor* gActor = Cast<AgrippableActor>(aActors);

			gActor->StaticMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			FVector loc = gActor->GetActorLocation();
			FRotator rot = gActor->GetActorRotation();
			//FString abc = Result.LastTraceDestination.Location.ToString();

			locAndRot = FString::Printf(TEXT("{\"action\":\"move\",\"X\":%f,\"Y\":%f,\"Z\":%f,\"Pitch\":%f,\"Roll\":%f,\"Yaw\":%f}\n"), loc.X, loc.Y, loc.Z, rot.Pitch, rot.Roll, rot.Yaw);
			//FString::GetCharArray();  
			//this->ReportActor(locAndRot);
			//gActor->StaticMesh->SetSimulatePhysics(true);

			//GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, FString::Printf(TEXT("%f,%f,%f"), loc.X, loc.Y, loc.Z), true, FVector2D(2.f, 2.f));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AvrMotionController: %s"), *locAndRot);
	return locAndRot;
}

