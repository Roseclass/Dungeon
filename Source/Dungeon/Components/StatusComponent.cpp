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
		UpdateStatus();
		UpdateRegen();

		CurrentHealth = MaxHealth;
		CurrentMana = MaxMana;

		OnRep_CurrentHealth();
		OnRep_CurrentMana();

		// bind on equipment changed
		UInventoryComponent* inv = CHelpers::GetComponent<UInventoryComponent>(GetOwner());
		if (!inv)
		{
			CLog::Print("UStatusComponent::GetMaxHealth inv is nullptr", -1, 10, FColor::Red);
			return;
		}
		inv->OnInventoryEquippedItemsChanged.AddDynamic(this, &UStatusComponent::UpdateStatus);
		inv->OnInventoryEquippedItemsChanged.AddDynamic(this, &UStatusComponent::UpdateRegen);
	}
}

void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->HasAuthority())
	{
		AmountOfHealthRegen_Server += HealthRegen * DeltaTime;
		AmountOfManaRegen_Server += ManaRegen * DeltaTime;
	}

	if (GetOwner()->GetOwner())
	{
		AmountOfHealthRegen_Client += HealthRegen * DeltaTime;
		AmountOfManaRegen_Client += ManaRegen * DeltaTime;
	}
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated 변수를 여기에 추가
	DOREPLIFETIME_CONDITION(UStatusComponent, Level, COND_None);

	DOREPLIFETIME_CONDITION(UStatusComponent, MaxHealth, COND_None);
	DOREPLIFETIME_CONDITION(UStatusComponent, CurrentHealth, COND_None);
	DOREPLIFETIME_CONDITION(UStatusComponent, HealthRegen, COND_None);

	DOREPLIFETIME_CONDITION(UStatusComponent, MaxMana, COND_None);
	DOREPLIFETIME_CONDITION(UStatusComponent, CurrentMana, COND_None);
	DOREPLIFETIME_CONDITION(UStatusComponent, ManaRegen, COND_None);

	DOREPLIFETIME_CONDITION(UStatusComponent, Damage, COND_None);
}

void UStatusComponent::OnRep_Level()
{
	// Level up event
}

void UStatusComponent::OnRep_MaxHealth()
{
	// Max Health Changed
	OnMaxHealthChanged.Broadcast(MaxHealth);
}

void UStatusComponent::OnRep_CurrentHealth()
{
	// Current Health Changed

	// check error
	if (MaxHealth < 0)
	{
		CLog::Print("OnRep_CurrentHealth, MaxHealth < 0", -1, 10, FColor::Red);
		return;
	}

	// Send Cur/Max rate
	OnCurrentHealthChanged.Broadcast(CurrentHealth / MaxHealth);

	// reset client data
	AmountOfHealthRegen_Client = 0;
}

void UStatusComponent::OnRep_HealthRegen()
{
	// HealthRegen Changed
	OnHealthRegenChanged.Broadcast(HealthRegen);
}

void UStatusComponent::OnRep_MaxMana()
{
	// Max Mana Changed
	OnMaxManaChanged.Broadcast(MaxMana);
}

void UStatusComponent::OnRep_CurrentMana()
{
	// Current Mana Changed

	// check error
	if (MaxMana < 0)
	{
		CLog::Print("OnRep_CurrentHealth, MaxHealth < 0", -1, 10, FColor::Red);
		return;
	}

	// Send Cur/Max rate
	OnCurrentManaChanged.Broadcast(CurrentMana / MaxMana);

	// reset client data
	AmountOfManaRegen_Client = 0;
}

void UStatusComponent::OnRep_ManaRegen()
{
	// ManaRegen Changed
	OnManaRegenChanged.Broadcast(ManaRegen);
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

void UStatusComponent::UpdateRegen()
{
	UpdateHealthRegen();
	UpdateManaRegen();
}

void UStatusComponent::UpdateHealthRegen()
{
	// set base Health Regen
	float result = 5;

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
		result += data.GetFinalHealthRegen();
	}

	result *= pow(1.1, Level);

	// update Health Regen
	HealthRegen = result;
	if (GetOwner()->HasAuthority())
		OnRep_HealthRegen();
}

void UStatusComponent::UpdateManaRegen()
{
	// set base Mana Regen
	float result = 5;

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
		result += data.GetFinalManaRegen();
	}

	result *= pow(1.1, Level);

	// update Mana Regen
	ManaRegen = result;
	if (GetOwner()->HasAuthority())
		OnRep_ManaRegen();
}

void UStatusComponent::AdjustCurrentHealth(float InValue)
{
	float result = UKismetMathLibrary::FClamp(CurrentHealth + AmountOfHealthRegen_Server, 0, MaxHealth);
	AmountOfHealthRegen_Server = 0;

	CurrentHealth = UKismetMathLibrary::FClamp(result + InValue, 0, MaxHealth);
	if (GetOwner()->HasAuthority()) 
		OnRep_CurrentHealth();
}

void UStatusComponent::AdjustCurrentMana(float InValue)
{
	float result = UKismetMathLibrary::FClamp(CurrentMana + AmountOfManaRegen_Server, 0, MaxMana);
	AmountOfManaRegen_Server = 0;

	CurrentMana = UKismetMathLibrary::FClamp(result + InValue, 0, MaxMana);
	if (GetOwner()->HasAuthority())
		OnRep_CurrentMana();
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

void UStatusComponent::Update()
{
	OnRep_Level();

	OnRep_MaxHealth();
	OnRep_CurrentHealth();
	OnRep_HealthRegen();

	OnRep_MaxMana();
	OnRep_CurrentMana();
	OnRep_ManaRegen();

	OnRep_Damage();
}
