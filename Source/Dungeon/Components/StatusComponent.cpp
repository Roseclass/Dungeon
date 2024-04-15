#include "Components/StatusComponent.h"
#include "Global.h"

#include "Components/InventoryComponent.h"
#include "Objects/Eqquipment.h"

UStatusComponent::UStatusComponent()
{
	SetIsReplicatedByDefault(1);
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		// bind on equipment changed
		UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwner());
		if (!inv)
		{
			CLog::Print("UStatusComponent::GetMaxHealth inv is nullptr", -1, 10, FColor::Red);
			return;
		}
		inv->OnInventoryEquippedItemsChanged.AddDynamic(this, &UStatusComponent::UpdateStatus);
	}
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

void UStatusComponent::OnRep_MaxMana()
{
	// Max Mana Changed
}

void UStatusComponent::OnRep_CurrentMana()
{
	// Current Mana Changed
}

void UStatusComponent::OnRep_Damage()
{
	// Damage Changed
}

void UStatusComponent::UpdateStatus()
{
	UpdateMaxHealth();
	UpdateMaxMana();
	UpdateDamage();
}

void UStatusComponent::UpdateMaxHealth()
{
	// set base health
	float result = 100;

	// get current equipment datas
	UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwner());
	if (!inv)
	{
		CLog::Print("UStatusComponent::GetMaxHealth inv is nullptr", -1, 10, FColor::Red);
		return;
	}
	auto items = inv->GetAllEquippedItems();

	// calculate max health
	for (auto i : items)
	{
		if (!i)continue;
		auto data = i->GetItemStatus();
		result += data.GetFinalMaxHealth();
	}

	result *= pow(1.1, Level);

	// update max health
	MaxHealth = result;
	CurrentHealth = UKismetMathLibrary::FClamp(CurrentHealth, 0, MaxHealth);
	if (GetOwner()->HasAuthority())
	{
		OnRep_MaxHealth();
		OnRep_CurrentHealth();
	}
}

void UStatusComponent::UpdateMaxMana()
{
	// set base Mana
	float result = 100;

	// get current equipment datas
	UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwner());
	if (!inv)
	{
		CLog::Print("UStatusComponent::GetMaxMana inv is nullptr", -1, 10, FColor::Red);
		return;
	}
	auto items = inv->GetAllEquippedItems();

	// calculate max Mana
	for (auto i : items)
	{
		if (!i)continue;
		auto data = i->GetItemStatus();
		result += data.GetFinalMaxMana();
	}

	result *= pow(1.1, Level);

	// update max Mana
	MaxMana = result;
	CurrentMana = UKismetMathLibrary::FClamp(CurrentMana, 0, MaxMana);
	if (GetOwner()->HasAuthority())
	{
		OnRep_MaxMana();
		OnRep_CurrentMana();
	}
}

void UStatusComponent::UpdateDamage()
{
	// set base Mana
	float result = 10 + Level;

	// get current equipment datas
	UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwner());
	if (!inv)
	{
		CLog::Print("UStatusComponent::GetMaxMana inv is nullptr", -1, 10, FColor::Red);
		return;
	}
	auto items = inv->GetAllEquippedItems();

	// calculate max Mana
	for (auto i : items)
	{
		if (!i)continue;
		auto data = i->GetItemStatus();
		result += data.GetFinalDamage();
	}

	// update Damage
	Damage = result;

	if (GetOwner()->HasAuthority())
		OnRep_Damage();
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
