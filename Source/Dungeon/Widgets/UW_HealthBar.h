#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HealthBar.generated.h"

/**
 * 
 */

class UImage;
class UTextBlock;
class UProgressBar;
class UWidgetAnimation;

UCLASS()
class DUNGEON_API UUW_HealthBar : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized()override;
	virtual void NativeConstruct()override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

	//property
private:
	float DelayPercent;

	float Max;
	float Regen;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Effect;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Delay;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Name;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Level;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Percent;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* HitEffect;

	UPROPERTY(EditDefaultsOnly)
		float DelaySpeed = 1;
public:

	//function
private:
	UFUNCTION()void HitEffectStartEvent();
protected:
public:
	void Init(FText NewName, uint8 NewLevel);
	void SetName(FText NewName);
	void SetMax(float NewMax);
	void SetPercent(float NewPercent);
	void SetRegen(float NewRegen);

	void SetPlayerType();
	void SetOtherPlayerType();
	void SetMonsterType();
	void SetEliteMonsterType();
};
