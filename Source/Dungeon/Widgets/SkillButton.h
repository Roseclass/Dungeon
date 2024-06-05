#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/SkillComponent.h"
#include "SkillButton.generated.h"

/**
 * 스킬슬롯, 스킬트리 위젯에 사용되는 버튼
 */

DECLARE_DELEGATE_OneParam(FSkillButtonClicked, USkillButton*);

UCLASS()
class DUNGEON_API USkillButton : public UButton
{
	GENERATED_BODY()

	//property
private:
	int32 SkillID;
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
	void Init(const FSkillData& InSkillData);

	FORCEINLINE int32 GetSkillID() const { return SkillID; }
	FORCEINLINE FVector2D GetPosition() const { return Position; }
};
