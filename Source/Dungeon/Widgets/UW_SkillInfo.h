#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SkillInfo.generated.h"

/**
 * 
 */

class UTextBlock;
class UImage;
class UMediaPlayer;
class ASkillActor;

UCLASS()
class DUNGEON_API UUW_SkillInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized()override;
public:

	//property
private:
	//USkillTreeComponent* OwnerComponent;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Name;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Description;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* Preview;

	UPROPERTY(EditDefaultsOnly)
		UMediaPlayer* MediaPlayer;
public:

	//function
private:
	UFUNCTION()void OnMediaOpened(FString OpenedUrl);
	UFUNCTION()void OnMediaPlay();
	UFUNCTION()void OnMediaPause();
protected:
public:
	void On(ASkillActor* InSkillActor);
	void Off();
	
};
