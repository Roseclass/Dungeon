#include "Components/BehaviorComponent.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/DungeonCharacter.h"

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

void UBehaviorComponent::SetHittedMode()
{
	ChangeType(EBehaviorType::Hitted);
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

bool UBehaviorComponent::IsHittedMode()
{
	return GetType() == EBehaviorType::Hitted;
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

ADungeonCharacter* UBehaviorComponent::GetTarget()
{
	return Cast<ADungeonCharacter>(Blackboard->GetValueAsObject(TargetKey));
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