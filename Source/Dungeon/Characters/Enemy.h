#pragma once

#include "CoreMinimal.h"
#include "Characters/DungeonCharacterBase.h"
#include "Enemy.generated.h"

/**
 * enemy character's basic class
 *
 */

class UBehaviorTree;
class USceneComponent;
class UWidgetComponent;
class UStaticMeshComponent;
class USkillComponent;
class UMontageComponent;
class UStatusComponent;
class UStateComponent;
class UInventoryComponent;
class UItemObject;
class ASkillActor;
class UUW_HealthBar;



UCLASS()
class DUNGEON_API AEnemy : public ADungeonCharacterBase
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

	//property
private:
	UPROPERTY(EditAnywhere, Category = "HealthBarInfo", meta = (AllowPrivateAccess = true))
		FText Name = FText::FromString("Monster");

	UPROPERTY(EditAnywhere, Category = "HealthBarInfo", meta = (AllowPrivateAccess = true))
		uint8 Level = 1;
protected:
	UUW_HealthBar* HealthBarWidget;

	//scene
	UPROPERTY(VisibleAnywhere)
		USceneComponent* HealthBarRoot;

	UPROPERTY(VisibleAnywhere)
		UWidgetComponent* HealthBar;

	//actor
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		UBehaviorTree* BehaviorTree;

public:

	//function
private:
	UFUNCTION()void ChangeHealthBarPercent(float NewPercent);
	UFUNCTION(NetMulticast, Reliable)void Multicast_UseSkill(int32 InIndex);
protected:
	virtual void Init()override;
public:	

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }

	//Skill
	virtual void UseSkill(int32 Idx)override;
};
