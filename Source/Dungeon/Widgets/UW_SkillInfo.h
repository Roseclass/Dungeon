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
class USkillComponent;

UCLASS()
class DUNGEON_API UUW_SkillInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized()override;
public:

	//property
private:
	USkillComponent* OwnerComponent;
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
	void On(int32 InSkillID);
	void Off();
	
	FORCEINLINE void SetOwnerComponent(USkillComponent* NewOwnerComponent) { OwnerComponent = NewOwnerComponent; }
};
