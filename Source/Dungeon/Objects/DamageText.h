#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
protected:	
	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Scene;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		UWidgetComponent* Widget;
public:

	//function
private:
protected:
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)void Init(float InDamage, bool IsCritical);
};
