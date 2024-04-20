#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"

/*
* 
*/

UENUM()
enum class EStateType : uint8
{
	Idle, Skill, Hit, KnockBack, KnockDown, Dead, Sequence, Max
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FStateTypeChanged, EStateType, EStateType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	UPROPERTY(Replicated)EStateType PrevType;
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_Type")EStateType Type;
	bool bIsInvincible;
protected:
public:
	FStateTypeChanged OnStateTypeChanged;

	//function
private:
	UFUNCTION()void OnRep_Type();
	void ChangeType(EStateType InNewType);
protected:
public:
	void SetIdleMode();
	void SetSkillMode();
	void SetHitMode();
	void SetKnockBackMode();
	void SetKnockDownMode();
	void SetDeadMode(); 
	void SetSequenceMode();

	FORCEINLINE bool IsIdleMode() { return Type == EStateType::Idle; }
	FORCEINLINE bool IsActionMode() { return Type == EStateType::Skill; }
	FORCEINLINE bool IsHitMode() { return Type == EStateType::Hit; }
	FORCEINLINE bool IsDeadMode() { return Type == EStateType::Dead; }
	FORCEINLINE bool IsSequenceMode() { return Type == EStateType::Sequence; }
	FORCEINLINE bool IsHitable() { return !IsHitMode() || !bIsInvincible; }
};
