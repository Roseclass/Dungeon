#include "Components/LootComponent.h"
#include "Global.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/Enemy.h"
#include "Objects/Weapon.h"
#include "Components/EquipmentManagementComponent.h"

ULootComponent::ULootComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULootComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckTrue(GetOwnerRole() != ENetRole::ROLE_Authority);
	GenerateItems();
	AEnemy* owner = Cast<AEnemy>(GetOwner());
	owner->OnEnemyDead.AddUFunction(this, "DropItems");
}

void ULootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULootComponent::GenerateItems()
{
	if (DataTable)
		DataTable->GetAllRows<FDropItemTable>("", Datas);

	if (Datas.IsEmpty())
	{
		CLog::Print("ULootComponent::GenerateItems Datas is empty", -1, 10, FColor::Red);
		return;
	}

	// generate items
	AEnemy* owner = Cast<AEnemy>(GetOwner());
	for (auto i : Datas)
	{
		double max = 1e5;
		float rate = i->Rate;
		int32 num = UKismetMathLibrary::RandomIntegerInRange(0, max);
		if (num > rate * max) continue;
		//spawn item
		FEquipmentStateUpdateParameters state;
		state.State = EItemMode::Inventory;
		state.NewOwner = owner;
		UEquipmentManagementComponent::SpawnEquipment(GetWorld(), i->ItemClass, state);
	}
}

void ULootComponent::DropItems()
{
	CheckTrue(GetOwnerRole() != ENetRole::ROLE_Authority);
	AEnemy* owner = Cast<AEnemy>(GetOwner());
	//drop item
	for(auto i : LootItems)
	{ 
		// find drop location
		float halfSize = owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector start = owner->GetActorLocation();
		start.Z -= halfSize;
		FVector end = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), start, 200);
		start.Z += halfSize;

		UEquipmentManagementComponent::Drop(GetWorld(), i->GetUniqueID(), start, end);
	}
	LootItems.Empty();
}
