#include "Components/LootComponent.h"
#include "Global.h"

#include "DungeonPlayerController.h"
#include "Characters/Enemy.h"
#include "Objects/Weapon.h"
#include "Objects/ItemManager.h"

ULootComponent::ULootComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULootComponent::BeginPlay()
{
	Super::BeginPlay();
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

	// find manager
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	AItemManager* manager = controller->GetItemManager();
	if (!manager)
		CLog::Print("ULootComponent::GenerateItems manager is nullptr", -1, 10, FColor::Yellow);

	// generate items
	for (auto i : Datas)
	{
		double max = 1e5;
		float rate = i->Rate;
		int32 num = UKismetMathLibrary::RandomIntegerInRange(0, max);
		if (num > rate * max) continue;
		//spawn item
		FTransform transform;		
		AEqquipment* equipment = manager->SpawnItemDeferred(i->ItemClass, transform, GetOwner());
		equipment->SetMode(EItemMode::Inventory);
		UGameplayStatics::FinishSpawningActor(equipment, transform);
		LootItems.Add(equipment);
		equipment->SetOwnerCharacter(Cast<AEnemy>(GetOwner()));
	}
}

void ULootComponent::DropItems()
{
	//drop item
	for(auto i : LootItems)
	{ 
		i->ChangeVisibility(EItemMode::Loot);
	}
	LootItems.Empty();
}
