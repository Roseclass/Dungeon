#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "DungeonCharacterBase.generated.h"

class UWidgetComponent;
class USkillComponent;
class UMontageComponent;
class UStatusComponent;
class UStateComponent;
class UInventoryComponent;
class UItemObject;
class ASkillActor;
class AWeapon;
class UUW_HealthBar;

enum class EStateType : uint8;
enum class EReactionType : uint8;

UCLASS()
class DUNGEON_API ADungeonCharacterBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ADungeonCharacterBase();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "HealthBarInfo")
		FText Name = FText::FromString("Monster");
	UUW_HealthBar* HealthBarWidget;

	//scene
	UPROPERTY(VisibleAnywhere)
		USceneComponent* HealthBarRoot;

	UPROPERTY(VisibleAnywhere)
		UWidgetComponent* HealthBar;

	//actor
	UPROPERTY(VisibleDefaultsOnly)
		USkillComponent* Skill;

	UPROPERTY(VisibleDefaultsOnly)
		UMontageComponent* Montage;

	UPROPERTY(VisibleDefaultsOnly)
		UStateComponent* State;

	UPROPERTY(VisibleDefaultsOnly)
		UStatusComponent* Status;

	UPROPERTY(VisibleDefaultsOnly)
		UInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		uint8 TeamID = 1;
public:

	//function
private:
	virtual void HitReaction_None();
	virtual void HitReaction_Normal();
	virtual void HitReaction_KnockBack();
	virtual void HitReaction_KnockDown();

	UFUNCTION()void ChangeHealthBarMax(float NewMax);
	UFUNCTION()void ChangeHealthBarPercent(float NewPercent);
	UFUNCTION()void ChangeHealthBarRegen(float NewRegen);
protected:
	virtual void Init();
	
	//State
	UFUNCTION()virtual void ChangeState(EStateType PrevType, EStateType NewType);
public:
	//Skill
	virtual void UseSkill(int32 Idx) {};

	//Inventory
	virtual void TryAddItem(AWeapon* InObject) {};

	//getter
	virtual bool CanUse();
	virtual bool CanMove();

	//for notify
	UFUNCTION(BlueprintCallable)virtual void SetUse();
	UFUNCTION(BlueprintCallable)virtual void SetCannotUse();
	UFUNCTION(BlueprintCallable)virtual void SetMove();
	UFUNCTION(BlueprintCallable)virtual void SetStop();
	UFUNCTION(BlueprintCallable)virtual void UnsetSkill();
	UFUNCTION(BlueprintCallable)virtual void SpawnProjectile();
	UFUNCTION(BlueprintCallable)virtual void OnCollision();
	UFUNCTION(BlueprintCallable)virtual void OffCollision();
	UFUNCTION(BlueprintCallable)virtual void ResetHitActors();
	UFUNCTION(BlueprintCallable)virtual void SetIdleMode();
	UFUNCTION(BlueprintCallable)virtual void SetSkillMode();
	UFUNCTION(BlueprintCallable)virtual void SetHitMode();
	UFUNCTION(BlueprintCallable)virtual void SetKnockBackMode();
	UFUNCTION(BlueprintCallable)virtual void SetKnockDownMode();
	UFUNCTION(BlueprintCallable)virtual void SetDeadMode();
};
