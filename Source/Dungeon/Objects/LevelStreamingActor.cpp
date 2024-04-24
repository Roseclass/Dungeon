#include "Objects/LevelStreamingActor.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"
#include "Components/QuestComponent.h"
#include "Components/ConfirmPopupComponent.h"
#include "Components/TravelEffectComponent.h"

#include "Objects/Quest.h"

ALevelStreamingActor::ALevelStreamingActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent(this, &InteractCollision, "InteractCollision");
}

void ALevelStreamingActor::BeginPlay()
{
	Super::BeginPlay();

	if (DataTable)
	{
		TArray<FStageData*> datas;
		DataTable->GetAllRows<FStageData>("", datas);
		for (auto data : datas)
		{
			Datas.Add(FStageData());
			Datas[Datas.Num() - 1].StageList = data->StageList;
			Datas[Datas.Num() - 1].StageName = data->StageName;
			Datas[Datas.Num() - 1].StageDescription = data->StageDescription;
			Datas[Datas.Num() - 1].StartLocations = data->StartLocations;
		}
	}

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

		UQuestComponent* quest = CHelpers::GetComponent<UQuestComponent>(InPlayer->GetPawn());
		if (!quest)return;

		if (confirm)confirm->SendPopupAllPlayers(quest->GetQuest()->GetPopupMessage() , func);
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

	CurrentStage = InData.StageList;

	LoadLevelStarted();
}

void ALevelStreamingActor::LoadLevel(FName InLevelName)
{
	FLatentActionInfo temp;
	temp.CallbackTarget = this;
	temp.ExecutionFunction = FName("LoadLevelFinished");
	temp.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), InLevelName, 1, 0, temp);

	for(auto& i : Datas)
		if (i.StageName == InLevelName)
		{
			CurrentStage = i.StageList;
			break;
		}

	LoadLevelStarted();
}

void ALevelStreamingActor::LoadLevelStarted()
{
	// this is server

	// for multicast
	SetOwner(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0));

	// get all player characters
	TArray<AActor*> characters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), characters);

	// set values
	SequenceMap.Empty();
	for (auto i : characters)
	{
		APlayerCharacter* ch = Cast<APlayerCharacter>(i);
		if (!ch)continue;
		SequenceMap.Add(ch, 0);

		UTravelEffectComponent* travel = CHelpers::GetComponent<UTravelEffectComponent>(ch);
		if(!travel)continue;
		travel->OnStartSequenceFinishedCheck.AddDynamic(this, &ALevelStreamingActor::ForwardSequenceFinished);
	}

	// play forward sequence
	Multicast_ForwardSequence();
}

void ALevelStreamingActor::LoadLevelFinished()
{
	// this is server

	bLoad = 1;

	// play reverse sequence
	StartReverseSequence();
}

void ALevelStreamingActor::Multicast_ForwardSequence_Implementation()
{
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UTravelEffectComponent* travel = CHelpers::GetComponent<UTravelEffectComponent>(pawn);
	if (travel)
		travel->Play();
}

void ALevelStreamingActor::Multicast_ReverseSequence_Implementation()
{
	APawn* pawn = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0)->GetPawn();

	UTravelEffectComponent* travel = CHelpers::GetComponent<UTravelEffectComponent>(pawn);
	if (travel)
		travel->Reverse();
}

void ALevelStreamingActor::ForwardSequenceFinished(APlayerCharacter* InPlayerCharacter)
{
	SequenceMap.FindOrAdd(InPlayerCharacter) = 1;
	StartReverseSequence();
}

void ALevelStreamingActor::StartReverseSequence()
{
	// is load?
	CheckFalse(bLoad);

	// check sequencemap
	for (auto i : SequenceMap)
		CheckFalse(i.Value);

	// set values
	int32 idx = 0;
	for (auto i : SequenceMap)
	{
		while (1)
			if (!GetCurrentData().StartLocations.IsValidIndex(idx))
			{
				CLog::Print("ALevelStreamingActor::StartReverseSequence " + FString::FromInt(idx) + "is not valid idx");
				if (!idx)return;
				--idx;
				continue;
			}
			else break;
		FVector loc = GetCurrentData().StartLocations[idx++];
		i.Key->SetActorLocation(loc);
	}

	// reeset values
	bLoad = 0;
	SequenceMap.Empty();

	FTimerHandle WaitHandle;
	float WaitTime = 0.5;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		Multicast_ReverseSequence();
	}), WaitTime, false);
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

FStageData ALevelStreamingActor::GetCurrentData()const
{
	for (auto& i : Datas)
		if (i.StageList == CurrentStage)
			return i;
	return FStageData();
}
