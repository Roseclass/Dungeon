#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyCharacter.generated.h"

/**
 * 
 */

class UBorder;
class UButton;
class UText;
class UEditableTextBox;

DECLARE_DELEGATE_RetVal_TwoParams(FLinearColor, FColorPalette, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FColorPalettePicked, FVector);

UCLASS()
class DUNGEON_API UUW_LobbyCharacter : public UUserWidget
{
	GENERATED_BODY()
	//property
private:
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* ColorPalette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* HairPrev;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* HairNext;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* HairPalette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UText* HairText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* UpperBodyPrev;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* UpperBodyNext;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* UpperBodyPalette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UText* UpperBodyText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* LowerBodyPrev;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* LowerBodyNext;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* LowerBodyPalette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UText* LowerBodyText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* SkinPalette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Cofirm;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Cancel;

public:
	FColorPalette OnColorPalette;
	FColorPalettePicked OnColorPalettePicked;

	//function
private:
	UFUNCTION()FEventReply OnColorPaletteMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION()FLinearColor Palette_RGB(float X, float Y);
	UFUNCTION()FLinearColor Palette_Skin(float X, float Y);

	UFUNCTION()void OnColorPalettePicked();
	UFUNCTION()void OnPrevClicked();
	UFUNCTION()void OnNextClicked();
	UFUNCTION()void OnPaletteClicked();
	UFUNCTION()void OnSkinPaletteClicked();

protected:
public:
};
