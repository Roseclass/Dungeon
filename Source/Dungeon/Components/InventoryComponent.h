#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/CharcterAppearanceData.h"
#include "InventoryComponent.generated.h"

/**
 * equip items, store items
 */

class AEqquipment;
class AWeapon;
class UUW_Inventory;
class UUW_Trade;
class USaveGameData;
class UGameplayEffect;

struct FDamageEhancementData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEquippedDataChanged, UItemObject*, InObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryEquippedChanged, EAppearancePart, InMeshPart, int32, InIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEquippedVisiblity, bool, NewState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	UPROPERTY(ReplicatedUsing = "OnRep_Items")TArray<FString> Items;
	UPROPERTY(ReplicatedUsing = "OnRep_EquippedItems")TArray<FString> EquippedItems;

	UPROPERTY()UUW_Inventory* Widget;
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> DefaultWeapon;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<AEqquipment>> DefaultItems;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Columns = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Rows = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUW_Inventory> WidgetClass;
public:
	FInventoryChanged OnInventoryChanged;
	FInventoryChanged OnInventoryEquippedItemsChanged;
	TArray<FInventoryEquippedDataChanged> OnInventoryEquippedDataChanged;
	FInventoryEquippedChanged OnInventoryEquippedChanged;
	FInventoryEquippedVisiblity OnChangeHairVisiblity;
	//function
private:
	void InitDefault();
	void InitWidget();
	UFUNCTION() void OnRep_Items();
	UFUNCTION() void OnRep_EquippedItems();

	void Reset();
	void IndexToTile(int32 InIndex, int32& X, int32& Y);
	bool GetItemAtIndex(int32 InIndex, AEqquipment** OutObject);
	UFUNCTION(Reliable, Server)void Server_LoadData(const TArray<TSubclassOf<AEqquipment>>& EquippedClasses, const TArray<FItemStatusData>& EquippedDatas,
													const TArray<FVector2D>& Locations, const TArray<TSubclassOf<AEqquipment>>& InventoryClasses, const TArray<FItemStatusData>& InventoryDatas);

	UFUNCTION(Client, Reliable)void Client_Trade(AActor* InActor);
	UFUNCTION(Reliable, Server)void Server_Buy(const FString& InObject);
protected:
public:
	//for equipment
	void OnCollision(const FDamageEhancementData* InDamageData);
	void OffCollision();
	void ResetHitActors();

	//for inventory grid
	bool IsRoomAvailable(AEqquipment* InObject, int32 TopLeftIndex);
	bool IsRoomAvailable(AEqquipment* InObject);
	bool IsRoomGreen(AEqquipment* InObject, int32 TopLeftIndex);

	UFUNCTION(Reliable, Server)void Server_TryAddItem(const FString& InObject);
	UFUNCTION(Reliable, Server)void Server_AddItemAt(const FString& InObject, int32 TopLeftIndex);

	UFUNCTION(Reliable, Server)void Server_RemoveItem(const FString& InObject);

	void GetAllItems(TMap<AEqquipment*, TTuple<int32, int32>>& Map);

	FORCEINLINE int32 TileToIndex(int32 X, int32 Y) {return X + (Y * Columns);}
	FORCEINLINE int32 GetRows() { return Rows; }
	FORCEINLINE int32 GetColumns() { return Columns; }

	//for trade
	void Trade(AActor* InActor);
	UFUNCTION(Reliable, Server)void Server_Sell(const FString& InObject);
	void Buy(AEqquipment* InObject);

	//for EquipmentSlot
	bool CanTakeOffEquipment(int32 InIdx);
	AEqquipment* GetEquippedItems(int32 InIdx);
	FORCEINLINE const TArray<FString>& GetAllEquippedItems() { return EquippedItems; }
	UFUNCTION(Reliable, Server)void Server_Equip(const FString& InData);
	UFUNCTION(Reliable, Server)void Server_ChangeEquippedData(int32 InIdx, const FString& InData);
	UFUNCTION(Reliable, Server)void Server_RemoveEquipped_Drag(int32 InIdx);

	//Widget
	bool IsWidgetVisible();
	void OnWidget();
	void OffWidget();
	void RefreshGrid();
	
	//for save
	void SaveData(USaveGameData* SaveData);
	void LoadData(USaveGameData* const ReadData);
};
