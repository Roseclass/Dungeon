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
public:
	UMontageComponent();
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	UPROPERTY(Replicated)AActor* DamageCauser;
	UPROPERTY(Replicated)FVector Force;
protected:
	UPROPERTY(EditAnywhere, Category = "Datas")
		UAnimMontage* DeadMontage;

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
	void PlayDeadMontage();
	void PlayHitMontage();
	void PlayKnockDownMontage();

	FORCEINLINE void SetDamageCauser(AActor* NewDamageCauser) { DamageCauser = NewDamageCauser; }
	FORCEINLINE void SetForce(FVector NewForce) { Force = NewForce; }


	FORCEINLINE FVector GetForce()const {return Force;}
};
