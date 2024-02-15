#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
 * 아이템 장착 및 보관
 */

class AWeapon;

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
	AWeapon* CurrentWeapon;

protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWeapon> DefaultWeapon;
public:

	//function
private:
	void InitDefault();
protected:
public:
	void OnCollision();
	void OffCollision();
	void ResetHittedActors();

	//
	// 드래그 드롭 - 이동 및 장착, 해제
	// 우클릭 - 장착 및 해제
	// 장착전에 조건 확인
	// 장착시 외형 변경, 효과 적용
	// 드롭시 등급에 맞는 이펙트
	// 격자무늬 인벤토리 탭3개
	// 프리셋 탭 2개
	//
};
