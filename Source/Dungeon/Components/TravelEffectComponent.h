#pragma once

#include "CoreMinimal.h"
#include "Components/PostProcessComponent.h"
#include "Components/TimelineComponent.h"
#include "TravelEffectComponent.generated.h"

/**
 * 
 */

class UMaterialParameterCollection;
class UMaterialInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTravelReserveEvent);

UCLASS()
class DUNGEON_API UTravelEffectComponent : public UPostProcessComponent
{
	GENERATED_BODY()
public:
	UTravelEffectComponent();
protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void Serialize(FArchive& Ar) override;
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere, Category = "Materials")
		UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Category = "Materials")
		UMaterialParameterCollection* Collection;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveFloat* SequenceCurve;
	FTimeline SequenceTimeline;
	FOnTimelineFloat SequenceFloat;

public:
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)FTravelReserveEvent OnStartSequenceFinished;
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)FTravelReserveEvent OnEndSequenceFinished;

	//function
private:
	UFUNCTION()void SequenceTickFunction(float Value);
protected:
public:
	UFUNCTION(BlueprintCallable)void Play();
	UFUNCTION(BlueprintCallable)void Reverse();
};
