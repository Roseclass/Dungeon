#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuffActor_Overlap.generated.h"

class UShapeComponent;
class UGameplayEffect;

UCLASS()
class DUNGEON_API ABuffActor_Overlap : public AActor
{
	GENERATED_BODY()	
public:	
	ABuffActor_Overlap();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	//property
private:
	TArray<UShapeComponent*> OverlapCollisionComponents;
protected:
	UPROPERTY(EditAnywhere, Category = "Essential")
		FName OverlapTag = FName("OverlapCollision");

	UPROPERTY(EditAnywhere, Category = "Essential")
		TSubclassOf<UGameplayEffect> EffectClass;
public:

	//function
private:
	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void FindComponents();
protected:
public:
};
