#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimeLineComponent.h"
#include "WarningSign.generated.h"

/*
* spawn by notify
* spawn only in client
* not replicated
* showing range of skill
*/

class USceneComponent;
class UDecalComponent;
class UMaterialInstance;

UCLASS()
class DUNGEON_API AWarningSign : public AActor
{
	GENERATED_BODY()
	
public:	
	AWarningSign();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	bool bActivate;
	float StartTime;
protected:
	UPROPERTY(EditAnywhere, Category = "Duration")
		float Duration;

	UPROPERTY(EditAnywhere, Category = "Duration")
		float ExtraDuration;

	UPROPERTY(EditAnywhere, Category = "Duration")
		UMaterialInstance* Sign;

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly)
		UDecalComponent* Decal;
public:

	//function
private:
protected:
public:
	void Activate(float StartServerTime, float NewDuration, float NewExtraDuration);
};
