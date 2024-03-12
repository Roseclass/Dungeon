#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyCharacter.generated.h"

/**
 * 
 */

class UBorder;
class UMaterialInterface;
class UButton;
class UTextBlock;
class UEditableTextBox;
class ALobbyCharacter;
class UUW_LobbyCharacter;

enum class EAppearancePart : uint8;

DECLARE_DELEGATE_RetVal_TwoParams(FLinearColor, FCreateColor, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FColorPalettePicked, FLinearColor);
DECLARE_DELEGATE(FPaletteButtonClicked);
DECLARE_DELEGATE(FLobbyCharacter);

UCLASS()
class DUNGEON_API UUW_LobbyCharacterPart : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct()override;

	//property
private:
	int32 Index;
	UUW_LobbyCharacter* Parent;
protected:
	UPROPERTY(EditDefaultsOnly)
		FName ColorParameterName;

	UPROPERTY(EditDefaultsOnly)
		FString PartText;

	UPROPERTY(EditDefaultsOnly)
		int32 MaxCount;

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
	FPaletteButtonClicked OnPaletteButtonClicked;

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
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	//property
private:
	friend UUW_LobbyCharacterPart;

	UUW_LobbyCharacterPart* CurrentTab;
	ALobbyCharacter* Target;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "PaletteMaterial")
		UMaterialInterface* Material_RGB;

	UPROPERTY(EditDefaultsOnly, Category = "PaletteMaterial")
		UMaterialInterface* Material_Skin;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBorder* ColorPalette;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* Hair;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* Eyebrows;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* Head;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* FacialHair;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UUW_LobbyCharacterPart* Skin;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Confirm;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* Cancel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* NameBox;

public:
	FLobbyCharacter OnConfirmButtonClickedDelegate;
	FLobbyCharacter OnCancelButtonClickedDelegate;


	//function
private:
	UFUNCTION()FEventReply OnColorPaletteMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION()FLinearColor Palette_RGB(float X, float Y);
	UFUNCTION()FLinearColor Palette_Skin(float X, float Y);
	UFUNCTION()void OnConfirmClicked();
	UFUNCTION()void OnCancelClicked();
	UFUNCTION()void SetRGBPallette();
	UFUNCTION()void SetSkinPallette();

protected:
public:
	FORCEINLINE void SetTarget(ALobbyCharacter* InTarget) { Target = InTarget; }
};
