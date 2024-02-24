#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "DungeonCharacter.generated.h"

/**
 * ĳ���� �⺻ ���̽�(�÷��̾�,����)
 * TODO::�и��� �ʿ��ҵ�?
 */

class UCameraComponent;
class USpringArmComponent;
class USkillComponent;
class USkillTreeComponent;
class UMontageComponent;
class UStatusComponent;
class UInventoryComponent;
class UItemObject;
class ASkillActor;

struct FSkillData;

UCLASS(Blueprintable)
class ADungeonCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ADungeonCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual FGenericTeamId GetGenericTeamId() const;

	//property
private:
	//scene
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	//actor
	UPROPERTY(VisibleDefaultsOnly)
		USkillComponent* Skill;

	UPROPERTY(VisibleDefaultsOnly)
		USkillTreeComponent* SkillTree;

	UPROPERTY(VisibleDefaultsOnly)
		UMontageComponent* Montage;

	UPROPERTY(VisibleDefaultsOnly)
		UStatusComponent* Status;

	UPROPERTY(VisibleDefaultsOnly)
		UInventoryComponent* Inventory;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		uint8 TeamID = 1;
protected:
public:

	//function
private:
	void Init();
	void OffAllWidget();
protected:
public:
	void InitClientWidget();
	//Skill
	void UseSkill(int32 Idx);
	void UseLeft();
	void UseRight();
	void UseQ();
	void UseW();
	void UseE();
	void UseR();
	void ChangeQuickSlotData(int32 Index, ASkillActor* InSkillActor);

	//Inventory
	bool TryAddItem(UItemObject* InObject);

	//getter
	bool CanUse();
	bool CanMove();

	//for notify
	UFUNCTION(BlueprintCallable)void SetUse();
	UFUNCTION(BlueprintCallable)void SetCannotUse();
	UFUNCTION(BlueprintCallable)void SetMove();
	UFUNCTION(BlueprintCallable)void SetStop();
	UFUNCTION(BlueprintCallable)void UnsetSkill();
	UFUNCTION(BlueprintCallable)void SpawnProjectile();
	UFUNCTION(BlueprintCallable)void OnCollision();
	UFUNCTION(BlueprintCallable)void OffCollision();
	UFUNCTION(BlueprintCallable)void ResetHittedActors();

	//widget Toggle
	void ToggleSkillTree();
	void ToggleInventory();

	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

