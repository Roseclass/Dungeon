#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MontageComponent.generated.h"

/**
 * playing anim montages ex)dead, hitted...
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
	UPROPERTY(EditAnywhere, Category = "Datas")
		UAnimMontage* KnockBackMontage;

	UPROPERTY(EditAnywhere, Category = "Datas")
		UAnimMontage* KnockDownMontage;

public:

	//function
private:
protected:
public:
	void PlayKnockDownMontage(FVector NewForce);

	FORCEINLINE FVector GetForce()const {return Force;}
};
