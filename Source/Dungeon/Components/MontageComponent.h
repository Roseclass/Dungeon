#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MontageComponent.generated.h"

/**
 * playing anim montages ex)dead, hit...
 */

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UMontageComponent : public UActorComponent
{
	GENERATED_BODY()
	//engine
private:
protected:
public:
	UMontageComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	FVector Force;
protected:
	UPROPERTY(EditAnywhere, Category = "HitDatas", meta = (DisplayName = "Fronts"))
		TArray<UAnimMontage*> HitFronts;

	UPROPERTY(EditAnywhere, Category = "HitDatas", meta = (DisplayName = "Backs"))
		TArray<UAnimMontage*> HitBacks;

	UPROPERTY(EditAnywhere, Category = "HitDatas", meta = (DisplayName = "Rights"))
		TArray<UAnimMontage*> HitRights;

	UPROPERTY(EditAnywhere, Category = "HitDatas", meta = (DisplayName = "Lefts"))
		TArray<UAnimMontage*> HitLefts;

	UPROPERTY(EditAnywhere, Category = "Datas")
		UAnimMontage* KnockBackMontage;

	UPROPERTY(EditAnywhere, Category = "Datas")
		UAnimMontage* KnockDownMontage;

public:

	//function
private:
	int32 FindDirection(float FDot, float RDot);
protected:
public:
	void PlayHitMontage(AActor* InCauser);
	void PlayKnockDownMontage(FVector NewForce);

	FORCEINLINE FVector GetForce()const {return Force;}
};
