// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Movement, meta= (AllowPrivateAccess = true))
	class AShooterCharacter* ShooterCharacter;

	/** character speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Movement, meta= (AllowPrivateAccess = true))
	float Speed;

	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Movement, meta= (AllowPrivateAccess = true))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Movement, meta= (AllowPrivateAccess = true))
	bool bIsAccelerating;
};
