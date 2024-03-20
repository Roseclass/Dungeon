#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/IInteractable.h"
#include "NPC.generated.h"

class UShapeComponent;
class UDialogComponent;

UCLASS()
class DUNGEON_API ANPC : public ACharacter, public IIInteractable
{
	GENERATED_BODY()
	
public:	
	ANPC();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(APlayerCharacter* InPlayer)override;
	virtual void PreInteractEvent(APlayerCharacter* InPlayer)override;
	virtual void InInteractEvent(APlayerCharacter* InPlayer)override;
	virtual void PostInteractEvent(APlayerCharacter* InPlayer)override;
	virtual bool IsInteractable()override;

	virtual void Init();

	//property
private:
	bool IsInteracting;
	TArray<UShapeComponent*> InteractionCollisionComponents;
protected:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		UDialogComponent* Dialog;

	UPROPERTY(EditDefaultsOnly, Category = "ComponentTags")
		FName InteractionCollisionTag = FName("InteractionCollision");
public:
	//function
private:
	void FindInteractionCollision();
protected:
public:
	void EndInteract();
};
