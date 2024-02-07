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

UCLASS(Blueprintable)
class ADungeonCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
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

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
		uint8 TeamID = 1;
protected:
public:

	//engine
private:
protected:
public:
	ADungeonCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual FGenericTeamId GetGenericTeamId() const;

	//function
private:
protected:
public:
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

