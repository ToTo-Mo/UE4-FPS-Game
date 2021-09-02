// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class FPSGAME_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	// --- Member Variable ----
private:
	// spring arm
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	// base turn rate : deg/sec
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float BaseHorizontalRate;

	// base look up down rate : deg/sec
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float BaseVerticalRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	class USoundCue* FireSound;

	/** Flash spawns at the muzzle*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = true))
	class UParticleSystem* MuzzleFlash;
	
    
public:

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// --- Member Function ----
private:

	// ---- Character Movement ----
	void MoveVertical(float Value);
	void MoveHorizontal(float Value);

	// ---- Character View Movement ----

	/**
	 * Called via input to turn at a given rate
	 * @Param Rate This is a normalized rate, i.e 1.0 means 100% of desired turn rate
	*/
	void TurnHorizontalRate(float Rate);

	/**
	 * Called via input to look up & down at a given rate
	 * @Param Rate This is a normalized rate, i.e 1.0 means 100% of desired turn rate
	*/
	void TurnVerticalRate(float Rate);

	void FireWeapon();

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
