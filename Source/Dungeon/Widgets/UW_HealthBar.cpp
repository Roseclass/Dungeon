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

	if (DelayPercent > HealthBar->Percent)
	{
		CLog::Print(DelayPercent, -1, 10, FColor::Black);
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
