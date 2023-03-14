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

	camChaserComp = CreateDefaultSubobject<USceneComponent>(TEXT("camChaserComponent"));
	camChaserComp->SetupAttachment(SceneComponent);
	camChaserComp->SetRelativeLocation(defaultCamPos);
	camChaserComp->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));



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


	speedCamOffset = FVector(0.0f, 0.0f, 0.0f);

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

	//camMoved = false;

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




	handleCameraLerp(DeltaTime);


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
			"RMBdown", // The input identifier (specified in DefaultInput.ini)
			this, // The object instance that is going to react to the input
			&APlayerShip::RMBrot // The function that will fire when input is received
		);

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
		PlayerInputComponent->BindAxis
		(
			"mouseWheel", // The input identifier (specified in DefaultInput.ini)
			this, // The object instance that is going to react to the input
			&APlayerShip::MouseWheelFunc // The function that will fire when input is received
		);

		
		EnableInput(GetWorld()->GetFirstPlayerController());
	}

	
}



void APlayerShip::MoveShipForward(float AxisValue) {

	if (!warping) {

		if(AxisValue){
			AddActorWorldOffset(GetActorForwardVector() * shipMoveSpeed * GetWorld()->DeltaTimeSeconds * AxisValue);
			if (speedCamOffset.X <  CamOffsetFromMiddle && speedCamOffset.X > - CamOffsetFromMiddle) {
				speedCamOffset.X += GetWorld()->DeltaTimeSeconds * camMoveSpeed *3.0f * -AxisValue;
			}



			
		}
		else if(abs(speedCamOffset.X) > 3) {
			float dir = speedCamOffset.X / abs(speedCamOffset.X);

			speedCamOffset.X += -dir * camMoveSpeed * 2.0f * GetWorld()->DeltaTimeSeconds;

			if (abs(speedCamOffset.X) < 3) {

				speedCamOffset.X = 0.0f;
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

		if (speedCamOffset.Y <  CamOffsetFromMiddle && speedCamOffset.Y > - CamOffsetFromMiddle) {
			speedCamOffset.Y += GetWorld()->DeltaTimeSeconds * camMoveSpeed * AxisValue * 2.0f;
		}
	}
	else if (abs(speedCamOffset.Y) > 3) {
		float dir = speedCamOffset.Y / abs(speedCamOffset.Y);

		speedCamOffset.Y += -dir * camMoveSpeed * 2.0f * GetWorld()->DeltaTimeSeconds;

		if (abs(speedCamOffset.Y) < 3) {

			speedCamOffset.Y = 0.0f;
		}
	}

}


void APlayerShip::RotateShipUp(float AxisValue) {
	



	
	if (AxisValue) {
		//FVector defaultCamPos = FVector(-1250.0f, 0.0f, 500.0f);
		AddActorLocalRotation(FRotator(-AxisValue * shipRotateSpeed * GetWorld()->DeltaTimeSeconds, 0, 0));
		if (speedCamOffset.Z <  CamOffsetFromMiddle && speedCamOffset.Z >  - CamOffsetFromMiddle) {
			speedCamOffset.Z += GetWorld()->DeltaTimeSeconds * camMoveSpeed * -AxisValue;
		}
	}
	else if (abs(speedCamOffset.Z) > 3) {
		float dir = speedCamOffset.Z / abs(speedCamOffset.Z);

		speedCamOffset.Z += -dir * camMoveSpeed * 2.0f * GetWorld()->DeltaTimeSeconds;

		if (abs(speedCamOffset.Z) < 3) {

			speedCamOffset.Z = 0.0f;
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

	FVector dirV_ = (SceneComponent->GetComponentLocation() - OurCamera->GetComponentLocation()).GetSafeNormal();
	FRotator finRot;


	//GetWorld()->GetFirstPlayerController()->axis

	if (inputCompRef->GetAxisValue("RMBdown")) {
		float mouseX, mouseY;
		FVector2D VPsize;
		//inputCompRef->position
		GetWorld()->GetFirstPlayerController()->GetMousePosition(mouseX, mouseY);
		GetWorld()->GetGameViewport()->GetViewportSize(VPsize);


		float dtLokSpeed = DeltaTime * 10.0f;



		float mouseYmove = -dtLokSpeed * (int(mouseY) - int(VPsize.Y / 2));
		float mouseXmove = dtLokSpeed * (int(mouseX) - int(VPsize.X / 2));



		FVector2D upRot(-dirV_.X, dirV_.Y);

		FVector2D MouseXY_(mouseXmove,mouseYmove);


		upRot.Normalize();

		if (nowCamPos.Rotation().Pitch > 87 && mouseYmove < 0 || nowCamPos.Rotation().Pitch < -87 && mouseYmove > 0) {
			mouseYmove = 0;
		}

		nowCamPos = nowCamPos.RotateAngleAxis(mouseXmove, FVector(0,0, 1));
		nowCamPos = nowCamPos.RotateAngleAxis(mouseYmove, FVector(upRot.Y, upRot.X, 0));

		
		
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(float(VPsize.X) / 2, float(VPsize.Y) / 2);


	

		movingBacktoDefaultCamPos = false;

		finRot = ( -1.0f*(nowCamPos + speedCamOffset)).GetSafeNormal().Rotation();

		OurCamera->SetRelativeLocation(nowCamPos + speedCamOffset);
		OurCamera->SetWorldRotation(finRot);
	}
	else {
		if (inputCompRef->GetAxisValue("MoveForward")) {

			if (!movingBacktoDefaultCamPos) {
				if ((nowCamPos - defaultCamPos).Length() > 2.0f || !OurCamera->GetComponentRotation().Equals(camChaserComp->GetComponentRotation(),2.0f)) {
					movingBacktoDefaultCamPos = true;
					lerpVal = 0.0f;

					lerpPos_ = nowCamPos;
					lerpRot = OurCamera->GetComponentRotation();
				}
				



			}
			else {


				lerpVal += DeltaTime*2.0f;
				
				if (lerpVal >= 1.0f) {
					lerpVal = 1.0f;
					movingBacktoDefaultCamPos = false;
				}




				nowCamPos.X = FMath::Lerp(lerpPos_.X,defaultCamPos.X, lerpVal);
				nowCamPos.Y = FMath::Lerp(lerpPos_.Y,defaultCamPos.Y, lerpVal);
				nowCamPos.Z = FMath::Lerp(lerpPos_.Z,defaultCamPos.Z, lerpVal);

				OurCamera->SetRelativeLocation(nowCamPos + speedCamOffset);

				FRotator myRot_;
				FRotator rotDesired = camChaserComp->GetComponentRotation();
				myRot_.Yaw = FMath::Lerp(lerpRot.Yaw, rotDesired.Yaw, lerpVal);
				myRot_.Pitch = FMath::Lerp(lerpRot.Pitch, rotDesired.Pitch, lerpVal);
				myRot_.Roll = FMath::Lerp(lerpRot.Roll, rotDesired.Roll, lerpVal);

				OurCamera->SetWorldRotation(myRot_);



			}




		}
	}


	
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, nowCamPos.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, nowCamPos.Rotation().ToString());
	
	
}


void APlayerShip::RMBrot(float AxisValue) {



}

void APlayerShip::MouseWheelFunc(float AxisValue) {


	//GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Blue, std::to_string(AxisValue).c_str());

	if (AxisValue > 0 && defaultCamPos.Length() < 3000) {
		defaultCamPos += defaultCamPos.GetSafeNormal() * 100.0f;
		nowCamPos += nowCamPos.GetSafeNormal() * 100.0f;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "AAAAAAAAAAA");
	}
	else if (AxisValue < 0 && defaultCamPos.Length() > 500) {
		defaultCamPos -= defaultCamPos.GetSafeNormal() * 100.0f;
		nowCamPos -= nowCamPos.GetSafeNormal() * 100.0f;

	}

	OurCamera->SetRelativeLocation(nowCamPos + speedCamOffset);

}