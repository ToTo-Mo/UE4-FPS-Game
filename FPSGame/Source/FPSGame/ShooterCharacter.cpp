// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArmComponent sub object
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;	// The camera follows as this distance behind the character
	SpringArm->bUsePawnControlRotation = true;	// Rotate and arm based on the controller


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach camera to end of spring arm
	FollowCamera->bUsePawnControlRotation = false;	// Camera dose not rotate relative to arm
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveVertical", this, &AShooterCharacter::MoveVertical);
	PlayerInputComponent->BindAxis("MoveHorizontal", this, &AShooterCharacter::MoveHorizontal);
}

void AShooterCharacter::MoveVertical(float Value)
{
	if (nullptr == Controller)
	{
		return;
	}

	//const FRotator Rotation{ Controller->GetControlRotation() };
	//const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

	//const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };

	AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::X), Value);
}

void AShooterCharacter::MoveHorizontal(float Value)
{
	AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y), Value);
}
