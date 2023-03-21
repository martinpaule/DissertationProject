// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraHandlingComponent.h"

#include "PlayerShip.h"

// Sets default values for this component's properties
UCameraHandlingComponent::UCameraHandlingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	defaultCamOffset = FVector(-1250.0f, 0.0f, 500.0f).GetSafeNormal();
	nowCamOffset = defaultCamOffset;
	camDistanceFromRoot = 1500.0f;
	speedCamOffset = FVector(0.0f, 0.0f, 0.0f);
}


// Called when the game starts
void UCameraHandlingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCameraHandlingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	handleCameraLerp(DeltaTime);
}




void UCameraHandlingComponent::handleCameraLerp(float DeltaTime) {

	FVector dirV_ = (shipRef->SceneComponent->GetComponentLocation() - shipRef->OurCamera->GetComponentLocation()).GetSafeNormal();
	FRotator finRot;


	if (shipRef->inputCompRef->GetAxisValue("RMBdown")) {





		float mouseX, mouseY;
		FVector2D VPsize;

		GetWorld()->GetFirstPlayerController()->GetMousePosition(mouseX, mouseY);
		GetWorld()->GetGameViewport()->GetViewportSize(VPsize);

		if (!oneHoldFramePassed) {
			oneHoldFramePassed = true;
			GetWorld()->GetFirstPlayerController()->SetMouseLocation(float(VPsize.X) / 2, float(VPsize.Y) / 2);
			return;
		}

		float dtLokSpeed = DeltaTime * 10.0f;



		float mouseYmove = -dtLokSpeed * (int(mouseY) - int(VPsize.Y / 2));
		float mouseXmove = dtLokSpeed * (int(mouseX) - int(VPsize.X / 2));



		FVector2D upRot(-dirV_.X, dirV_.Y);

		FVector2D MouseXY_(mouseXmove, mouseYmove);


		upRot.Normalize();

		nowCamOffset = nowCamOffset.RotateAngleAxis(mouseXmove, FVector(0, 0, 1));

		if (!(nowCamOffset.Rotation().Pitch > 87 && mouseYmove < 0) && !(nowCamOffset.Rotation().Pitch < -87 && mouseYmove > 0)) {
			nowCamOffset = nowCamOffset.RotateAngleAxis(mouseYmove, FVector(upRot.Y, upRot.X, 0));
		}


		GetWorld()->GetFirstPlayerController()->SetMouseLocation(float(VPsize.X) / 2, float(VPsize.Y) / 2);




		movingBacktoDefaultCamPos = false;

		finRot = dirV_.Rotation();

		//OurCamera->SetRelativeLocation(nowCamOffset * camDistanceFromRoot + speedCamOffset);
		shipRef->OurCamera->SetRelativeRotation((-nowCamOffset).Rotation());

	}
	else {

		oneHoldFramePassed = false;

		if (shipRef->inputCompRef->GetAxisValue("MoveForward")) {

			if (!movingBacktoDefaultCamPos) {
				if ((nowCamOffset - defaultCamOffset).Length() > 2.0f || !shipRef->OurCamera->GetComponentRotation().Equals(shipRef->camChaserComp->GetComponentRotation(), 2.0f)) {
					movingBacktoDefaultCamPos = true;
					lerpVal = 0.0f;

					lerpPos_ = nowCamOffset;
					lerpRot = shipRef->OurCamera->GetComponentRotation();
				}




			}
			else {


				lerpVal += DeltaTime * 2.0f;

				if (lerpVal >= 1.0f) {
					lerpVal = 1.0f;
					movingBacktoDefaultCamPos = false;
				}




				nowCamOffset.X = FMath::Lerp(lerpPos_.X, defaultCamOffset.X, lerpVal);
				nowCamOffset.Y = FMath::Lerp(lerpPos_.Y, defaultCamOffset.Y, lerpVal);
				nowCamOffset.Z = FMath::Lerp(lerpPos_.Z, defaultCamOffset.Z, lerpVal);

				//OurCamera->SetRelativeLocation(nowCamOffset * camDistanceFromRoot + speedCamOffset);

				FRotator myRot_;
				FRotator rotDesired = shipRef->camChaserComp->GetComponentRotation();
				myRot_.Yaw = FMath::Lerp(lerpRot.Yaw, rotDesired.Yaw, lerpVal);
				myRot_.Pitch = FMath::Lerp(lerpRot.Pitch, rotDesired.Pitch, lerpVal);
				myRot_.Roll = FMath::Lerp(lerpRot.Roll, rotDesired.Roll, lerpVal);

				shipRef->OurCamera->SetWorldRotation(myRot_);



			}




		}
	}

	shipRef->OurCamera->SetRelativeLocation(nowCamOffset * camDistanceFromRoot + speedCamOffset);


	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, nowCamOffset.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, nowCamOffset.Rotation().ToString());


}



void UCameraHandlingComponent::MoveShipForward_CamHandle(float AxisValue) {


	if (AxisValue) {
		if (speedCamOffset.X <  CamOffsetFromMiddle && speedCamOffset.X > -CamOffsetFromMiddle) {
			speedCamOffset.X += GetWorld()->DeltaTimeSeconds * camMoveSpeed * 3.0f * -AxisValue;
		}




	}
	else if (abs(speedCamOffset.X) > 3) {
		float dir = speedCamOffset.X / abs(speedCamOffset.X);

		speedCamOffset.X += -dir * camMoveSpeed * 2.0f * GetWorld()->DeltaTimeSeconds;

		if (abs(speedCamOffset.X) < 3) {

			speedCamOffset.X = 0.0f;
		}
	}



}

void UCameraHandlingComponent::MoveShipRight_CamHandle(float AxisValue) {


	if (AxisValue) {

		if (speedCamOffset.Y <  CamOffsetFromMiddle && speedCamOffset.Y > -CamOffsetFromMiddle) {
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


void UCameraHandlingComponent::RotateShipUp_CamHandle(float AxisValue) {





	if (AxisValue) {
		if (speedCamOffset.Z <  CamOffsetFromMiddle && speedCamOffset.Z >  -CamOffsetFromMiddle) {
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

