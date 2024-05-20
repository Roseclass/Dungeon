#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Orb.generated.h"

/**
 * 
 */

class UProgressBar;
class UUW_Main;

UCLASS()
class DUNGEON_API UUW_Orb : public UUserWidget
{
	GENERATED_BODY()
	friend UUW_Main;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

	//property
private:
	bool bUpdate = 1;
	float Max;
	float Regen;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UProgressBar* Orb;
public:

	//function
private:
	UFUNCTION() void SetMax(float NewMax);
	UFUNCTION() void SetPercent(float NewPercent);
	UFUNCTION() void SetRegen(float NewRegen);
protected:
public:
	void Dead();
};
