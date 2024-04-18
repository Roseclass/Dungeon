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

DECLARE_MULTICAST_DELEGATE_OneParam(FStatusChanged, float);

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
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_Level")uint8 Level;

	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_MaxHealth")float MaxHealth;
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_CurrentHealth")float CurrentHealth;
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_HealthRegen")float HealthRegen;

	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_MaxMana")float MaxMana;
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_CurrentMana")float CurrentMana;
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_ManaRegen")float ManaRegen;

	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_Damage")float Damage;

	// updated only in client
	float AmountOfHealthRegen_Client;
	float AmountOfManaRegen_Client;

	// updated only in sever
	float AmountOfHealthRegen_Server;
	float AmountOfManaRegen_Server;

	bool bCanUse = 1;
protected:
public:
	FStatusChanged OnMaxHealthChanged;
	FStatusChanged OnCurrentHealthChanged;
	FStatusChanged OnHealthRegenChanged;

	FStatusChanged OnMaxManaChanged;
	FStatusChanged OnCurrentManaChanged;
	FStatusChanged OnManaRegenChanged;

	//function
private:
	UFUNCTION()void OnRep_Level();
	UFUNCTION()void OnRep_MaxHealth();
	UFUNCTION()void OnRep_CurrentHealth();
	UFUNCTION()void OnRep_HealthRegen();
	UFUNCTION()void OnRep_MaxMana();
	UFUNCTION()void OnRep_CurrentMana();
	UFUNCTION()void OnRep_ManaRegen();
	UFUNCTION()void OnRep_Damage();

	UFUNCTION()void UpdateStatus();
	void UpdateMaxHealth();
	void UpdateMaxMana();
	void UpdateDamage();

	UFUNCTION()void UpdateRegen();
	void UpdateHealthRegen();
	void UpdateManaRegen();
protected:
public:
	void AdjustCurrentHealth(float InValue);
	void AdjustCurrentMana(float InValue);

	void SetUse();
	void SetCannotUse();

	void SetMove();
	void SetStop();
	
	void Update();

	FORCEINLINE bool CanUse() const {return bCanUse;}
	bool CanMove() const;

	FORCEINLINE uint8 GetLevel() const {return Level;}

	FORCEINLINE float GetMaxHealth() const {return MaxHealth;}
	FORCEINLINE float GetCurrentHealth() const {return CurrentHealth;}
	FORCEINLINE float GetHealthRegen() const {return HealthRegen;}

	FORCEINLINE float GetMaxMana() const {return MaxMana;}
	FORCEINLINE float GetCurrentMana() const {return CurrentMana;}
	FORCEINLINE float GetManaRegen() const {return ManaRegen;}

	FORCEINLINE float GetCurrentHealth_Client() const { return CurrentHealth + AmountOfHealthRegen_Client; }
	FORCEINLINE float GetCurrentMana_Client() const { return CurrentMana + AmountOfManaRegen_Client; }
	FORCEINLINE float GetCurrentHealth_Server() const { return CurrentHealth + AmountOfHealthRegen_Server; }
	FORCEINLINE float GetCurrentMana_Server() const { return CurrentMana + AmountOfManaRegen_Server; }

	FORCEINLINE float GetDamage() const {return Damage;}
};
