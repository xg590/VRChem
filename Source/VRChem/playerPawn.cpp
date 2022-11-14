// Fill out your copyright notice in the Description page of Project Settings.


#include "playerPawn.h"
#include "HeadMountedDisplayFunctionLibrary.h" 
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h" 
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h" 

#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "SocketTypes.h"

#include "HAL/UnrealMemory.h"
#include "Containers/UnrealString.h"  

#include "grippableActor.h"
#include "vrMotionController.h"
//#include <winsock2.h>

// Sets default values
AplayerPawn::AplayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void AplayerPawn::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);

	LController = GetWorld()->SpawnActor<AvrMotionController>();
	LController->SetOwner(this);
	LController->AttachToComponent(Root, rules);
	LController->hand = EControllerHand::Left;


	RController = GetWorld()->SpawnActor<AvrMotionController>();
	RController->SetOwner(this);
	RController->AttachToComponent(Root, rules);
	RController->hand = EControllerHand::Right;


	if (1) { // Networking 
		FIPv4Address IPAddress;
		FIPv4Address::Parse(FString("192.168.1.14"), IPAddress);
		TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		addr->SetIp(IPAddress.Value);
		addr->SetPort(11220);

		sock = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("xg590"), false);

		bool connected = sock->Connect(*addr);
	} 
	
	//GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("Output: %s"), *this->GetActorLocation().ToString()), true, FVector2D(2.f,2.f));

}

void AplayerPawn::EndPlay(EEndPlayReason::Type Reason)
{ 
	sock->Shutdown(ESocketShutdownMode::ReadWrite);
	sock->Close();
}

// Called every frame
void AplayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AplayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("TeleportLeft", IE_Pressed, this, &AplayerPawn::TeleportLeft);
	PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &AplayerPawn::TeleportLeftRelease);
	PlayerInputComponent->BindAction("TeleportRight", IE_Pressed, this, &AplayerPawn::TeleportRight);
	PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &AplayerPawn::TeleportRightRelease);
	PlayerInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AplayerPawn::GrabLeft);
	PlayerInputComponent->BindAction("GrabLeft", IE_Released, this, &AplayerPawn::ReleaseLeft);
	PlayerInputComponent->BindAction("GrabRight", IE_Pressed, this, &AplayerPawn::GrabRight);
	PlayerInputComponent->BindAction("GrabRight", IE_Released, this, &AplayerPawn::ReleaseRight);
	PlayerInputComponent->BindAction("Homing", IE_Pressed, this, &AplayerPawn::Homing); 

}


void AplayerPawn::TeleportLeft() {
	LController->TeleportActive = true;

};


void AplayerPawn::TeleportLeftRelease() {
	LController->TeleportActive = false;

};;

void AplayerPawn::TeleportRight() {
	RController->TeleportActive = true;
};;

void AplayerPawn::TeleportRightRelease() {
	RController->TeleportActive = false;

};;

void AplayerPawn::GrabLeft() {
	//UE_LOG(LogTemp, Warning, TEXT("Grab"));
	LController->Grab();
};;

void AplayerPawn::GrabRight() {
	//UE_LOG(LogTemp, Warning, TEXT("Grab"));
	RController->Grab();
};;

void AplayerPawn::ReleaseLeft() {
	//UE_LOG(LogTemp, Warning, TEXT("Release"));

	FString locAndRot = LController->Release();
	if (!locAndRot.IsEmpty()) {
		this->ReportActor(locAndRot);
	}
	UE_LOG(LogTemp, Warning, TEXT("lAplayerPawn %s"), *locAndRot);

};;

void AplayerPawn::ReleaseRight() {
	//UE_LOG(LogTemp, Warning, TEXT("Release"));
	FString locAndRot = RController->Release();
	if (!locAndRot.IsEmpty()) {
		this->ReportActor(locAndRot);
	}
	UE_LOG(LogTemp, Warning, TEXT("rAplayerPawn %s"), *locAndRot);
};;
 
void AplayerPawn::ReportActor(FString msg)
{
	TCHAR* msgInChar = msg.GetCharArray().GetData();
	uint8 msgSize = FCString::Strlen(msgInChar);
	int32 sent = 0;

	bool successful = sock->Send((uint8*)TCHAR_TO_ANSI(msgInChar), msgSize, sent);
	UE_LOG(LogTemp, Warning, TEXT("sent: %d"), sent);

	// TArray<uint8> msgInUint8Arr;
	// //msgInUint8Arr.Init(88, 45);
	// msgInUint8Arr.AddUninitialized(msgSize);
	// FMemory::Memcpy(msgInUint8Arr.GetData(), (uint8*)TCHAR_TO_ANSI(msgInChar), msgSize * sizeof(uint8)); 
	// msgInUint8Arr.Add(10); 

}


void AplayerPawn::Homing() {
	FString msg = "{\"action\":\"homing\"}\n";
	TCHAR* msgInChar = msg.GetCharArray().GetData();
	uint8 msgSize = FCString::Strlen(msgInChar);
	int32 sent = 0;

	bool successful = sock->Send((uint8*)TCHAR_TO_ANSI(msgInChar), msgSize, sent);
	UE_LOG(LogTemp, Warning, TEXT("sent: %d"), sent);

	 
	TArray<AActor*> ActorsToFind;

	//Gets all Actors of FireEffect class that have the "FindActorTag"

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AgrippableActor::StaticClass(), FName("FindActorTag"), ActorsToFind);

	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AgrippableActor::StaticClass(), ActorsToFind);
	}
	for (AActor* Actor : ActorsToFind) 
	{ 
		AgrippableActor* actor = Cast<AgrippableActor>(Actor); 
		if (actor)
		{
			actor->SetActorLocationAndRotation(FVector(0.f,0.f,130.f),FRotator(0.f,0.f,0.f),false,0,ETeleportType::ResetPhysics); 
		} 
	}
	  
	 


};