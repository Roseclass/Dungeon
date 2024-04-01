#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimeLineComponent.h"
#include "DamageText.generated.h"

class USceneComponent;
class UWidgetComponent;

UCLASS()
class DUNGEON_API ADamageText : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageText();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	float DistanceX;
	float DistanceY;
	float CurrentX;
	float CurrentY;
	float CurrentOpacity;
protected:	
	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Scene;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		UWidgetComponent* Widget;

	UPROPERTY(EditDefaultsOnly, Category = "TranslationSettings")
		bool bUseTransltaionCurve;

	UPROPERTY(EditDefaultsOnly, Category = "TranslationSettings")
		float MinX = 30;

	UPROPERTY(EditDefaultsOnly, Category = "TranslationSettings")
		float MaxX = 48;

	UPROPERTY(EditDefaultsOnly, Category = "TranslationSettings", meta = (EditCondition = "!bUseTransltaionCurve", EditConditionHides))
		float MinY = -48;

	UPROPERTY(EditDefaultsOnly, Category = "TranslationSettings", meta = (EditCondition = "!bUseTransltaionCurve", EditConditionHides))
		float MaxY = -30;

	UPROPERTY(EditDefaultsOnly, Category = "TranslationSettings", meta = (EditCondition = "!bUseTransltaionCurve", EditConditionHides))
		float RateY = 1.4;

	UPROPERTY(EditDefaultsOnly, Category = "TranslationSettings", meta = (EditCondition = "bUseTransltaionCurve", EditConditionHides))
		UCurveFloat* YCurve;
	FTimeline YTimeLine;
	FOnTimelineFloat YTimelineFloat;



	UPROPERTY(EditDefaultsOnly, Category = "OpacitySettings")
		bool bUseOpacityCurve;

	UPROPERTY(EditDefaultsOnly, Category = "OpacitySettings", meta = (EditCondition = "!bUseOpacityCurve", EditConditionHides))
		float MinOpacity = 0.4;

	UPROPERTY(EditDefaultsOnly, Category = "OpacitySettings", meta = (EditCondition = "bUseOpacityCurve", EditConditionHides))
		UCurveFloat* OpacityCurve;
	FTimeline OpacityTimeLine;
	FOnTimelineFloat OpacityTimelineFloat;


public:

	//function
private:
	UFUNCTION()void YSequenceTickFunction(float Value);
	UFUNCTION()void OpacitySequenceTickFunction(float Value);
protected:
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)void Init(float InDamage, bool IsCritical);
};
