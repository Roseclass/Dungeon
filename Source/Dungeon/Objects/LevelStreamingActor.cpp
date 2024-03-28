#include "Objects/LevelStreamingActor.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "Components/QuestComponent.h"
#include "Components/ConfirmPopupComponent.h"

#include "Objects/Quest.h"

ALevelStreamingActor::ALevelStreamingActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent(this, &InteractCollision, "InteractCollision");
}

void ALevelStreamingActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelStreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevelStreamingActor::Interact(ADungeonPlayerController* InPlayer) 
{
	CheckFalse(bActive);

	// if host?
	if (HasAuthority() && InPlayer->IsLocalController())
	{
		// send popup every clients
		UConfirmPopupComponent* confirm = CHelpers::GetComponent<UConfirmPopupComponent>(InPlayer);

		TFunction<bool()> func = [InPlayer,this]()->bool
		{
			UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(InPlayer->GetPawn());
			if (!quest)return 0;
			LoadLevel(quest->GetQuest()->GetStreamingLevelName());
			return 1;
		};		

		if (confirm)confirm->SendPopupAllPlayers("ebs?", func);
		else CLog::Print("nullptr");
	}

}

void ALevelStreamingActor::PreInteractEvent(ADungeonPlayerController* InPlayer) 
{

}

void ALevelStreamingActor::InInteractEvent(ADungeonPlayerController* InPlayer) 
{

}

void ALevelStreamingActor::PostInteractEvent(ADungeonPlayerController* InPlayer) 
{

}

bool ALevelStreamingActor::IsInteractable() 
{
	return bActive;
}

void ALevelStreamingActor::LoadLevel(FStageData InData)
{
	FLatentActionInfo temp;
	temp.CallbackTarget = this;
	temp.ExecutionFunction = FName("LoadLevelFinished");
	temp.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), InData.StageName, 1, 0, temp);

	LoadLevelStarted();
}

void ALevelStreamingActor::LoadLevel(FName InLevelName)
{
	FLatentActionInfo temp;
	temp.CallbackTarget = this;
	temp.ExecutionFunction = FName("LoadLevelFinished");
	temp.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), InLevelName, 1, 0, temp);

	LoadLevelStarted();
}


void ALevelStreamingActor::Activate_Implementation()
{
	// execute when accept quest
	bActive = 1;

	// sequence is implemented in the blueprint
}

void ALevelStreamingActor::Deactivate_Implementation()
{
	// execute when quest end
	bActive = 0;

	// sequence is implemented in the blueprint
}
