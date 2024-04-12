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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEquippedDataChanged, UItemObject*, InObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryEquippedChanged, EAppearancePart, InMeshPart, int32, InIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEquippedChangeMeshVisiblity, bool, NewState);

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
	UPROPERTY(ReplicatedUsing = "OnRep_Items")TArray<AEqquipment*> Items;
	UPROPERTY(ReplicatedUsing = "OnRep_EquippedItems")TArray<AEqquipment*> EquippedItems;

	UPROPERTY()UUW_Inventory* Widget;
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> DefaultWeapon;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AEqquipment> InvTestClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Columns = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Rows = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUW_Inventory> WidgetClass;
public:
	FInventoryChanged OnInventoryChanged;
	TArray<FInventoryEquippedDataChanged> OnInventoryEquippedDataChanged;
	FInventoryEquippedChanged OnInventoryEquippedChanged;
	FInventoryEquippedChangeMeshVisiblity OnChangeHairVisiblity;
	//function
private:
	void InitDefault();
	void InitWidget();
	UFUNCTION() void OnRep_Items();
	UFUNCTION() void OnRep_EquippedItems();

	void Reset();
	void IndexToTile(int32 InIndex, int32& X, int32& Y);
	bool GetItemAtIndex(int32 InIndex, AEqquipment** OutObject);
protected:
public:
	void OnCollision();
	void OffCollision();
	void ResetHittedActors();

	bool IsRoomAvailable(AEqquipment* InObject, int32 TopLeftIndex);
	bool IsRoomAvailable(AEqquipment* InObject);
	bool IsRoomGreen(AEqquipment* InObject, int32 TopLeftIndex);

	UFUNCTION(Reliable, Server)void Server_TryAddItem(AEqquipment* InObject);
	UFUNCTION(Reliable, Server)void Server_AddItemAt(AEqquipment* InObject, int32 TopLeftIndex);

	UFUNCTION(Reliable, Server)void Server_RemoveItem(AEqquipment* InObject);

	void GetAllItems(TMap<AEqquipment*, TTuple<int32, int32>>& Map);

	FORCEINLINE int32 TileToIndex(int32 X, int32 Y) {return X + (Y * Columns);}
	FORCEINLINE int32 GetRows() { return Rows; }
	FORCEINLINE int32 GetColumns() { return Columns; }

	//for EquipmentSlot
	bool CanTakeOffEquipment(int32 InIdx);
	AEqquipment* GetEquippedItems(int32 InIdx);
	UFUNCTION(Reliable, Server)void Server_Equip(AEqquipment* InData);
	UFUNCTION(Reliable, Server)void Server_ChangeEquippedData(int32 InIdx, AEqquipment* InData);
	UFUNCTION(Reliable, Server)void Server_RemoveEquipped_Drag(int32 InIdx);

	//Widget
	bool IsWidgetVisible();
	void OnWidget();
	void OffWidget();

	//
	// 드래그 드롭 - 이동 및 장착, 해제
	// 우클릭 - 장착 및 해제
	// 장착전에 조건 확인
	// 장착시 외형 변경, 효과 적용
	// 드롭시 등급에 맞는 이펙트
	// 격자무늬 인벤토리 탭3개
	// 프리셋 탭 2개
	//
	
	//ui
	//void LoadData(UCSaveGameData* const InData);

};
