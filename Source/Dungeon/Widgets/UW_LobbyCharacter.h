#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyCharacter.generated.h"

/**
 * 
 */

class UBorder;
class UButton;
class UTextBlock;
class UEditableTextBox;
class ALobbyCharacter;
class UUW_LobbyCharacter;

enum class EAppearancePart : uint8;

DECLARE_DELEGATE_RetVal_TwoParams(FLinearColor, FCreateColor, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FColorPalettePicked, FLinearColor);

UCLASS()
class DUNGEON_API UUW_LobbyCharacterPart : public UUserWidget
{
	GENERATED_BODY()
	//property
private:
	int32 Index;
	UUW_LobbyCharacter* Parent;
protected:
	UPROPERTY(EditDefaultsOnly)
		FString PartText;

	UPROPERTY(EditDefaultsOnly)
		TArray<EAppearancePart> Parts;

	//widget
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Prev;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Next;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Palette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* Text;
public:
	FCreateColor OnCreateColor;

	//function
private:
	UFUNCTION()void OnPrevClicked();
	UFUNCTION()void OnNextClicked();
	UFUNCTION()void OnPaletteClicked();
protected:
public:
	void Init(UUW_LobbyCharacter* InParent);
	void ChangeColor(float X, float Y);

	FLinearColor GetColor(float X, float Y);
};

UCLASS()
class DUNGEON_API UUW_LobbyCharacter : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

	//property
private:
	friend UUW_LobbyCharacterPart;

	UUW_LobbyCharacterPart* CurrentTab;
	ALobbyCharacter* Target;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* ColorPalette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* Hair;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* UpperBody;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* LowerBody;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* Skin;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Cofirm;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Cancel;

public:

	//function
private:
	UFUNCTION()FEventReply OnColorPaletteMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION()FLinearColor Palette_RGB(float X, float Y);
	UFUNCTION()FLinearColor Palette_Skin(float X, float Y);

protected:
public:
};
