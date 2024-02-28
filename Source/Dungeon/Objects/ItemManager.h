#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.generated.h"

/*
* 아이템 관리 담당, 월드마다 하나씩 배치
* 서버 컨트롤러를 오너로 삼고 아이템들의 상태를 동기화
*/

class AWeapon;

enum class EItemMode : uint8;

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
	UFUNCTION(NetMulticast, Reliable)void test();
	UFUNCTION(Reliable, Server)void stest();
	UFUNCTION(Reliable, Server)void Server_ChangeVisibility(AWeapon* InItem, EItemMode NewMode);
protected:
public:
	void ChangeVisibility(AWeapon* InItem, EItemMode NewMode);
};
