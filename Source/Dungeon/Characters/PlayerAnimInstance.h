#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 플레이어 캐릭터를 위한 애님인스턴스
 */

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class DUNGEON_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		ACharacter* OnwerCharacter;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		UCharacterMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FVector Velocity;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		bool ShouldMove;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		bool IsFalling;
public:

	//function
private:
protected:
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
