// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

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
	SpringArm->SocketOffset = FVector(0.0f, 50.f, 50.f);

	// Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach camera to end of spring arm
	FollowCamera->bUsePawnControlRotation = false;	// Camera dose not rotate relative to arm
    
	// Don't rotate when the controller rotates. Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input
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
	// 1. play fire sound
	if (nullptr == FireSound) return;
	UGameplayStatics::PlaySound2D(this, FireSound);

	// 2. find muzzle socket form skeleton
	const USkeletalMeshSocket* MuzzleSocket = GetMesh()->GetSocketByName("MuzzleSocket");
	if(nullptr == MuzzleSocket) return;

	// 3. find transform of muzzle socket
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetMesh());
	if(nullptr == MuzzleFlash) return;

	// 4. create muzzle effect
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);

	// 5. find bullet trail end position
	FVector BulletTrailEndLocation;
	bool bBulletTrailEnd = GetBulletTrailEndLocation(SocketTransform.GetLocation(), BulletTrailEndLocation);

	// 6. 
	if (bBulletTrailEnd)
	{
		// 8-7. spawn impact particles after updating bullet trail end point
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				BulletTrailEndLocation
			);
		}

		if (BeamParticles)
		{
			UParticleSystemComponent* BulletTrail = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform
			);

			if (BulletTrail)
			{
				BulletTrail->SetVectorParameter(FName("Target"), BulletTrailEndLocation);
			}
		}
	}
	
	// 7. play weapon fire montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (nullptr == AnimInstance && nullptr == HipFireMontage) return;

	AnimInstance->Montage_Play(HipFireMontage);
	AnimInstance->Montage_JumpToSection(FName("StartFire"));
}

bool AShooterCharacter::GetBulletTrailEndLocation(const FVector MuzzleSocketLocation, FVector& OutBulletTrailLocation)
{
	// 5. Get current size of viewport
	FVector2D ViewporSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewporSize);
	}

	// 6. Get screen space location of cross hairs
	FVector2D CrosshairLocation(ViewporSize.X / 2.f, ViewporSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 7. get world position and direction of cross hairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	// 8. was deprojection successful?
	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;

		const FVector ScreenTraceStart{ CrosshairWorldPosition };
		const FVector ScreenTraceEnd{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		// 8-1. Set beam end point to line trace end point
		OutBulletTrailLocation = ScreenTraceEnd;

		// 8-2. Trace outward from cross hairs world location
		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			ScreenTraceStart,
			ScreenTraceEnd,
			ECollisionChannel::ECC_Visibility
		);

		// 8-3. was there a trace hit?
		if (ScreenTraceHit.bBlockingHit)
		{
			// 8-4. bullet trail end point in now trace hit location
			OutBulletTrailLocation = ScreenTraceHit.Location;
		}

		// 8-5. trace hit from muzzle
		FHitResult MuzzleTraceHit;
		const FVector MuzzleTraceStart{ MuzzleSocketLocation };
		const FVector MuzzleTraceEnd{ OutBulletTrailLocation };

		GetWorld()->LineTraceSingleByChannel(
			MuzzleTraceHit,
			MuzzleTraceStart,
			MuzzleTraceEnd,
			ECollisionChannel::ECC_Visibility
		);

		// 8-6. object between barrel and BulletTrailEndPoint
		if (MuzzleTraceHit.bBlockingHit)
		{
			OutBulletTrailLocation = MuzzleTraceHit.Location;
		}

		return true;
	}

	return false;
}

