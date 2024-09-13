#include "Components/EquipmentManagementComponent.h"
#include "Global.h"
#include "Misc/DateTime.h"
#include "Misc/Guid.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"

#include "DungeonPlayerController.h"
#include "Characters/Enemy.h"
#include "Characters/DungeonCharacterBase.h"
#include "Objects/ItemObject.h"

FString UEquipmentManagementComponent::GenerateUniqueID(UClass* Class)
{
	if (!Class)CLog::Print("UEquipmentManagementComponent::GenerateUniqueID Class is nullptr");

	FString name = Class ? Class->GetName() : "NONE";
	FString timeStamp = FDateTime::Now().ToString();
	FString guid = FGuid::NewGuid().ToString();
	return FString::Printf(TEXT("%s_%s_%s"), *name, *timeStamp, *guid);
}

AEqquipment* UEquipmentManagementComponent::SpawnEquipment(UWorld* CurrentWorld, UClass* Class, const FEquipmentStateUpdateParameters& StateParameters, bool UpadteStatus)
{
	/*
	* 1.����üũ
	* 2.���� �� uniqueid�ο�
	* 3.��� �÷��̾� ��Ͽ� ���
	* 4.��� �÷��̾� Ŭ���̾�Ʈ�� �������ͽ� ����ȭ
	*
	* TODO::������ ���� ����ȭ
	*/

	// 1.����üũ
	if (CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority)
	{
		CLog::Print("UEquipmentManagementComponent::SpawnEquipment,this is not SERVER");
		return nullptr;
	}

	// 2.���� �� uniqueid�ο�	
	FEquipmentUniqueID newID;
	newID.UniqueID = GenerateUniqueID(Class);
	newID.Equipment = CurrentWorld->SpawnActor<AEqquipment>(Class);
	newID.Equipment->AssignUniqueID(newID.UniqueID);
	newID.Equipment->UpdateState(StateParameters);

	// 3.��� �÷��̾� ��Ͽ� ���
	for (auto it = CurrentWorld->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pc = it->Get();
		if (!pc)continue;
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		comp->Client_SpawnEquipment(Class, StateParameters, newID.UniqueID);
	}

	//4.��� �÷��̾� Ŭ���̾�Ʈ�� �������ͽ� ����ȭ
	if (UpadteStatus)
		UEquipmentManagementComponent::UpadteStatus(CurrentWorld, newID.UniqueID, newID.Equipment->GetItemStatus());

	return newID.Equipment;
}

void UEquipmentManagementComponent::DestroyEquipment(UWorld* CurrentWorld, const FString& UniqueID)
{
	/*
	* 1.����üũ
	* 2.����
	*/

	// 1.����üũ
	if (CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority)
	{
		CLog::Print("UEquipmentManagementComponent::SpawnEquipment,this is not SERVER");
		return;
	}

	// 2.����
	for (auto it = CurrentWorld->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pc = it->Get();
		if (!pc)continue;
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		comp->Client_DestroyEquipment(UniqueID);
	}
}

AEqquipment* UEquipmentManagementComponent::GetEquipmentFromUniqueID(UWorld* CurrentWorld, const FString& UniqueID)
{
	APlayerController* pc = CurrentWorld->GetFirstPlayerController();
	if (!pc)return nullptr;
	UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
	if (!comp)return nullptr;

	AEqquipment* result=nullptr;
	comp->IsValidID(UniqueID, result);

	return result;
}

void UEquipmentManagementComponent::PickUp(UWorld* CurrentWorld, const FString& UniqueID, ADungeonCharacterBase* Owner)
{
	/*
	* 1.��ȿ���˻�
	* 1-1.��ǲ ���� ��ȿ?
	* 1-2.����Ʈ�� ��� ��ȿ?
	* 2.��Ƽĳ��Ʈ ����ȭ
	*/

	// 1.����üũ
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.��ȿ���˻�
	//1-1.��ǲ ���� ��ȿ?
	if (!Owner)
	{
		CLog::Print("UEquipmentManagementComponent::PickUp there is NO Owner");
		return;
	}
	//1-2.����Ʈ�� ��� ��ȿ?
	TArray<APlayerController*>pcs;
	for (auto it = CurrentWorld->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pc = it->Get();
		if (!pc)continue;
		pcs.Add(pc);
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		AEqquipment* equipment = nullptr;
		if (!comp->IsValidID(UniqueID, equipment))
		{
			CLog::Print("UEquipmentManagementComponent::PickUp equipment is nullptr");
			return;
		}
	}

	//2.��Ƽĳ��Ʈ ����ȭ
	for (auto pc : pcs)
	{
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		comp->Client_PickUp(UniqueID, Owner);
	}
}

void UEquipmentManagementComponent::Equip(UWorld* CurrentWorld, const FString& UniqueID, ADungeonCharacterBase* Owner)
{
	/*
	* 1.��ȿ���˻�
	* 1-1.��ǲ ���� ��ȿ?
	* 1-2.����Ʈ�� ��� ��ȿ?
	* 2.��Ƽĳ��Ʈ ����ȭ
	*/

	// 1.����üũ
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.��ȿ���˻�
	//1-1.��ǲ ���� ��ȿ?
	if (!Owner)
	{
		CLog::Print("UEquipmentManagementComponent::Equip there is NO Owner");
		return;
	}
	//1-2.����Ʈ�� ��� ��ȿ?
	TArray<APlayerController*>pcs;
	for (auto it = CurrentWorld->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pc = it->Get();
		if (!pc)continue;
		pcs.Add(pc);
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		AEqquipment* equipment = nullptr;
		if (!comp->IsValidID(UniqueID, equipment))
		{
			CLog::Print("UEquipmentManagementComponent::Equip equipment is nullptr");
			return;
		}
	}

	//2.��Ƽĳ��Ʈ ����ȭ
	for (auto pc : pcs)
	{
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		comp->Client_Equip(UniqueID, Owner);
	}
}

void UEquipmentManagementComponent::Drop(UWorld* CurrentWorld, const FString& UniqueID, FVector Start, FVector End)
{
	/*
	* 1.��ȿ���˻�
	* 1-1.����Ʈ�� ��� ��ȿ?
	* 2.��Ƽĳ��Ʈ ����ȭ
	*/

	// 1.����üũ
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.��ȿ���˻�
	//1-1.����Ʈ�� ��� ��ȿ?
	TArray<APlayerController*>pcs;
	for (auto it = CurrentWorld->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pc = it->Get();
		if (!pc)continue;
		pcs.Add(pc);
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		AEqquipment* equipment = nullptr;
		if (!comp->IsValidID(UniqueID, equipment))
		{
			CLog::Print("UEquipmentManagementComponent::Drop equipment is nullptr");
			return;
		}
	}

	//2.��Ƽĳ��Ʈ ����ȭ
	for (auto pc : pcs)
	{
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		comp->Client_Drop(UniqueID, Start, End);
	}
}

void UEquipmentManagementComponent::UpadteStatus(UWorld* CurrentWorld, const FString& UniqueID, const FItemStatusData& NewData)
{
	/*
	* 1.��ȿ���˻�
	* 1-1.����Ʈ�� ��� ��ȿ?
	* 2.��Ƽĳ��Ʈ ����ȭ
	*/

	// 1.����üũ
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.��ȿ���˻�
	//1-1.����Ʈ�� ��� ��ȿ?
	TArray<APlayerController*>pcs;
	for (auto it = CurrentWorld->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pc = it->Get();
		if (!pc)continue;
		pcs.Add(pc);
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		AEqquipment* equipment = nullptr;
		if (!comp->IsValidID(UniqueID, equipment))
		{
			CLog::Print("UEquipmentManagementComponent::UpadteStatus equipment is nullptr");
			return;
		}
	}

	//2.��Ƽĳ��Ʈ ����ȭ
	for (auto pc : pcs)
	{
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		comp->Client_UpadteStatus(UniqueID, NewData);
	}
}

UEquipmentManagementComponent::UEquipmentManagementComponent()
{
	SetIsReplicatedByDefault(1);
	PrimaryComponentTick.bCanEverTick = true;
}

void UEquipmentManagementComponent::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void UEquipmentManagementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEquipmentManagementComponent::Init()
{
	/*
	* 1.�����ΰ�?
	* 1-1.������ ȣ��Ʈ�� ���
	* placed�� ���� ����
	* (spawned ���͵��� �����ÿ� ��ϵ� ����)
	* 1-2.ȣ��Ʈ�� �ƴѰ��
	* firstcontroller�� ���� ������ ����ȭ
	*/

	//1.�����ΰ�?
	CheckFalse(GetOwnerRole() == ENetRole::ROLE_Authority);
	
	ADungeonPlayerController* owner = Cast<ADungeonPlayerController>(GetOwner());
	CheckNull(owner);

	//1-1.������ ȣ��Ʈ�� ���
	if (owner == GetWorld()->GetFirstPlayerController())
	{
		//placed�� ���� ����
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEqquipment::StaticClass(), actors);
		for (auto i : actors)
		{
			AEqquipment* equipment = Cast<AEqquipment>(i);
			if (!equipment)continue;
			if(equipment->GetInitType() == EEquipmentInitType::Spawned)continue;
			
			//Equipments�� ���
			FEquipmentUniqueID newID;
			newID.UniqueID = GenerateUniqueID(equipment->GetClass());
			newID.Equipment = equipment;
			Equipments.AddEquipmentUniqueID(newID);
		}
	}
	//1-2.ȣ��Ʈ�� �ƴѰ��
	else
	{
		//firstcontroller�� ���� ������ ����ȭ
		UEquipmentManagementComponent* firstComp = CHelpers::GetComponent<UEquipmentManagementComponent>(GetWorld()->GetFirstPlayerController());
		if (!firstComp)
		{
			CLog::Print("UEquipmentManagementComponent::Init, firstComp is nullptr");
			return;
		}

		//Equipments ����
		Equipments = firstComp->GetEquipments();

		TArray<FPlacedInitialData> placedDatas;
		TArray<FSpawnedInitialData> spawnedDatas;
		for (auto item : Equipments.Items)
		{
			//�迭 Ž���� placed�� spawned�� ������ �迭 ����
			AEqquipment* equipment = item.Equipment;
			if(equipment->GetInitType() == EEquipmentInitType::Spawned)
			{
				FSpawnedInitialData data;
				data.UniqueID = item.UniqueID;
				data.EquipmentClass = equipment->GetClass();
				data.State = equipment->GetEquipmentState();
			}
			else if(equipment->GetInitType() == EEquipmentInitType::Placed)
			{
				FPlacedInitialData data;
				data.UniqueID = item.UniqueID;
				data.EquipmentName = equipment->GetName();
				data.State = equipment->GetEquipmentState();
			}
		}

		//client ����ȭ ȣ��
		Client_InitSpawned(spawnedDatas);
		Client_InitPlaced(placedDatas);
	}
}

void UEquipmentManagementComponent::Client_InitPlaced_Implementation(const TArray<FPlacedInitialData>& Datas)
{
	/*
	* 1.Equipments ����ȭ ����
	* 1-1.��ġ�ϴ� �̸��̶�� Equipments�� ���
	* 1-2.��ġ�ϴ� �̸��� ���ٸ� ����
	*/

	//1.Equipments ����ȭ ����
	TArray<AActor*> actors;
	TMap<AEqquipment*,bool> equipmentMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEqquipment::StaticClass(), actors);
	for (auto actor : actors)
	{
		AEqquipment* equipment = Cast<AEqquipment>(actor);
		if (!equipment)continue;
		equipmentMap.Add(equipment, 0);
	}

	for (auto& data : Datas)
	{
		for (auto& map : equipmentMap)
		{
			if (map.Value)continue;
			AEqquipment* equipment = map.Key;
			if (!equipment)continue;
			if (equipment->GetInitType() == EEquipmentInitType::Spawned)continue;
			if (equipment->GetName() != data.EquipmentName)continue;
			map.Value = 1;

			//Equipments�� ���
			FEquipmentUniqueID newID;
			newID.UniqueID = data.UniqueID;
			newID.Equipment = equipment;
			newID.Equipment->UpdateState(data.State);
			Equipments.AddEquipmentUniqueID(newID);
		}

		//1-2.��ġ�ϴ� �̸��� ���ٸ� ����
		TArray<AEqquipment*> arr;
		for (auto& map : equipmentMap)
		{
			if (map.Value)continue;
			AEqquipment* equipment = map.Key;
			if (!equipment)continue;
			if (equipment->GetInitType() == EEquipmentInitType::Spawned)continue;
			arr.Add(equipment);
		}
		for (auto i : arr)
		{
			equipmentMap.Remove(i);
			i->Destroy();
		}
	}
}

void UEquipmentManagementComponent::Client_InitSpawned_Implementation(const TArray<FSpawnedInitialData>& Datas)
{
	/*
	* 1.Equipments ����ȭ ����
	* 1-1.Ŭ������ �°� ������ ��ġ ���� ���� �� ����ȭ
	*/

	//1.Equipments ����ȭ ����
	for (auto& data : Datas)
	{
		AEqquipment* equipment = GetWorld()->SpawnActor<AEqquipment>(data.EquipmentClass);

		//Equipments�� ���
		FEquipmentUniqueID newID;
		newID.UniqueID = data.UniqueID;
		newID.Equipment = equipment;
		newID.Equipment->AssignUniqueID(newID.UniqueID);
		newID.Equipment->UpdateState(data.State);
		Equipments.AddEquipmentUniqueID(newID);
	}
}

void UEquipmentManagementComponent::Client_SpawnEquipment_Implementation(UClass* Class, const FEquipmentStateUpdateParameters& StateParameters, const FString& UniqueID)
{
	// �ߺ� üũ
	for (auto item : Equipments.Items)
		if (item.UniqueID == UniqueID)
		{
			CLog::Print(FString::Printf(TEXT("Client_SpawnEquipment,%s is already exist"), *UniqueID));
			return;
		}

	// ��� ���
	FEquipmentUniqueID newID;
	newID.UniqueID = UniqueID;
	newID.Equipment = GetWorld()->SpawnActor<AEqquipment>(Class);
	newID.Equipment->AssignUniqueID(newID.UniqueID);
	newID.Equipment->UpdateState(StateParameters);
	Equipments.AddEquipmentUniqueID(newID);
}

void UEquipmentManagementComponent::Client_DestroyEquipment_Implementation(const FString& UniqueID)
{
	for (int32 i = 0; i < Equipments.Items.Num();)
	{
		if (Equipments.Items[i].UniqueID == UniqueID)
		{
			Equipments.Items[i].Equipment->Destroy();
			Equipments.RemoveEquipmentUniqueID(i);
		}
	}
}

void UEquipmentManagementComponent::Client_Equip_Implementation(const FString& UniqueID, ADungeonCharacterBase* Owner)
{
	/*
	* 1.��ȿ�� �˻�
	* 2.��� ���� ����
	*/

	//1.��ȿ�� �˻�
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_Equip_Implementation equipment is nullptr");
		return;
	}

	//2.��� ���� ����
	{
		FEquipmentStateUpdateParameters params;
		params.State = EItemMode::Equip;
		params.NewOwner = Owner;
		equipment->UpdateState(params);
	}
}

void UEquipmentManagementComponent::Client_PickUp_Implementation(const FString& UniqueID, ADungeonCharacterBase* Owner)
{
	/*
	* 1.��ȿ�� �˻�
	* 2.��� ���� ����
	*/

	//1.��ȿ�� �˻�
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_PickUp_Implementation equipment is nullptr");
		return;
	}

	//2.��� ���� ����
	{
		FEquipmentStateUpdateParameters params;
		params.State = EItemMode::Inventory;
		params.NewOwner = Owner;
		equipment->UpdateState(params);
	}
}

void UEquipmentManagementComponent::Client_Drop_Implementation(const FString& UniqueID, FVector Start, FVector End)
{
	/*
	* 1.��ȿ�� �˻�
	* 2.��� ���� ����
	*/

	//1.��ȿ�� �˻�
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_DropSequence_Implementation equipment is nullptr");
		return;
	}

	//2.��� ���� ����
	{
		FEquipmentStateUpdateParameters params;
		params.State = EItemMode::Drop;
		params.NewOwner = nullptr;
		params.DropStart = Start;
		params.DropEnd = End;
		equipment->UpdateState(params); 
	}
}

void UEquipmentManagementComponent::Client_UpadteStatus_Implementation(const FString& UniqueID, const FItemStatusData& NewData)
{
	/*
	* 1.��ȿ�� �˻�
	* 2.��� ���� ����
	*/

	//1.��ȿ�� �˻�
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_DropSequence_Implementation equipment is nullptr");
		return;
	}

	//2.��� ���� ����
	{
		equipment->UpdateStatus(NewData);
	}
}

bool UEquipmentManagementComponent::IsValidID(const FString& ID, AEqquipment*& OutEquipment)const
{
	for (auto i : Equipments.Items)
		if (i.UniqueID == ID)
		{
			OutEquipment = i.Equipment;
			return i.Equipment != nullptr;
		}
	return 0;
}

bool UEquipmentManagementComponent::IsValidEquipment(AEqquipment* Equipment, FString& OutID)const
{
	for (auto i : Equipments.Items)
		if (i.Equipment == Equipment)
		{
			OutID = i.UniqueID;
			return 1;
		}
	return 0;
}

/*
* 
* inittype�� ���� ����� �� �ʿ��ҵ� ����
* ���� ������������ ������ ��Ȳ�� �������� �������� ����ÿ� �κ��丮�� ���ٸ� �����ؾߴ�
* �׳� �������� ����ÿ� �κ��丮�� ������ �����ϸ� �Ǵ°� �ƴѰ�? ��;
* 
* ������ �������ͽ��� ��� ����?
* �ϴ� �ٸ��� ���� �����ϰ� �غ���
* ó���� �������ϴ� ���������� �˰��־ ������ �ȵɰŰ���
* ui�����͸� �� ���̰� �ϸ� ���� ���� ������?
* 
* �κ��丮 �ε�?
* �κ��丮 �����͸� ��� �������� �����غ���
* equipment �迭�� unique id �迭�� ����
* 
* �÷��̾� ��Ʈ�ѷ��� equip pickup drop �����Լ� �̰�
* ���⼭�� �Ϲ��Լ� ȣ��
* 
* serverinteract(pc)->invcomp���� �����ϴٰ� �ǴܵǸ� pickupȣ������
* �� ������Ʈ�� ����� ���¸� ����ȭ �����ְ� ����Ʈ�� �����ϴ� �������� �κ��丮 ������Ʈ�� �����ϰ� �̷��� ������ �ƴ�
* 
* ��Ÿ �������� ��ȣ�ۿ��Ҷ��� ��Ʈ�ѷ��� �̿��� ��ȣ�ۿ��ϵ��� ����
* 
*/