#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogComponent.generated.h"

class APlayerCharacter;
class UUW_Dialog;
class UBehaviorTree;
class UBlackboardData;
class AAIController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UDialogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDialogComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UUW_Dialog* DialogWidget;
	AAIController* Controller;
protected:
	UPROPERTY(EditDefaultsOnly)
		UBehaviorTree* DialogTree;

	UPROPERTY(EditDefaultsOnly)
		UBlackboardData* Blackboard;

	UPROPERTY(EditDefaultsOnly)
		FName WidgetObjectBBName = FName("DialogWidget");

	UPROPERTY(EditDefaultsOnly)
		FName PlayerObjectBBName = FName("Player");

	UPROPERTY(EditDefaultsOnly)
		FName RewardBoolBBName = FName("Reward");

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UUW_Dialog> DialogWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widget", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface", DisallowedClasses = "MediaTexture"))
		TObjectPtr<UObject> Portrait;

	UPROPERTY(EditAnywhere, Category = "Widget")
		FText Name;

public:

	//function
private:
	UFUNCTION()void OnExit();
protected:
public:
	void OnInteraction(APlayerCharacter* InPlayer);

	void SetValueAsBool(FName InName, bool Value);
	void SetValueAsInt(FName InName, int32 Value);
	bool GetValueAsBool(FName InName);
	int32 GetValueAsInt(FName InName);

};
