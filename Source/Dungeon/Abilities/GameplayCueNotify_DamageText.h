#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayCueNotify_DamageText.generated.h"

/**
 * 
 */

class ADamageText;

UCLASS(Blueprintable, Category = "GameplayCueNotify")
class DUNGEON_API UGameplayCueNotify_DamageText : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGameplayCueNotify_DamageText();
protected:
public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ADamageText> DamageTextClass;
public:

	//function
private:
protected:
public:
};
