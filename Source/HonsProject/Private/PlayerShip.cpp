// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerShip.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include <string>
#include "NiagaraComponent.h"


// Sets default values
APlayerShip::APlayerShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//camChaserComp = CreateDefaultSubobject<USceneComponent>(TEXT("camChaserComponent"));
	//camChaserComp->SetupAttachment(SceneComponent);
	//camChaserComp->SetRelativeLocation(defaultCamPos);
	//camChaserComp->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));



	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshCompopnent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/MyHonsContent/PlayerShip/scene.scene'"));
	UStaticMesh* tempM = MeshAsset.Object;
	StaticMeshComponent->SetStaticMesh(tempM);

	StaticMeshComponent->SetRelativeRotation(FRotator(0,-90,0));

	//camRef
	AutoPossessPlayer = EAutoReceiveInput::Player0;



	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	// Attach our camera and visible object to our root component. Offset and rotate the camera.
	OurCamera->SetupAttachment(RootComponent);
	OurCamera->SetRelativeLocation(defaultCamPos);
	OurCamera->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	nowCamPos = defaultCamPos;


	 

	auto particleSys = ConstructorHelpers::FObjectFinder<UNiagaraSystem>(TEXT("NiagaraSystem'/Game/MyHonsContent/PlanetRelated/P_Trail.p_Trail'"));
	shipTrail1 = particleSys.Object;

	auto particleSysWarp = ConstructorHelpers::FObjectFinder<UNiagaraSystem>(TEXT("NiagaraSystem'/Game/MyHonsContent/PlayerShip/HyperSpaceFX.HyperSpaceFX'"));
	WarpDrive = particleSysWarp.Object;

}




// Called when the game starts or when spawned
void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(shipTrail1, RootComponent, NAME_None, FVector(-400.0f, 230.0f, 30.0f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);
	UNiagaraComponent* NiagaraComp2 = UNiagaraFunctionLibrary::SpawnSystemAttached(shipTrail1, RootComponent, NAME_None, FVector(-400.0f, -230.0f, 30.0f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);

	NiagaraComp->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	NiagaraComp2->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);

	NiagaraComp->SetNiagaraVariableLinearColor("User.MyCol", FLinearColor(0.6f,0.6f,1.0f));
	NiagaraComp2->SetNiagaraVariableLinearColor("User.MyCol", FLinearColor(0.6f, 0.6f, 1.0f));
	
	NiagaraComp2->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	NiagaraComp->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));

	

	//WarpDriveComp->SetComponentTickEnabled(false);
	//WarpDriveComp->Deactivate();
}

// Called every frame
void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, std::to_string(warpStrength).c_str());

	if (warping) {
		warpTimeLeft -= DeltaTime;


		AddActorWorldOffset(GetActorForwardVector() * shipMoveSpeed * GetWorld()->DeltaTimeSeconds * int(warpStrength)*3);



		if (warpTimeLeft < 0.0f) {
			warping = false;
			warpStrength = 0;


			partLife = 1.0f;
			slowingWarp = true;
		}
	}

	if (slowingWarp) {
		partLife -= DeltaTime;

		if (partLife < 0.0f) {
			slowingWarp = false;
			WarpDriveComp->DestroyComponent();
			WarpDriveComp = NULL;
			//WarpDriveComp->Deactivate();
		}

	}


	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "banaenae");

	//inputCompRef->

	if (moveCamBack) {
		handleCameraLerp(DeltaTime);
	}
	
	if ((defaultCamPos - nowCamPos).Length() > 10) {
		OurCamera->SetRelativeLocation(nowCamPos);

		//OurCamera.settarg
	}

}

// Called to bind functionality to input
void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	inputCompRef = PlayerInputComponent;

	if (PlayerInputComponent)
	{
		// Bind an action to it
		PlayerInputComponent->BindAxis
		(
			"MoveForward", // The input identifier (specified in DefaultInput.ini)
			this, // The object instance that is going to react to the input
			&APlayerShip::MoveShipForward // The function that will fire when input is received
		);

		// Bind an action to it
		PlayerInputComponent->BindAxis
		(
			"MoveRight", // The input identifier (specified in DefaultInput.ini)
			this, // The object instance that is going to react to the input
			&APlayerShip::MoveShipRight // The function that will fire when input is received
		);

		PlayerInputComponent->BindAxis
		(
			"RotateUp", // The input identifier (specified in DefaultInput.ini)
			this, // The object instance that is going to react to the input
			&APlayerShip::RotateShipUp // The function that will fire when input is received
		);

		PlayerInputComponent->BindAxis
		(
			"RotateRight", // The input identifier (specified in DefaultInput.ini)
			this, // The object instance that is going to react to the input
			&APlayerShip::RotateShipRight // The function that will fire when input is received
		);

		PlayerInputComponent->BindAxis
		(
			"ChargeWarpSpeed", // The input identifier (specified in DefaultInput.ini)
			this, // The object instance that is going to react to the input
			&APlayerShip::ChargeWarpSpeed // The function that will fire when input is received
		);

		EnableInput(GetWorld()->GetFirstPlayerController());
	}

	
}



void APlayerShip::MoveShipForward(float AxisValue) {

	if (!warping) {

		if(AxisValue){
			AddActorWorldOffset(GetActorForwardVector() * shipMoveSpeed * GetWorld()->DeltaTimeSeconds * AxisValue);
			if (nowCamPos.X < defaultCamPos.X + CamOffsetFromMiddle && nowCamPos.X > defaultCamPos.X - CamOffsetFromMiddle) {
				nowCamPos.X += GetWorld()->DeltaTimeSeconds * camMoveSpeed *3.0f * -AxisValue;
			}
		}

		//launch drive
		if (warpStrength > 1.0f && AxisValue) {

			warping = true;
			warpTimeLeft = 3.0f;

			
			WarpDriveComp = UNiagaraFunctionLibrary::SpawnSystemAttached(WarpDrive, RootComponent, NAME_None, FVector(7777.0f, 0.0f, 0.0f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);

			WarpDriveComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			slowingWarp = false;

		}

	}

}



void APlayerShip::MoveShipRight(float AxisValue) {


	if(AxisValue){
		AddActorLocalRotation(FRotator(0, AxisValue * shipRotateSpeed * GetWorld()->DeltaTimeSeconds, 0));

		if (nowCamPos.Y < defaultCamPos.Y + CamOffsetFromMiddle && nowCamPos.Y > defaultCamPos.Y - CamOffsetFromMiddle) {
			nowCamPos.Y += GetWorld()->DeltaTimeSeconds * camMoveSpeed * AxisValue * 2.0f;
		}
	}
	

}


void APlayerShip::RotateShipUp(float AxisValue) {
	



	
	if (AxisValue) {
		//FVector defaultCamPos = FVector(-1250.0f, 0.0f, 500.0f);
		AddActorLocalRotation(FRotator(-AxisValue * shipRotateSpeed * GetWorld()->DeltaTimeSeconds, 0, 0));
		if (nowCamPos.Z < defaultCamPos.Z + CamOffsetFromMiddle && nowCamPos.Z > defaultCamPos.Z - CamOffsetFromMiddle) {
			nowCamPos.Z += GetWorld()->DeltaTimeSeconds * camMoveSpeed * -AxisValue;
		}
	}
	
	

}



void APlayerShip::RotateShipRight(float AxisValue) {

	


	
	AddActorLocalRotation(FRotator(0, 0, AxisValue * shipRotateSpeed * GetWorld()->DeltaTimeSeconds));



}

void APlayerShip::ChargeWarpSpeed(float AxisValue) {


	if (warping) {
		return;
	}

	if (AxisValue) {
		warpStrength += GetWorld()->DeltaTimeSeconds;
	}
	else {

		

		if (warpStrength > 0.0f) {
			warpStrength -= GetWorld()->DeltaTimeSeconds;
			if (warpStrength < 0.0f) {
				warpStrength = 0.0f;
			}
		}
	}

}

void APlayerShip::handleCameraLerp(float DeltaTime) {

	if (!inputCompRef->GetAxisValue("MoveForward")) {
		float dist = defaultCamPos.X - nowCamPos.X;
		if (abs(dist) > 3) {
			float dir = dist / abs(dist);

			nowCamPos.X += dir * camMoveSpeed * 2.0f * DeltaTime;

			if (abs(defaultCamPos.X - nowCamPos.X) < 3) {

				nowCamPos.X = defaultCamPos.X;
			}
		}
	}

	if (!inputCompRef->GetAxisValue("MoveRight")) {
		float dist = defaultCamPos.Y - nowCamPos.Y;
		if (abs(dist) > 3) {
			float dir = dist / abs(dist);

			nowCamPos.Y += dir * camMoveSpeed * 2.0f * DeltaTime;

			if (abs(defaultCamPos.Y - nowCamPos.Y) < 3) {

				nowCamPos.Y = defaultCamPos.Y;
			}
		}
	}

	if (!inputCompRef->GetAxisValue("RotateUp")) {
		float dist = defaultCamPos.Z - nowCamPos.Z;
		if (abs(dist) > 3) {
			float dir = dist / abs(dist);

			nowCamPos.Z += dir * camMoveSpeed * 2.0f * DeltaTime;

			if (abs(defaultCamPos.Z - nowCamPos.Z) < 3) {

				nowCamPos.Z = defaultCamPos.Z;
			}
		}
	}

	
}