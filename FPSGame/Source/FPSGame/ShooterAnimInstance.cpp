// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
    }
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
    ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}