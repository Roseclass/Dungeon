#pragma once

#include "CoreMinimal.h"
#include "Objects/Eqquipment.h"
#include "Weapon.generated.h"

/**
 *
 */

class UGameplayEffect;

struct FDamageEhancementData;

UCLASS()
class DUNGEON_API AWeapon : public AEqquipment
{
	GENERATED_BODY()
	
public:	
	AWeapon();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//property
private:
	TArray<UShapeComponent*> CollisionComponents;
	TArray<AActor*> HitActors;
	const FDamageEhancementData* DamageData;

	UPROPERTY(EditDefaultsOnly, Category = "Attach")
		FTransform AttachTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Attach")
		FName SocketName;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Essential")
		uint8 TeamID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Essential")
		float Damage;

	UPROPERTY(EditAnywhere, Category = "Essential")
		TSubclassOf<UGameplayEffect> CommonEffectClass;

	UPROPERTY(EditAnywhere, Category = "Essential")
		TArray<TSubclassOf<UGameplayEffect>> UniqueEffectClasses;

public:

	//function
private:
protected:	
	virtual void OnRep_Mode()override;
	virtual void FindComponents()override;
	virtual void SpawnLootEffects()override;
	virtual void SetEffectLocation()override;
	virtual void SortMesh()override;
	virtual void ActivateEffect()override;
	virtual void DeactivateEffect()override;
	virtual void SetPickableMode()override;
	virtual void SetInventoryMode()override;
	virtual void SetEquipMode()override;

	UFUNCTION()void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:
	virtual void SetOwnerCharacter(ACharacter* InCharacter)override;
	void OnCollision(const FDamageEhancementData* InDamageData);
	void OffCollision();
	void ResetHitActors();

	virtual void SetItemLocation(const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None)override;
	virtual void SetItemRotation(FRotator NewRotation, ETeleportType Teleport = ETeleportType::None)override;
	virtual void AttachItemToComponent(USceneComponent* Parent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName = NAME_None)override;
	virtual void ChangeVisibility(EItemMode InMode)override;
	virtual void SetMode(EItemMode InMode)override;

	void GetUniqueEffectDescriptions(TArray<FString>& Descriptions)const;

	FORCEINLINE void SetTeamID(uint8 InID) { TeamID = InID; }
	FORCEINLINE void SetDamage(float InDamage) { Damage = InDamage; }

	FORCEINLINE FTransform GetAttachTransform() { return AttachTransform; }
	FORCEINLINE FName GetSocketName() { return SocketName; }
};
