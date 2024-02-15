#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
 * ������ ���� �� ����
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
	// �巡�� ��� - �̵� �� ����, ����
	// ��Ŭ�� - ���� �� ����
	// �������� ���� Ȯ��
	// ������ ���� ����, ȿ�� ����
	// ��ӽ� ��޿� �´� ����Ʈ
	// ���ڹ��� �κ��丮 ��3��
	// ������ �� 2��
	//
};
