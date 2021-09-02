// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	BaseHorizontalRate(45.0f),
	BaseVerticalRate(45.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArmComponent sub object
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;	// The camera follows as this distance behind the character
	SpringArm->bUsePawnControlRotation = true;	// Rotate and arm based on the controller

	// Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach camera to end of spring arm
	FollowCamera->bUsePawnControlRotation = false;	// Camera dose not rotate relative to arm
    
	// Don't rotate when the controller rotates. Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
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

	PlayerInputComponent->BindAxis("TurnHorizontalRate", this, &AShooterCharacter::TurnHorizontalRate);
	PlayerInputComponent->BindAxis("TurnVerticalRate", this, &AShooterCharacter::TurnVerticalRate);
	PlayerInputComponent->BindAxis("TurnHorizontal", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnVertical", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::FireWeapon);
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


void AShooterCharacter::TurnHorizontalRate(float Rate)
{
	// calculate delta for this frame from the rate information
	// deg/sec * sec/frame = deg/frame
	AddControllerYawInput(Rate * BaseHorizontalRate * GetWorld()->GetDeltaSeconds());
}
void AShooterCharacter::TurnVerticalRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseVerticalRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire"));

	if (nullptr == FireSound)
	{
		return;
	}

	UGameplayStatics::PlaySound2D(this, FireSound);

	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("MuzzleSocket");

	if(nullptr == MuzzleSocket)
	{
		return;
	}

	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());

	if(nullptr == MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
	}
}

