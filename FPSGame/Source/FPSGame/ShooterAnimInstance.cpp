// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if(nullptr == ShooterCharacter)
    {
        ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
    } 

    if(ShooterCharacter)
    {
        // Get the character speed x, y axis from velocity
        FVector Velocity { ShooterCharacter->GetVelocity()};
        Velocity.Z = 0;

        Speed = Velocity.Size();

        // Is the character is the air?
        bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

        // Is the character Accelerating?
        bIsAccelerating = ShooterCharacter ->GetCharacterMovement() ->GetCurrentAcceleration().Size() > 0.0f;

        FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

        MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

        if (bIsAccelerating)
        {
            LastMovementOffsetYaw = MovementOffsetYaw;
        }

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::White, *FString::Printf(TEXT("Base Aim Rotation %f"), AimRotation.Yaw));
			GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::White, *FString::Printf(TEXT("Movement Rotation %f"), MovementRotation.Yaw));
			GEngine->AddOnScreenDebugMessage(3, 0.0f, FColor::White, *FString::Printf(TEXT("Movement offset Rotation %f"), MovementOffsetYaw));
		}
    }
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
    ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}