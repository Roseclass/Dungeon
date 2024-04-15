#include "Components/BehaviorComponent.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/PlayerCharacter.h"

UBehaviorComponent::UBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBehaviorComponent::ChangeType(EBehaviorType InNewType)
{
	EBehaviorType type = GetType();
	Blackboard->SetValueAsEnum(BehaviorKey, (uint8)InNewType);

	if (OnBehaviorTypeChanged.IsBound())
		OnBehaviorTypeChanged.Broadcast(type, InNewType);
}

EBehaviorType UBehaviorComponent::GetType()
{
	return (EBehaviorType)Blackboard->GetValueAsEnum(BehaviorKey);
}

void UBehaviorComponent::TaskStart()
{
	TaskStartTime = GetWorld()->GetTimeSeconds();
}

void UBehaviorComponent::SetWaitMode()
{
	ChangeType(EBehaviorType::Wait);
}

void UBehaviorComponent::SetApproachMode()
{
	ChangeType(EBehaviorType::Approach);
}

void UBehaviorComponent::SetPatrolMode()
{
	ChangeType(EBehaviorType::Patrol);
}

void UBehaviorComponent::SetHitMode()
{
	ChangeType(EBehaviorType::Hit);
}

void UBehaviorComponent::SetAvoidMode()
{
	ChangeType(EBehaviorType::Avoid);
}

void UBehaviorComponent::SetSkillMode()
{
	ChangeType(EBehaviorType::Skill);
}

void UBehaviorComponent::SetStrafingMode()
{
	ChangeType(EBehaviorType::Strafing);
}

void UBehaviorComponent::SetSequenceMode()
{
	ChangeType(EBehaviorType::Sequence);
}

bool UBehaviorComponent::IsWaitMode()
{
	return GetType() == EBehaviorType::Wait;
}

bool UBehaviorComponent::IsApproachMode()
{
	return GetType() == EBehaviorType::Approach;
}

bool UBehaviorComponent::IsPatrolMode()
{
	return GetType() == EBehaviorType::Patrol;
}

bool UBehaviorComponent::IsHitMode()
{
	return GetType() == EBehaviorType::Hit;
}

bool UBehaviorComponent::IsAvoidMode()
{
	return GetType() == EBehaviorType::Avoid;
}

bool UBehaviorComponent::IsSkillMode()
{
	return GetType() == EBehaviorType::Skill;
}

bool UBehaviorComponent::IsStrafingMode()
{
	return GetType() == EBehaviorType::Strafing;
}

bool UBehaviorComponent::IsSequenceMode()
{
	return GetType() == EBehaviorType::Sequence;
}

APlayerCharacter* UBehaviorComponent::GetTarget()
{
	return Cast<APlayerCharacter>(Blackboard->GetValueAsObject(TargetKey));
}

FVector UBehaviorComponent::GetGoalLocation()
{
	return Blackboard->GetValueAsVector(GoalLocationKey);
}

float UBehaviorComponent::GetSkillRange(int32 InIndex)
{
	// get skill component

	return 0;
}