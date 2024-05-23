#include "Widgets/UW_SkillInfo.h"
#include "Global.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "MediaPlayer.h"

#include "Objects/SkillActor.h"

void UUW_SkillInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (MediaPlayer)
	{
		MediaPlayer->OnMediaOpened.AddDynamic(this, &UUW_SkillInfo::OnMediaOpened);
		MediaPlayer->OnPlaybackResumed.AddDynamic(this, &UUW_SkillInfo::OnMediaPlay);
		MediaPlayer->OnPlaybackSuspended.AddDynamic(this, &UUW_SkillInfo::OnMediaPause);
	}

	Preview->SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillInfo::OnMediaOpened(FString OpenedUrl)
{
	MediaPlayer->Rewind();
	MediaPlayer->Play();
}

void UUW_SkillInfo::OnMediaPlay()
{
	Preview->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUW_SkillInfo::OnMediaPause()
{
	Preview->SetVisibility(ESlateVisibility::Hidden);
}

void UUW_SkillInfo::On(ASkillActor* InSkillActor)
{
	const FSkillData* data = InSkillActor->GetSkillData();
	if (MediaPlayer)MediaPlayer->OpenSource(data->PreviewSource);
	Name->SetText(data->Name);
	Description->SetText(data->Description);

	SetVisibility(ESlateVisibility::Visible);
}

void UUW_SkillInfo::Off()
{
	SetVisibility(ESlateVisibility::Hidden);
	if (MediaPlayer)MediaPlayer->Pause();
}
