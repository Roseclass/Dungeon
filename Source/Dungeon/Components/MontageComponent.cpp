#include "Components/MontageComponent.h"
#include "Global.h"

#include "Characters/DungeonCharacterBase.h"

UMontageComponent::UMontageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMontageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMontageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMontageComponent::PlayKnockDownMontage(FVector NewForce)
{
	Force = NewForce;

	ADungeonCharacterBase* owner = Cast<ADungeonCharacterBase>(GetOwner());
	CheckNull(owner);
	owner->PlayAnimMontage(KnockDownMontage);
}


//
// damagetype
// radial, normal
// normal, hit, knockback, knockdown
// 
// solve add force impulse network issue
//
