#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "SkillButton.generated.h"

/**
 * 
 */

class ASkillActor;

DECLARE_DELEGATE_OneParam(FSkillButtonClicked, USkillButton*);

UCLASS()
class DUNGEON_API USkillButton : public UButton
{
	GENERATED_BODY()

	//property
private:
	ASkillActor* SkillActor;
	FVector2D Position;
protected:
public:
	FSkillButtonClicked OnSkillButtonClicked;
	//function
private:
	UFUNCTION()void OnButtonClicked();
	UFUNCTION()void OnButtonLocked();
	UFUNCTION()void OnButtonUnlocked();
	UFUNCTION()void OnButtonAcquired();
protected:
public:
	void Init(ASkillActor* InSkillActor);

	FORCEINLINE ASkillActor* GetSkillActor() const { return SkillActor; }
	FORCEINLINE FVector2D GetPosition() const { return Position; }

};
