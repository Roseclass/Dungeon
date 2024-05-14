#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/IInteractable.h"
#include "NPC.generated.h"

class UShapeComponent;
class UMeshComponent;
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

	virtual void StartInteract(ADungeonPlayerController* InPlayer)override;
	virtual void EndInteract(ADungeonPlayerController* InPlayer)override;
	virtual void StartCursorOver(ADungeonPlayerController* InPlayer)override;
	virtual void EndCursorOver(ADungeonPlayerController* InPlayer)override;
	virtual bool IsInteractable()override;

	virtual void Init();

	//property
private:
	bool IsInteracting;
	TArray<UShapeComponent*> InteractionCollisionComponents;
	TArray<UMeshComponent*> MeshComponents;
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UDialogComponent* Dialog;

	UPROPERTY(EditDefaultsOnly, Category = "ComponentTags")
		FName InteractionCollisionTag = FName("InteractionCollision");
public:
	//function
private:
	void FindInteractionCollision();
	void FindMesh();
protected:
public:
};
