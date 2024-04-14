#include "Widgets/UW_ConfirmPopup.h"
#include "Global.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"

void UUW_ConfirmPopup::NativeConstruct()
{
	Super::NativeConstruct();

	ConfirmButton->OnClicked.AddDynamic(this, &UUW_ConfirmPopup::OnConfirmButtonClicked);
	CancelButton->OnClicked.AddDynamic(this, &UUW_ConfirmPopup::OnCancelButtonClicked);
	Activate();
}

void UUW_ConfirmPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bActive)
	{
		if (!bCancel || !bConfirm)
		{
			float time = GetWorld()->GetTimeSeconds() - ActivateTime;
			time = time / TimeLimit;
			if (time > 1)
			{
				RemoveFromParent();
				return;
			}
			TimerBar->SetPercent(time);
		}
		TickFunction(InDeltaTime);
	}
}

void UUW_ConfirmPopup::RemoveFromParent()
{
	// get player controller
	ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (!pc)
	{
		CLog::Print("UUW_ConfirmPopup::Activate is nullptr", -1, 10, FColor::Red);
		return;
	}
	// restore input
	pc->SetGameAndUIMode();

	Super::RemoveFromParent();
}

void UUW_ConfirmPopup::OnConfirmButtonClicked()
{
	OnConfirm.ExecuteIfBound();
}

void UUW_ConfirmPopup::OnCancelButtonClicked()
{
	OnCancel.ExecuteIfBound();
}

void UUW_ConfirmPopup::TickFunction(float DeltaTime)
{
	for (auto& i : TickSigns)
	{
		if (i.Value.Key > 1)continue;
		UImage* sign = i.Key;
		UMaterialInstanceDynamic* mat = sign->GetDynamicMaterial();
		if (i.Value.Key < 0.09 && i.Value.Value)DeltaTime *= 0.045;
		i.Value.Key += DeltaTime;
		mat->SetScalarParameterValue(FName("Limit"), i.Value.Key);
	}
}

void UUW_ConfirmPopup::SetWaitSign(UImage* InImage)
{
	// set image to wait
	InImage->SetBrushFromMaterial(WaitSign);
}

void UUW_ConfirmPopup::SetConfirmSign(UImage* InImage)
{
	// set image to confirm
	InImage->SetBrushFromMaterial(ConfirmSign);
	auto data = TickSigns.FindOrAdd(InImage);
	data.Key = 0;data.Value = 1;
}

void UUW_ConfirmPopup::SetCancelSign(UImage* InImage) 
{
	// set image to cancel
	InImage->SetBrushFromMaterial(CancelSign);
	auto data = TickSigns.FindOrAdd(InImage);
	data.Key = 0;data.Value = 0;

	// disable buttons
	ConfirmButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	CancelButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	bCancel = 1;

	// reserve cancel sequence
	FTimerHandle WaitHandle;
	float WaitTime = 1;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		// bind Cancel Event
		FWidgetAnimationDynamicEvent finished;
		finished.BindUFunction(this, "RemoveFromParent");
		BindToAnimationFinished(Cancel, finished);

		// play cancel animation
		PlayAnimationForward(Cancel);
	}), WaitTime, false);
}

void UUW_ConfirmPopup::Init(FString InDescription, TArray<AActor*> NewPortraitActors, float NewTimeLimit)
{
	Description->SetText(FText::FromString(InDescription));
	TimeLimit = NewTimeLimit;
	PortraitActors = NewPortraitActors;
	int32 NumOfPlayers = NewPortraitActors.Num();
	if (NumOfPlayers < 4)
	{
		Portrait3->SetVisibility(ESlateVisibility::Collapsed);
		Sign3->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (NumOfPlayers < 3)
	{
		Portrait2->SetVisibility(ESlateVisibility::Collapsed);
		Sign2->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (NumOfPlayers < 2)
	{
		Portrait1->SetVisibility(ESlateVisibility::Collapsed);
		Sign1->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (NumOfPlayers < 1)
	{
		CLog::Print("UUW_ConfirmPopup::Init NumOfPlayers < 1", -1, 10, FColor::Red);
		return;
	}

	SetWaitSign(Sign0);SetWaitSign(Sign1);
	SetWaitSign(Sign2);SetWaitSign(Sign3);

	CreatePortrait(PortraitActors);
}

void UUW_ConfirmPopup::Activate()
{
	// get player controller
	ADungeonPlayerController* pc = Cast<ADungeonPlayerController>(GetOwningPlayer());
	if (!pc)
	{
		CLog::Print("UUW_ConfirmPopup::Activate is nullptr", -1, 10, FColor::Red);
		return;
	}

	// ignore input
	pc->SetUIOnlyMode();

	// shut down other popup widget
	pc->HideAddtiveWidget();

	// stop character movement
	pc->StopPawnImmediately();

	// set variables
	bActive = 1;
	ActivateTime = GetWorld()->GetTimeSeconds();

	// play opening animation
	PlayAnimationForward(Reveal);
}

void UUW_ConfirmPopup::UpdateSign(AActor* PortraitActor, bool NewState)
{
	UImage* sign = nullptr;
	int32 idx = PortraitActors.Find(PortraitActor);

	switch (idx)
	{
	case 0:sign = Sign0;break;
	case 1:sign = Sign1;break;
	case 2:sign = Sign2;break;
	case 3:sign = Sign3;break;
	default:break;
	}

	if (!sign)
	{
		CLog::Print("UUW_ConfirmPopup::UpdateSign sign is nullptr", -1, 10, FColor::Red);
		return;
	}

	NewState ? SetConfirmSign(sign) : SetCancelSign(sign);
}

void UUW_ConfirmPopup::ConfirmedSequence()
{
	// disable buttons
	ConfirmButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	CancelButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	bConfirm = 1;

	// reserve cancel sequence
	FTimerHandle WaitHandle;
	float WaitTime = 1;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		// play reverse Reveal animation
		PlayAnimationReverse(Reveal);
	}), WaitTime, false);

	// reserve cancel sequence
	FTimerHandle RemoveHandle;
	WaitTime += Reveal->GetEndTime();
	GetWorld()->GetTimerManager().SetTimer(RemoveHandle, FTimerDelegate::CreateLambda([&]()
	{
		// delete widget
		RemoveFromParent();
	}), WaitTime, false);
}
