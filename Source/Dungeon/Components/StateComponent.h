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
	Idle, Skill, Hitted, Dead, Max
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

	//property
private:
	EStateType Type;
	bool bIsInvincible;
protected:
public:
	FStateTypeChanged OnStateTypeChanged;

	//function
private:
	void ChangeType(EStateType InNewType);
protected:
public:
	void SetIdleMode();
	void SetSkillMode();
	void SetHittedMode();
	void SetDeadMode(); 

	FORCEINLINE bool IsIdleMode() { return Type == EStateType::Idle; }
	FORCEINLINE bool IsActionMode() { return Type == EStateType::Skill; }
	FORCEINLINE bool IsHittedMode() { return Type == EStateType::Hitted; }
	FORCEINLINE bool IsDeadMode() { return Type == EStateType::Dead; }
	FORCEINLINE bool IsHitable() { return !IsHittedMode() || !bIsInvincible; }
};
