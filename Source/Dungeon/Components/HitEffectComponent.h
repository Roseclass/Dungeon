#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitEffectComponent.generated.h"

class USkeletalMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UHitEffectComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UHitEffectComponent();
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//property
private:
	TArray<USkeletalMeshComponent*> Meshes;

	FVector CurrentEmissiveColor;
protected:
	UPROPERTY(EditAnywhere, Category = "Glow")
		FName ParameterName = FName("HitEmissive");

	UPROPERTY(EditAnywhere, Category = "Glow")
		FVector EmissiveColor = FVector(3, 0, 0);

	UPROPERTY(EditAnywhere, Category = "Glow")
		float GlowTime = 0.5;
public:

	//function
private:
	void FindMesh();
	void UpdateGlow(float DeltaTime);
protected:
public:
	void Glow();
};
