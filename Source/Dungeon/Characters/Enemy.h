#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
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
class DUNGEON_API AEnemy : public ACharacter, public IGenericTeamAgentInterface
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

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		UBehaviorTree* BehaviorTree;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		uint8 TeamID = 2;

	UPROPERTY(EditAnywhere, Category = "HealthBarInfo", meta = (AllowPrivateAccess = true))
		FText Name = FText::FromString("Monster");

	UPROPERTY(EditAnywhere, Category = "HealthBarInfo", meta = (AllowPrivateAccess = true))
		uint8 Level = 1;

protected:
public:

	//function
private:
	UFUNCTION()void ChangeHealthBarPercent(float NewPercent);
protected:
public:	
	void UseSkill(int32 InIndex);

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }
};
