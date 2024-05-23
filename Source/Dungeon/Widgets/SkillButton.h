#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "SkillButton.generated.h"

/**
 * 스킬슬롯, 스킬트리 위젯에 사용되는 버튼
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
	FSkillButtonClicked OnSkillButtonHovered;
	FSkillButtonClicked OnSkillButtonUnhovered;
	//function
private:
	UFUNCTION()void OnButtonClicked();
	UFUNCTION()void OnButtonHovered();
	UFUNCTION()void OnButtonUnhovered();
	UFUNCTION()void OnButtonLocked();
	UFUNCTION()void OnButtonUnlocked();
	UFUNCTION()void OnButtonAcquired();
protected:
public:
	void Init(ASkillActor* InSkillActor);

	FORCEINLINE ASkillActor* GetSkillActor() const { return SkillActor; }
	FORCEINLINE FVector2D GetPosition() const { return Position; }

	//호버시설명문
};
