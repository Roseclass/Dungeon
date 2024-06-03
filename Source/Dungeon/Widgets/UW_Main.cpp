#include "Widgets/UW_Main.h"
#include "Global.h"

#include "Characters/DungeonCharacterBase.h"

#include "Widgets/UW_Chat.h"
#include "Widgets/UW_Orb.h"

void UUW_Main::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UUW_Main::Init(APawn* NewPawn)
{

}

void UUW_Main::HealthChanged(float NewValue)
{
	HealthOrb->SetPercent(NewValue);
}

void UUW_Main::MaxHealthChanged(float NewValue)
{
	HealthOrb->SetMax(NewValue);
}

void UUW_Main::ManaChanged(float NewValue)
{
	ManaOrb->SetPercent(NewValue);
}

void UUW_Main::MaxManaChanged(float NewValue)
{
	ManaOrb->SetMax(NewValue);
}

void UUW_Main::OnChat()
{
	Chat->OnChat();
}

void UUW_Main::OnMessageUpdated(const FText& Text, const FLinearColor& Color)
{
	Chat->OnMessageUpdated(Text, Color);
}

void UUW_Main::OnDead()
{
	HealthOrb->Dead();
	ManaOrb->Dead();
}
