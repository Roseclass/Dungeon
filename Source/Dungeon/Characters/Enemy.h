#pragma once

#include "CoreMinimal.h"
#include "Characters/DungeonCharacterBase.h"
#include "Interfaces/IQuestObjective.h"
#include "Enemy.generated.h"

/**
 * enemy character's basic class
 *
 */

class UBehaviorTree;
class USceneComponent;
class UStaticMeshComponent;
class USkillComponent;
class UMontageComponent;
class UStatusComponent;
class UStateComponent;
class UInventoryComponent;
class UDamageTextComponent;
class UHitEffectComponent;
class ULootComponent;
class UItemObject;
class ASkillActor;

UCLASS()
class DUNGEON_API AEnemy : public ADungeonCharacterBase, public IIQuestObjective
{
	GENERATED_BODY()
public:
	AEnemy();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual FGenericTeamId GetGenericTeamId() const;

	//IIQuestObjective
	virtual void CompleteCondition()override;

	//property
private:
protected:
	//actor
	UPROPERTY(EditDefaultsOnly)
		UHitEffectComponent* HitEffect;

	UPROPERTY(EditDefaultsOnly)
		ULootComponent* Loot;

	UPROPERTY(EditDefaultsOnly)
		UDamageTextComponent* DamageText;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		UBehaviorTree* BehaviorTree;

public:

	//function
private:
	UFUNCTION(NetMulticast, Reliable)void Multicast_UseSkill(int32 InIndex);
	UFUNCTION(NetMulticast, Reliable)void Multicast_TakeDamage(float InDamage, bool IsCritical);
	void SequenceStart();
	UFUNCTION(BlueprintCallable)void SequenceEnd();
protected:
	virtual void Init()override;
public:	
	UFUNCTION(BlueprintNativeEvent)void PlaySequence(int32 InIndex);
	void PlaySequence_Implementation(int32 InIndex);	

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }

	//Skill
	virtual void UseSkill(int32 Idx)override;

	//state
	virtual void SetDeadMode()override;

	//Loot
	void GenerateLootItems();

	//for notify
	UFUNCTION(BlueprintCallable)virtual void SpawnWarningSign(int32 InIndex);
};
