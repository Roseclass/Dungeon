#include "Components/TravelEffectComponent.h"
#include "Global.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Camera/CameraComponent.h"

#include "DungeonPlayerController.h"
#include "Characters/PlayerCharacter.h"

UTravelEffectComponent::UTravelEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(1);
}

void UTravelEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	Camera = CHelpers::GetComponent<UCameraComponent>(GetOwner());
	if (!Camera)
	{
		CLog::Print("UTravelEffectComponent::BeginPlay Camera is null ptr", -1, 10, FColor::Red);
		return;
	}

	DynamicMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), Material);
	if (DynamicMaterial)
	{
		FWeightedBlendable wb;
		wb.Weight = 0;
		wb.Object = DynamicMaterial;
		Camera->PostProcessSettings.WeightedBlendables.Array.Add(wb);
	}
	else
	{
		CLog::Print("UTravelEffectComponent::BeginPlay DynamicMaterial is null ptr", -1, 10, FColor::Red);
		return;
	}

	//FOnTimelineEvent f;
	//f.BindUFunction(this, "ExecuteStageFailedSequence");
	//Timeline.SetTimelineFinishedFunc(f);

	SequenceFloat.BindUFunction(this, "SequenceTickFunction");
	SequenceTimeline.AddInterpFloat(SequenceCurve, SequenceFloat);
}

void UTravelEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SequenceTimeline.TickTimeline(DeltaTime);
}

void UTravelEffectComponent::SequenceTickFunction(float Value)
{
	DynamicMaterial->SetScalarParameterValue(FName("Radius Offset"), Value);
}

void UTravelEffectComponent::Server_StartSequenceFinished_Implementation()
{
	if (OnStartSequenceFinishedCheck.IsBound())
		OnStartSequenceFinishedCheck.Broadcast(Cast<APlayerCharacter>(GetOwner()));
}

void UTravelEffectComponent::Server_EndSequenceFinished_Implementation()
{
	if (OnEndSequenceFinishedCheck.IsBound())
		OnEndSequenceFinishedCheck.Broadcast(Cast<APlayerCharacter>(GetOwner()));
}

void UTravelEffectComponent::Play()
{
	CheckNull(GetOwner());

	// set mpc
	FVector loc = GetOwner()->GetActorLocation();
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("X"),loc.X);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("Y"),loc.Y);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("Z"),loc.Z);

	// set WeightedBlendable
	if (Camera)
	{
		for (auto& i : Camera->PostProcessSettings.WeightedBlendables.Array)
		{
			if (i.Object != DynamicMaterial)continue;
			i.Weight = 1;
		}
	}
	else
	{
		CLog::Print("UTravelEffectComponent::Play Camera is null ptr", -1, 10, FColor::Red);
		return;
	}


	// hide widget
	ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner()->GetOwner()); 
	if (controller)
	{
		controller->HideWidget();

		TArray<AActor*> characters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADungeonCharacterBase::StaticClass(), characters);

		for (auto i : characters)
		{
			ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(i);
			if (!ch)continue;
			ch->HideHealthBar();
		}
	}
	else CLog::Print("UTravelEffectComponent::Play controller is null ptr", -1, 10, FColor::Red);

	SequenceTimeline.Play();

	FTimerHandle WaitHandle;
	float WaitTime = SequenceTimeline.GetTimelineLength();
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		if (OnStartSequenceFinished.IsBound())
		{
			OnStartSequenceFinished.Broadcast();
			OnStartSequenceFinished.Clear();
		}
		Server_StartSequenceFinished();
	}), WaitTime, false);

}

void UTravelEffectComponent::Reverse()
{
	CheckNull(GetOwner());

	if (SequenceTimeline.IsPlaying())
	{
		OnStartSequenceFinished.AddDynamic(this, &UTravelEffectComponent::Reverse);
		return;
	}

	// set mpc
	FVector loc = GetOwner()->GetActorLocation();
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("X"), loc.X);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("Y"), loc.Y);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("Z"), loc.Z);

	SequenceTimeline.ReverseFromEnd();

	FTimerHandle WaitHandle;
	float WaitTime = SequenceTimeline.GetTimelineLength();
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		// set WeightedBlendable
		if (Camera)
		{
			for (auto& i : Camera->PostProcessSettings.WeightedBlendables.Array)
			{
				if (i.Object != DynamicMaterial)continue;
				i.Weight = 0;
			}
		}
		else
		{
			CLog::Print("UTravelEffectComponent::Reverse Camera is null ptr", -1, 10, FColor::Red);
			return;
		}
		if (OnEndSequenceFinished.IsBound())
		{
			OnEndSequenceFinished.Broadcast();
			OnEndSequenceFinished.Clear();
		}

		// reveal widget
		ADungeonPlayerController* controller = Cast<ADungeonPlayerController>(GetOwner()->GetOwner());
		if (controller)
		{
			controller->RevealMainWidget();

			TArray<AActor*> characters;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADungeonCharacterBase::StaticClass(), characters);

			for (auto i : characters)
			{
				ADungeonCharacterBase* ch = Cast<ADungeonCharacterBase>(i);
				if (!ch)continue;
				ch->RevealHealthBar();
			}
		}

		Server_EndSequenceFinished();
	}), WaitTime, false);
}
