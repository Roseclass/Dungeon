#include "Components/StateComponent.h"
#include "Global.h"

UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(1);
}

void UStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//switch (Type)
	//{
	//case EStateType::Idle:CLog::Print("Idle", 213, 0); break;
	//case EStateType::Skill:CLog::Print("Skill", 213, 0); break;
	//case EStateType::Hit:CLog::Print("Hit", 213, 0); break;
	//case EStateType::KnockBack:CLog::Print("KnockBack", 213, 0); break;
	//case EStateType::KnockDown:CLog::Print("KnockDown", 213, 0); break;
	//case EStateType::Sequence:CLog::Print("Sequence", 213, 0); break;
	//case EStateType::Max:CLog::Print("Max", 213, 0); break;
	//default:break;
	//}
}

void UStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(UStateComponent, Type, COND_None);
}

void UStateComponent::OnRep_Type()
{
	if (OnStateTypeChanged.IsBound())
		OnStateTypeChanged.Broadcast(PrevType, Type);
}

void UStateComponent::ChangeType(EStateType InNewType)
{
	PrevType = Type;
	Type = InNewType;

	if (GetOwner()->HasAuthority())
		OnRep_Type();
}

void UStateComponent::SetIdleMode()
{
	ChangeType(EStateType::Idle);
}

void UStateComponent::SetSkillMode()
{
	ChangeType(EStateType::Skill);
}

void UStateComponent::SetHitMode()
{
	ChangeType(EStateType::Hit);
}

void UStateComponent::SetKnockBackMode()
{
	ChangeType(EStateType::KnockBack);
}

void UStateComponent::SetKnockDownMode()
{
	ChangeType(EStateType::KnockDown);
}

void UStateComponent::SetDeadMode()
{
	ChangeType(EStateType::Dead);
}

void UStateComponent::SetSequenceMode()
{
	ChangeType(EStateType::Sequence);
}
