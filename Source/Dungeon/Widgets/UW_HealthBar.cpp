#include "Widgets/UW_HealthBar.h"
#include "Global.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UUW_HealthBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UUW_HealthBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_HealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Max > 0)// and player is alive
	{
		float cur = HealthBar->Percent;
		cur += InDeltaTime * Regen / Max;
		if (cur > 1)cur = 1;
		SetPercent(cur);
	}

	if (DelayPercent > HealthBar->Percent)
	{
		DelayPercent = UKismetMathLibrary::FInterpTo(DelayPercent, HealthBar->Percent, InDeltaTime, DelaySpeed);
		Delay->SetRenderScale(FVector2D(DelayPercent, 1));
	}
	else if(DelayPercent < HealthBar->Percent)
	{ 
		DelayPercent = HealthBar->Percent; 
		Delay->SetRenderScale(FVector2D(DelayPercent, 1));
	}
}

void UUW_HealthBar::HitEffectStartEvent()
{
	// adjust effect translation to fit end of progress bar
	float x = HealthBar->GetCachedGeometry().GetLocalSize().X;
	x *= HealthBar->Percent;
	Effect->SetRenderTranslation(FVector2D(x, 0));
}

void UUW_HealthBar::Init(FText NewName, uint8 NewLevel)
{
	// set new name, level
	Name->SetText(NewName);
	FText level = FText::FromString(FString::FromInt(NewLevel));
	Level->SetText(level);

	// bind hit effect start event;
	FWidgetAnimationDynamicEvent start;
	start.BindUFunction(this, "HitEffectStartEvent");
	BindToAnimationStarted(HitEffect, start);
}

void UUW_HealthBar::SetMax(float NewMax)
{
	Max = NewMax;
}

void UUW_HealthBar::SetPercent(float NewPercent)
{
	// set new value to progress bar
	// change percent text
	float prevPercent = HealthBar->Percent;

	HealthBar->SetPercent(NewPercent);

	FText txt = FText::FromString(FString::FromInt(int32(NewPercent * 100)) + "%");
	Percent->SetText(txt);

	// Effect
	// if its lower, adjust effect and play
	// or else just do nothing
	CheckFalse(NewPercent < prevPercent);
	PlayAnimationForward(HitEffect);
}

void UUW_HealthBar::SetRegen(float NewRegen)
{
	Regen = NewRegen;
}

void UUW_HealthBar::SetPlayerType()
{
	HealthBar->FillColorAndOpacity = FLinearColor::Green;
}

void UUW_HealthBar::SetOtherPlayerType()
{
	HealthBar->FillColorAndOpacity = FLinearColor::Blue;
}

void UUW_HealthBar::SetMonsterType()
{
	HealthBar->FillColorAndOpacity = FLinearColor::Red;
}

void UUW_HealthBar::SetEliteMonsterType()
{
	HealthBar->FillColorAndOpacity = FLinearColor::Red;
}
