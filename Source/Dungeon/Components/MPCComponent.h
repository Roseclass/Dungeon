#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPCComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UMPCComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMPCComponent();
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
