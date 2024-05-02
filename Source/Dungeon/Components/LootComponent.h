#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "LootComponent.generated.h"

class AEqquipment;

USTRUCT()
struct FDropItemTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<AEqquipment> ItemClass;

	UPROPERTY(EditAnywhere)
		float Rate = 0.5;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API ULootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULootComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	TArray<FDropItemTable*> Datas;
	TArray<AEqquipment*> LootItems;
protected:
	UPROPERTY(EditDefaultsOnly)
		UDataTable* DataTable;

public:

	//function
private:
protected:
public:
	void GenerateItems();
	void DropItems();
};
