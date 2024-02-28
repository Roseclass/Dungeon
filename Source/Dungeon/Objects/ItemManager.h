#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.generated.h"

/*
* ������ ���� ���, ���帶�� �ϳ��� ��ġ
* ���� ��Ʈ�ѷ��� ���ʷ� ��� �����۵��� ���¸� ����ȭ
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
