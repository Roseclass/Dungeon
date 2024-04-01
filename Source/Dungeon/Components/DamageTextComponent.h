#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageTextComponent.generated.h"

class ADamageText;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UDamageTextComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageTextComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
	void SpawnDamageText(FVector NewLocation, float InDamage, bool IsCritical);
};
