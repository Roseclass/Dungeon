// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "DungeonPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class ADungeonCharacter;

UCLASS()
class ADungeonPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADungeonPlayerController();
protected:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
public:

	//property
private:
	bool bInputPressed;
	float FollowTime;

	ADungeonCharacter* Target;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		float ShortPressThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UNiagaraSystem* FXCursor;

	uint32 bMoveToMouseCursor : 1;

public:

	//function
private:
protected:
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
public:
};


