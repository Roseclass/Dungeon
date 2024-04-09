#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClearViewComponent.generated.h"

/*
* masks objects between camera and owner
*/

class USpringArmComponent;
class UMaterialParameterCollection;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UClearViewComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UClearViewComponent();
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	UPROPERTY()USpringArmComponent* SpringArm;
protected:

	UPROPERTY(EditAnywhere, Category = "Dissolve")
		UMaterialParameterCollection* Collection;

	UPROPERTY(EditAnywhere, Category = "Dissolve")
		float HorizontalSize = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Dissolve")
		float VerticalSize = 2000.0f;

public:

	//function
private:
protected:
public:
};
