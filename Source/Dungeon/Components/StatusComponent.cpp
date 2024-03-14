#include "Components/StatusComponent.h"
#include "Global.h"

UStatusComponent::UStatusComponent()
{
	SetIsReplicatedByDefault(1);
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(UStatusComponent, Level, COND_None);
	DOREPLIFETIME_CONDITION(UStatusComponent, MaxHealth, COND_None);
	DOREPLIFETIME_CONDITION(UStatusComponent, CurrentHealth, COND_None);
}

void UStatusComponent::OnRep_Level()
{
	// Level up event
}

void UStatusComponent::OnRep_MaxHealth()
{
	// Max Health Changed
}

void UStatusComponent::OnRep_CurrentHealth()
{
	// Current Health Changed
	// Send Cur/Max rate
	if (MaxHealth < 0)
	{
		CLog::Print("OnRep_CurrentHealth, MaxHealth < 0", -1, 10, FColor::Red);
		return;
	}
	OnCurrentHealthChanged.ExecuteIfBound(CurrentHealth / MaxHealth);
}

void UStatusComponent::AdjustCurrentHealth(float InValue)
{
	InValue += CurrentHealth;
	CurrentHealth = UKismetMathLibrary::FClamp(InValue, 0, MaxHealth);
	if (GetOwner()->HasAuthority())
		OnRep_CurrentHealth();
}

void UStatusComponent::SetUse()
{
	bCanUse = 1;
}

void UStatusComponent::SetCannotUse()
{
	bCanUse = 0;
}

void UStatusComponent::SetMove()
{
	bCanMove = 1;
}

void UStatusComponent::SetStop()
{
	bCanMove = 0;
}
