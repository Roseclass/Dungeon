#include "Components/LootComponent.h"
#include "Global.h"

#include "Objects/Weapon.h"
#include "Objects/ItemManager.h"

ULootComponent::ULootComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULootComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DataTable)
		DataTable->GetAllRows<FDropItemTable>("", Datas);
}

void ULootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULootComponent::GenerateItems()
{
	if (Datas.IsEmpty())
	{
		CLog::Print("ULootComponent::GenerateItems Datas is empty", -1, 10, FColor::Red);
		return;
	}

	// find manager
	AItemManager* manager = Cast<AItemManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AItemManager::StaticClass()));
	if (!manager)
	{
		CLog::Print("ULootComponent::GenerateItems manager is nullptr", -1, 10, FColor::Red);
		return;
	}

	// generate items
	for (auto i : Datas)
	{
		int32 max = 1;
		float rate = i->Rate;
		while (1)
		{
			if (max > 1e8)break;
			if (max * i->Rate > 0)break;
			max *= 10;
		}		
		int32 num = UKismetMathLibrary::RandomIntegerInRange(0, max);
		if (num > rate * max) continue;
		//spawn item
		FTransform transform;		
		AEqquipment* equipment = manager->SpawnItemDeferred(i->ItemClass, transform, GetOwner());
		equipment->SetMode(EItemMode::Inventory);
		UGameplayStatics::FinishSpawningActor(equipment, transform);
		LootItems.Add(equipment);
	}
}

void ULootComponent::DropItems()
{
	//drop item
	for(auto i : LootItems)
	{ 
		// i->SetLootMode(start, end)
	}
	LootItems.Empty();
}
