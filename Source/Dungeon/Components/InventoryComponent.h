#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
 * ������ ���� �� ����
 */

class AWeapon;
class UUW_Inventory;
class UItemObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEquipWeapon, AWeapon*, InAttachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryPresetChanged);

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

	//property
private:
	UPROPERTY()TArray<UItemObject*> Items;
	UPROPERTY()TArray<UItemObject*> PresetItems;
	int32 PresetIndex;

	UPROPERTY()UUW_Inventory* Widget;

	AWeapon* CurrentWeapon;

protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> DefaultWeapon;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> InvTestClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Columns = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (ClampMin = 1.00))
		int32 Rows = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUW_Inventory> WidgetClass;
public:
	FInventoryChanged OnInventoryChanged;
	FInventoryEquipWeapon OnInventoryEquipWeapon;
	FInventoryPresetChanged OnInventoryPresetChanged;

	//function
private:
	void InitDefault();

	void Reset();
	void IndexToTile(int32 InIndex, int32& X, int32& Y);
	bool GetItemAtIndex(int32 InIndex, UItemObject** OutObject);

protected:
public:
	void OnCollision();
	void OffCollision();
	void ResetHittedActors();


	bool IsRoomAvailable(UItemObject* InObject, int32 TopLeftIndex);
	bool IsRoomAvailable(UItemObject* InObject);
	bool TryAddItem(UItemObject* InObject);
	void AddItemAt(UItemObject* InObject, int32 TopLeftIndex);
	void RemoveItem(UItemObject* InObject);

	void GetAllItems(TMap<UItemObject*, TTuple<int32, int32>>& Map);

	FORCEINLINE int32 TileToIndex(int32 X, int32 Y) {return X + (Y * Columns);}
	FORCEINLINE int32 GetRows() { return Rows; }
	FORCEINLINE int32 GetColumns() { return Columns; }

	//for EquipmentSlot
	bool CanTakeOffEquipment(int32 InIdx);
	bool CanTakeOffCurrentEquipment();
	UItemObject* GetPresetItems(int32 InIdx);
	FORCEINLINE UItemObject* GetCurrentPresetItem() { return PresetItems[PresetIndex]; };
	void Equip(UItemObject* InData);
	void EquipPreset(int32 InIdx);
	bool ChangePresetData(int32 InIdx, UItemObject* InData);
	void ChangePresetIndex(int32 InIdx);
	bool RemovePreset(int32 InIdx);
	bool RemovePreset_Drag(int32 InIdx);

	//Widget
	bool IsWidgetVisible();
	void OnWidget();
	void OffWidget();

	//
	// �巡�� ��� - �̵� �� ����, ����
	// ��Ŭ�� - ���� �� ����
	// �������� ���� Ȯ��
	// ������ ���� ����, ȿ�� ����
	// ��ӽ� ��޿� �´� ����Ʈ
	// ���ڹ��� �κ��丮 ��3��
	// ������ �� 2��
	//
	
	//ui
	//void LoadData(UCSaveGameData* const InData);

};
