#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "DungeonCharacter.generated.h"

/**
 * 캐릭터 기본 베이스(플레이어,몬스터)
 */

class UCameraComponent;
class USpringArmComponent;
class USkillComponent;
class UMontageComponent;
class UStatusComponent;

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
		UMontageComponent* Montage;

	UPROPERTY(VisibleDefaultsOnly)
		UStatusComponent* Status;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		uint8 TeamID = 1;
protected:
public:

	//function
private:
	void UseSkill(FSkillData* InData);
protected:
public:
	//Skill
	void UseSkill(int32 Idx);
	void UseLeft();
	void UseRight();
	void UseQ();
	void UseW();
	void UseE();
	void UseR();
	bool CanUse();
	bool CanMove();

	//for notify
	UFUNCTION(BlueprintCallable)void SetUse();
	UFUNCTION(BlueprintCallable)void SetMove();
	UFUNCTION(BlueprintCallable)void UnsetSkill();
	UFUNCTION(BlueprintCallable)void SpawnProjectile();

	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

