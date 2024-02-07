#include "Characters/PlayerAnimInstance.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OnwerCharacter = Cast<ACharacter>(GetOwningActor());

	if (OnwerCharacter)
	{
		MovementComponent = OnwerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OnwerCharacter && MovementComponent)
	{
		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Size2D();
		ShouldMove = UKismetMathLibrary::NotEqual_VectorVector(MovementComponent->GetCurrentAcceleration(), FVector(0, 0, 0), 0) && (GroundSpeed > 3);
		IsFalling = MovementComponent->IsFalling();
	}
}
