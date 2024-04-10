#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.generated.h"

/*
* 아이템 관리 담당, 월드마다 하나씩 배치
* 플레이어 컨트롤러를 오너로 삼고 아이템들의 상태를 동기화
*/

class AEqquipment;
class USceneComponent;

enum class EItemMode : uint8;
enum class EAttachmentRule : uint8;

UCLASS()
class DUNGEON_API AItemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemManager();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
protected:
public:

	//function
private:
	UFUNCTION(Reliable, Server)void Server_SetItemLocation(AEqquipment* InItem, const FVector& NewLocation, bool bSweep, const FHitResult& OutSweepHitResult, ETeleportType Teleport);
	UFUNCTION(Reliable, Server)void Server_SetItemRotation(AEqquipment* InItem, const FRotator& NewRotation, ETeleportType Teleport);
	UFUNCTION(Reliable, Server)void Server_AttachItemToComponent(AEqquipment* InItem, USceneComponent* Parent, EAttachmentRule Location, EAttachmentRule Rotation, EAttachmentRule Scale, bool bWeldSimulatedBodies, const FName& SocketName);
	UFUNCTION(Reliable, Server)void Server_ChangeVisibility(AEqquipment* InItem, EItemMode NewMode);
protected:
public:
	void SetItemLocation(AEqquipment* InItem, const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	void SetItemRotation(AEqquipment* InItem, FRotator NewRotation, ETeleportType Teleport = ETeleportType::None);
	void AttachItemToComponent(AEqquipment* InItem, USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName SocketName = NAME_None);
	void ChangeVisibility(AEqquipment* InItem, EItemMode NewMode);
	AEqquipment* SpawnItem(UClass* Class, FTransform const& Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());
	AEqquipment* SpawnItemDeferred(UClass* Class, FTransform const& Transform, AActor* NewOwner = nullptr, APawn* NewInstigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined);
};
