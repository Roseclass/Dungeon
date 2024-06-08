#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/SkillComponent.h"
#include "SkillButton.generated.h"

/**
 * ��ų����, ��ųƮ�� ������ ���Ǵ� ��ư
 */

enum class ESkillTreeSkillState : uint8;

DECLARE_DELEGATE_OneParam(FSkillButtonClicked, USkillButton*);

UCLASS()
class DUNGEON_API USkillButton : public UButton
{
	GENERATED_BODY()
public:
	USkillButton();

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
	void OnButtonLocked();
	void OnButtonUnlocked();
	void OnButtonAcquired();
protected:
public:
	void Init(const FSkillData& InSkillData);
	void Update(ESkillTreeSkillState NewState);

	FORCEINLINE int32 GetSkillID() const { return SkillID; }
	FORCEINLINE FVector2D GetPosition() const { return Position; }
};
