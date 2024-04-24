#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "TravelEffectComponent.generated.h"

/**
 * 
 */

class UMaterialParameterCollection;
class UMaterialInterface;
class UCameraComponent;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTravelReserveEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTravelSequenceFinishedCheck, APlayerCharacter*, InPlayerCharacter);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEON_API UTravelEffectComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UTravelEffectComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UPROPERTY()UCameraComponent* Camera;
	UPROPERTY()UMaterialInstanceDynamic* DynamicMaterial;
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
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)FTravelSequenceFinishedCheck OnStartSequenceFinishedCheck;
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)FTravelSequenceFinishedCheck OnEndSequenceFinishedCheck;

	//function
private:
	UFUNCTION()void SequenceTickFunction(float Value);
	UFUNCTION(Reliable, Server)void Server_StartSequenceFinished();
	UFUNCTION(Reliable, Server)void Server_EndSequenceFinished();
protected:
public:
	UFUNCTION(BlueprintCallable)void Play();
	UFUNCTION(BlueprintCallable)void Reverse();
};

/* hide hud, reveal hud*/