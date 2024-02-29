#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.generated.h"

/*
* 아이템 관리 담당, 월드마다 하나씩 배치
* 서버 컨트롤러를 오너로 삼고 아이템들의 상태를 동기화
*/

class AWeapon;
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
	UFUNCTION(Reliable, Server)void Server_SetItemLocation(AWeapon* InItem, const FVector& NewLocation, bool bSweep, const FHitResult& OutSweepHitResult, ETeleportType Teleport);
	UFUNCTION(Reliable, Server)void Server_SetItemRotation(AWeapon* InItem, const FRotator& NewRotation, ETeleportType Teleport);
	UFUNCTION(Reliable, Server)void Server_AttachItemToComponent(AWeapon* InItem, USceneComponent* Parent, EAttachmentRule Location, EAttachmentRule Rotation, EAttachmentRule Scale, bool bWeldSimulatedBodies, const FName& SocketName);
	UFUNCTION(Reliable, Server)void Server_ChangeVisibility(AWeapon* InItem, EItemMode NewMode);
protected:
public:
	void SetItemLocation(AWeapon* InItem, const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	void SetItemRotation(AWeapon* InItem, FRotator NewRotation, ETeleportType Teleport = ETeleportType::None);
	void AttachItemToComponent(AWeapon* InItem, USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName SocketName = NAME_None);
	void ChangeVisibility(AWeapon* InItem, EItemMode NewMode);
	AWeapon* SpawnItem(UClass* Class, FTransform const& Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());
	AWeapon* SpawnItemDeferred(UClass* Class, FTransform const& Transform, AActor* NewOwner = nullptr, APawn* NewInstigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined);
};
