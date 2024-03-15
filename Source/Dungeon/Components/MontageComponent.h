#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MontageComponent.generated.h"

/**
 * playing anim montages ex)dead, hitted...
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UMontageComponent : public UActorComponent
{
	GENERATED_BODY()
	//property
private:
protected:
public:

	//engine
private:
protected:
public:
	UMontageComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//function
private:
protected:
public:
};
