#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogComponent.generated.h"

class ADungeonPlayerController;
class UPlayerDialogDatas;
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
	UPROPERTY()UPlayerDialogDatas* PlayerDialogDatas; // avoid garbage collecting
protected:
	UPROPERTY(EditDefaultsOnly)
		UBehaviorTree* DialogTree;

	UPROPERTY(EditDefaultsOnly)
		UBlackboardData* Blackboard;

	UPROPERTY(EditDefaultsOnly)
		FName PlayerDialogDatasObjectBBName = FName("Datas");

	UPROPERTY(EditDefaultsOnly)
		FName PlayerControllerObjectBBName = FName("InteractingPlayer");

	UPROPERTY(EditDefaultsOnly)
		FName RewardBoolBBName = FName("Reward");

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
	void OnInteraction(ADungeonPlayerController* InPlayerController);
	void OnReply(ADungeonPlayerController* InPlayerController, int32 NextPoint);

	void SetValueAsBool(FName InName, bool Value);
	void SetValueAsInt(FName InName, int32 Value);
	bool GetValueAsBool(FName InName);
	int32 GetValueAsInt(FName InName);

	FORCEINLINE UObject* GetPortrait() const { return Portrait;}
	FORCEINLINE FText GetName() const { return Name; }
};
