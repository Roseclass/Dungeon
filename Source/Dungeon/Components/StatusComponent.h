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

DECLARE_DELEGATE_OneParam(FStatusCurrentHealthChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UStatusComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = "OnRep_Level")uint8 Level;
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = "OnRep_MaxHealth")float MaxHealth;
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = "OnRep_CurrentHealth")float CurrentHealth;
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = "OnRep_MaxMana")float MaxMana;
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = "OnRep_CurrentMana")float CurrentMana;
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = "OnRep_Damage")float Damage;

	bool bCanUse = 1;
	bool bCanMove = 1;
protected:
public:
	FStatusCurrentHealthChanged OnCurrentHealthChanged;

	//function
private:
	UFUNCTION()void OnRep_Level();
	UFUNCTION()void OnRep_MaxHealth();
	UFUNCTION()void OnRep_CurrentHealth();
	UFUNCTION()void OnRep_MaxMana();
	UFUNCTION()void OnRep_CurrentMana();
	UFUNCTION()void OnRep_Damage();

	UFUNCTION()void UpdateStatus();
	void UpdateMaxHealth();
	void UpdateMaxMana();
	void UpdateDamage();
protected:
public:
	void AdjustCurrentHealth(float InValue);

	void SetUse();
	void SetCannotUse();

	void SetMove();
	void SetStop();

	FORCEINLINE bool CanUse() const {return bCanUse;}
	FORCEINLINE bool CanMove() const {return bCanMove;}
	FORCEINLINE float GetMaxHealth() const {return MaxHealth;}
	FORCEINLINE float GetCurrentHealth() const {return CurrentHealth;}
	FORCEINLINE uint8 GetLevel() const {return Level;}
};
