#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

/**
 * �������� ��ġ(ü��..)���� �����ϰ�
 * ������ ����, ��ų ��� ���θ� ������
 * 
 * canmove,canuse ���� Ŭ���̾�Ʈ�� �Է¿� �����ϴ� ������ ���÷� ����
 * canmove,canuse�� ��� ���ŵǳ���? >> ��Ÿ�ֿ� ��Ƽ���̷� �ٿ������ϴ�
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()
	//property
private:
	bool bCanUse = 1;
	bool bCanMove = 1;
protected:
public:

	//engine
private:
protected:
public:
	UStatusComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//function
private:
protected:
public:
	void SetUse();
	void SetCannotUse();

	void SetMove();
	void SetStop();

	FORCEINLINE bool CanUse() {return bCanUse;}
	FORCEINLINE bool CanMove() {return bCanMove;}
};
