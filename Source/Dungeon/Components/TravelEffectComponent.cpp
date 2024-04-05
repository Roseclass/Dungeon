#include "Components/TravelEffectComponent.h"
#include "Global.h"
#include "Kismet/KismetMaterialLibrary.h"

UTravelEffectComponent::UTravelEffectComponent()
{

}

void UTravelEffectComponent::OnRegister()
{

}

void UTravelEffectComponent::OnUnregister()
{

}

void UTravelEffectComponent::Serialize(FArchive& Ar)
{

}

void UTravelEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	FWeightedBlendable wb;
	wb.Weight = 0;
	wb.Object = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), Material);
	Settings.WeightedBlendables.Array.Add(wb);

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
	UMaterialInstanceDynamic* mat = Cast<UMaterialInstanceDynamic>(Settings.WeightedBlendables.Array[0].Object);
	CheckNull(mat);
	mat->SetScalarParameterValue(FName("Radius Offset"), Value);
}

void UTravelEffectComponent::Play()
{
	CheckFalse(Settings.WeightedBlendables.Array.Num());
	CheckNull(GetOwner());

	// set mpc
	FVector loc = GetOwner()->GetActorLocation();
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("X"),loc.X);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("Y"),loc.Y);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Collection, FName("Z"),loc.Z);

	// set WeightedBlendable
	Settings.WeightedBlendables.Array[0].Weight = 1;

	SequenceTimeline.PlayFromStart();

	FTimerHandle WaitHandle;
	float WaitTime = SequenceTimeline.GetTimelineLength();
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	{
		if (OnStartSequenceFinished.IsBound())
		{
			OnStartSequenceFinished.Broadcast();
			OnStartSequenceFinished.Clear();
		}
	}), WaitTime, false);

}

void UTravelEffectComponent::Reverse()
{
	CheckFalse(Settings.WeightedBlendables.Array.Num());
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
		Settings.WeightedBlendables.Array[0].Weight = 0;

		if (OnEndSequenceFinished.IsBound())
		{
			OnEndSequenceFinished.Broadcast();
			OnEndSequenceFinished.Clear();
		}
	}), WaitTime, false);
}
