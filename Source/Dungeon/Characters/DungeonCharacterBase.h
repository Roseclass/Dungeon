#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "DungeonCharacterBase.generated.h"

class UWidgetComponent;
class USkillComponent;
class UMontageComponent;
class UInventoryComponent;
class UItemObject;
class ASkillActor;
class AWeapon;
class UUW_HealthBar;

class UAbilitySystemComponent;
class UAttributeSetBase;
class UGameplayAbility;

enum class EStateType : uint8;
enum class EReactionType : uint8;

UCLASS()
class DUNGEON_API ADungeonCharacterBase : public ACharacter, public IGenericTeamAgentInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADungeonCharacterBase();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// for Gameplay Ability System
protected:
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "HealthBarInfo", ReplicatedUsing = "OnRep_Name")
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
		UAttributeSetBase* AttributeSet;

	UPROPERTY(VisibleDefaultsOnly)
		UMontageComponent* Montage;

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

	UFUNCTION()void OnRep_Name();
protected:
	virtual void Init();
	
	UFUNCTION(Reliable, Server)void Server_SetName(const FText& NewName);
public:
	//HealthBar
	virtual void HideHealthBar();
	virtual void RevealHealthBar();

	//Skill
	virtual void UseSkill(int32 Idx) {};

	//Inventory
	virtual void TryAddItem(AWeapon* InObject) {};

	//getter
	virtual bool CanUse();
	virtual bool CanMove();

	//for notify
	UFUNCTION(BlueprintCallable)virtual void OnCollision();
	UFUNCTION(BlueprintCallable)virtual void OffCollision();
	UFUNCTION(BlueprintCallable)virtual void ResetHitActors();
};
