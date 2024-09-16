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
	* 1.서버체크
	* 2.유효성검사
	* 3.스폰 및 uniqueid부여
	* 4.모든 플레이어 목록에 등록
	* 5.모든 플레이어 클라이언트에 스테이터스 동기화
	*
	* TODO::아이템 스탯 동기화
	*/

	// 1.서버체크
	if (CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority)
	{
		CLog::Print("UEquipmentManagementComponent::SpawnEquipment,this is not SERVER");
		return nullptr;
	}

	// 2.유효성검사
	if (!Class)return nullptr;

	// 3.스폰 및 uniqueid부여	
	FEquipmentUniqueID newID;
	newID.UniqueID = GenerateUniqueID(Class);
	newID.Equipment = CurrentWorld->SpawnActor<AEqquipment>(Class);
	newID.Equipment->AssignUniqueID(newID.UniqueID);
	newID.Equipment->UpdateState(StateParameters);

	// 서버 예외 처리
	{
		APlayerController* pc = CurrentWorld->GetFirstPlayerController();
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (comp)
			comp->Equipments.AddEquipmentUniqueID(newID);
	}

	// 4.모든 플레이어 목록에 등록
	for (auto it = CurrentWorld->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* pc = it->Get();
		if (!pc || pc == CurrentWorld->GetFirstPlayerController())continue;
		UEquipmentManagementComponent* comp = CHelpers::GetComponent<UEquipmentManagementComponent>(pc);
		if (!comp)continue;
		comp->Client_SpawnEquipment(Class, StateParameters, newID.UniqueID);
	}

	// 5.모든 플레이어 클라이언트에 스테이터스 동기화
	if (UpadteStatus)
		UEquipmentManagementComponent::UpadteStatus(CurrentWorld, newID.UniqueID, newID.Equipment->GetItemStatus());

	return newID.Equipment;
}

void UEquipmentManagementComponent::DestroyEquipment(UWorld* CurrentWorld, const FString& UniqueID)
{
	/*
	* 1.서버체크
	* 2.삭제
	*/

	// 1.서버체크
	if (CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority)
	{
		CLog::Print("UEquipmentManagementComponent::SpawnEquipment,this is not SERVER");
		return;
	}

	// 2.삭제
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
	* 1.유효성검사
	* 1-1.인풋 오너 유효?
	* 1-2.리스트에 장비 유효?
	* 2.멀티캐스트 동기화
	*/

	// 1.서버체크
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.유효성검사
	//1-1.인풋 오너 유효?
	if (!Owner)
	{
		CLog::Print("UEquipmentManagementComponent::PickUp there is NO Owner");
		return;
	}
	//1-2.리스트에 장비 유효?
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

	//2.멀티캐스트 동기화
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
	* 1.유효성검사
	* 1-1.인풋 오너 유효?
	* 1-2.리스트에 장비 유효?
	* 2.멀티캐스트 동기화
	*/

	// 1.서버체크
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.유효성검사
	//1-1.인풋 오너 유효?
	if (!Owner)
	{
		CLog::Print("UEquipmentManagementComponent::Equip there is NO Owner");
		return;
	}
	//1-2.리스트에 장비 유효?
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

	//2.멀티캐스트 동기화
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
	* 1.유효성검사
	* 1-1.리스트에 장비 유효?
	* 2.멀티캐스트 동기화
	*/

	// 1.서버체크
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.유효성검사
	//1-1.리스트에 장비 유효?
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

	//2.멀티캐스트 동기화
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
	* 1.유효성검사
	* 1-1.리스트에 장비 유효?
	* 2.멀티캐스트 동기화
	*/

	// 1.서버체크
	CheckTrue(CurrentWorld->GetFirstPlayerController()->GetLocalRole() != ENetRole::ROLE_Authority);

	//1.유효성검사
	//1-1.리스트에 장비 유효?
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

	//2.멀티캐스트 동기화
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
	* 1.서버인가?
	* 1-1.서버의 호스트인 경우
	* placed된 액터 정리
	* (spawned 액터들은 스폰시에 등록될 예정)
	* 2.클라이언트?
	* 2-1.동기화요청
	*/

	//1.서버인가?
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		ADungeonPlayerController* owner = Cast<ADungeonPlayerController>(GetOwner());
		CheckNull(owner);

		//1-1.서버의 호스트인 경우
		if (owner == GetWorld()->GetFirstPlayerController())
		{
			//placed된 액터 정리
			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEqquipment::StaticClass(), actors);
			for (auto i : actors)
			{
				AEqquipment* equipment = Cast<AEqquipment>(i);
				if (!equipment)continue;
				if (equipment->GetInitType() == EEquipmentInitType::Spawned)continue;

				//Equipments에 등록
				FEquipmentUniqueID newID;
				newID.UniqueID = GenerateUniqueID(equipment->GetClass());
				newID.Equipment = equipment;
				Equipments.AddEquipmentUniqueID(newID);
			}
		}
	}
	//2.클라이언트?
	else Server_Init();//2-1.동기화요청
}

void UEquipmentManagementComponent::Server_Init_Implementation()
{
	{
		//firstcontroller로 부터 데이터 동기화
		UEquipmentManagementComponent* firstComp = CHelpers::GetComponent<UEquipmentManagementComponent>(GetWorld()->GetFirstPlayerController());
		if (!firstComp)
		{
			CLog::Print("UEquipmentManagementComponent::Init, firstComp is nullptr");
			return;
		}

		//Equipments 복사
		Equipments = firstComp->GetEquipments();

		TArray<FSpawnedInitialData> spawnedDatas;
		TArray<FPlacedInitialData> placedDatas;
		for (auto item : Equipments.Items)
		{
			//배열 탐색후 placed와 spawned를 구분해 배열 생성
			AEqquipment* equipment = item.Equipment;
			if (equipment->GetInitType() == EEquipmentInitType::Spawned)
			{
				FSpawnedInitialData data;
				data.UniqueID = item.UniqueID;
				data.EquipmentClass = equipment->GetClass();
				data.State = equipment->GetEquipmentState();
				data.Status = equipment->GetItemStatus();
				spawnedDatas.Add(data);
			}
			else if (equipment->GetInitType() == EEquipmentInitType::Placed)
			{
				FPlacedInitialData data;
				data.UniqueID = item.UniqueID;
				data.EquipmentName = equipment->GetName();
				data.State = equipment->GetEquipmentState();
				data.Status = equipment->GetItemStatus();
				placedDatas.Add(data);
			}
		}

		//client 동기화 호출
		Client_InitSpawned(spawnedDatas);
		Client_InitPlaced(placedDatas);
	}
}

void UEquipmentManagementComponent::Client_InitPlaced_Implementation(const TArray<FPlacedInitialData>& Datas)
{
	/*
	* 1.Equipments 동기화 수행
	* 1-1.일치하는 이름이라면 Equipments에 등록
	* 1-2.일치하는 이름이 없다면 삭제
	*/

	//1.Equipments 동기화 수행
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

			//Equipments에 등록
			FEquipmentUniqueID newID;
			newID.UniqueID = data.UniqueID;
			newID.Equipment = equipment;
			newID.Equipment->UpdateState(data.State);
			newID.Equipment->UpdateStatus(data.Status);
			Equipments.AddEquipmentUniqueID(newID);
		}

		//1-2.일치하는 이름이 없다면 삭제
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
	* 1.Equipments 동기화 수행
	* 1-1.클래스에 맞게 스폰후 위치 조정 스폰 및 동기화
	*/

	//1.Equipments 동기화 수행
	for (auto& data : Datas)
	{
		AEqquipment* equipment = GetWorld()->SpawnActor<AEqquipment>(data.EquipmentClass);

		//Equipments에 등록
		FEquipmentUniqueID newID;
		newID.UniqueID = data.UniqueID;
		newID.Equipment = equipment;
		newID.Equipment->AssignUniqueID(newID.UniqueID);
		newID.Equipment->UpdateState(data.State);
		newID.Equipment->UpdateStatus(data.Status);
		Equipments.AddEquipmentUniqueID(newID);
	}
}

void UEquipmentManagementComponent::Client_SpawnEquipment_Implementation(UClass* Class, const FEquipmentStateUpdateParameters& StateParameters, const FString& UniqueID)
{
	// 중복 체크
	for (auto item : Equipments.Items)
		if (item.UniqueID == UniqueID)
		{
			CLog::Print(FString::Printf(TEXT("Client_SpawnEquipment,%s is already exist"), *UniqueID));
			return;
		}

	// 장비 등록
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
	* 1.유효성 검사
	* 2.장비 상태 변경
	*/

	//1.유효성 검사
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_Equip_Implementation equipment is nullptr");
		return;
	}

	//2.장비 상태 변경
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
	* 1.유효성 검사
	* 2.장비 상태 변경
	*/

	//1.유효성 검사
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_PickUp_Implementation equipment is nullptr");
		return;
	}

	//2.장비 상태 변경
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
	* 1.유효성 검사
	* 2.장비 상태 변경
	*/

	//1.유효성 검사
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_DropSequence_Implementation equipment is nullptr");
		return;
	}

	//2.장비 상태 변경
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
	* 1.유효성 검사
	* 2.장비 상태 변경
	*/

	//1.유효성 검사
	AEqquipment* equipment = nullptr;
	if (!IsValidID(UniqueID, equipment))
	{
		CLog::Print("UEquipmentManagementComponent::Client_DropSequence_Implementation equipment is nullptr");
		return;
	}

	//2.장비 상태 변경
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
* inittype에 대한 몇가지가 더 필요할듯 싶음
* 전투 스테이지에서 스폰된 상황의 아이템은 스테이지 종료시에 인벤토리에 없다면 삭제해야댐
* 그냥 스테이지 종료시에 인벤토리에 없으면 삭제하면 되는거 아닌가? ㅋ;
* 
*/