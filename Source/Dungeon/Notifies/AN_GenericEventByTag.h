#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_GenericEventByTag.generated.h"

/**
 * send gameplay event to  actor
 */

UCLASS()
class DUNGEON_API UAN_GenericEventByTag : public UAnimNotify
{
	GENERATED_BODY()
public:
	FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag EventTag;
};
