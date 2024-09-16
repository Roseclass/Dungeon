#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/NetSerialization.h"
#include "Objects/Eqquipment.h"
#include "EquipmentManagementComponent.generated.h"

class ADungeonCharacterBase;
class USceneComponent;

enum class EEquipmentInitType : uint8;
enum class EItemMode : uint8;
enum class EAttachmentRule : uint8;

USTRUCT()
struct FPlacedInitialData
{
	GENERATED_BODY()
public:
	UPROPERTY()FString UniqueID;
	UPROPERTY()FString EquipmentName;
	UPROPERTY()FEquipmentStateUpdateParameters State;
	UPROPERTY()FItemStatusData Status;
};

USTRUCT()
struct FSpawnedInitialData
{
	GENERATED_BODY()
public:
	UPROPERTY()FString UniqueID;
	UPROPERTY()TSubclassOf<AEqquipment> EquipmentClass;
	UPROPERTY()FEquipmentStateUpdateParameters State;
	UPROPERTY()FItemStatusData Status;
};

USTRUCT()
struct FEquipmentUniqueID : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	UPROPERTY()FString UniqueID;
	AEqquipment* Equipment;
};

USTRUCT()
struct FEquipmentUniqueIDArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()TArray<FEquipmentUniqueID> Items;

	void AddEquipmentUniqueID(const FEquipmentUniqueID& NewID)
	{
		for (auto& item : Items)
		{
			if (item.UniqueID == NewID.UniqueID)
			{
				item.Equipment = NewID.Equipment;
				MarkItemDirty(item);
				return;
			}
		}
		Items.Add(NewID);
		MarkItemDirty(Items.Last());
	}

	void RemoveEquipmentUniqueID(int32 Index)
	{
		if (Items.IsValidIndex(Index))
		{
			Items.RemoveAt(Index);
			MarkArrayDirty();
		}
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FEquipmentUniqueID, FEquipmentUniqueIDArray>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FEquipmentUniqueIDArray> : public TStructOpsTypeTraitsBase2<FEquipmentUniqueIDArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UEquipmentManagementComponent : public UActorComponent
{
	GENERATED_BODY()

	//static
private:
	static FString GenerateUniqueID(UClass* Class);
public:
	static AEqquipment* SpawnEquipment(UWorld* CurrentWorld, UClass* Class, const FEquipmentStateUpdateParameters& StateParameters = FEquipmentStateUpdateParameters(), bool UpadteStatus = 1);
	static void DestroyEquipment(UWorld* CurrentWorld, const FString& UniqueID);
	static AEqquipment* GetEquipmentFromUniqueID(UWorld* CurrentWorld, const FString& UniqueID);
	static void PickUp(UWorld* CurrentWorld, const FString& UniqueID, ADungeonCharacterBase* Owner);
	static void Equip(UWorld* CurrentWorld, const FString& UniqueID, ADungeonCharacterBase* Owner);
	static void Drop(UWorld* CurrentWorld, const FString& UniqueID, FVector Start, FVector End);

	static void UpadteStatus(UWorld* CurrentWorld, const FString& UniqueID, const FItemStatusData& NewData);
public:
	UEquipmentManagementComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	FEquipmentUniqueIDArray Equipments;
protected:
public:

	//function
private:
	void Init();
	UFUNCTION(Reliable, Server)void Server_Init();
	UFUNCTION(Client, Reliable)void Client_InitPlaced(const TArray<FPlacedInitialData>& Datas);
	UFUNCTION(Client, Reliable)void Client_InitSpawned(const TArray<FSpawnedInitialData>& Datas);

	UFUNCTION(Client, Reliable)void Client_SpawnEquipment(UClass* Class, const FEquipmentStateUpdateParameters& StateParameters, const FString& UniqueID);
	UFUNCTION(Client, Reliable)void Client_DestroyEquipment(const FString& UniqueID);

	UFUNCTION(Client, Reliable)void Client_Equip(const FString& UniqueID, ADungeonCharacterBase* Owner);
	UFUNCTION(Client, Reliable)void Client_PickUp(const FString& UniqueID, ADungeonCharacterBase* Owner);
	UFUNCTION(Client, Reliable)void Client_Drop(const FString& UniqueID, FVector Start, FVector End);

	UFUNCTION(Client, Reliable)void Client_UpadteStatus(const FString& UniqueID, const FItemStatusData& NewData);

protected:
public:
	FORCEINLINE const FEquipmentUniqueIDArray& GetEquipments()const { return Equipments; }
	bool IsValidID(const FString& ID, AEqquipment*& OutEquipment)const;
	bool IsValidEquipment(AEqquipment* Equipment, FString& OutID)const;
};
