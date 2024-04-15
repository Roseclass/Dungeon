#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

/**
 * 여러가지 수치(체력..)들을 관리하고
 * 움직임 여부, 스킬 사용 여부를 관리함
 * 
 * canmove,canuse 같은 클라이언트의 입력에 관여하는 변수는 로컬로 갱신
 * canmove,canuse는 어디서 갱신되나요? >> 몽타주에 노티파이로 붙여놨습니다
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
