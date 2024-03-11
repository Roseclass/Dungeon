#include "Widgets/UW_LobbyCharacter.h"
#include "Global.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "SaveManager.h"
#include "Characters/LobbyCharacter.h"

/////////////////////////
// UUW_LobbyCharacterPart
/////////////////////////

void UUW_LobbyCharacterPart::NativeConstruct()
{
	Super::NativeConstruct();

	Prev->OnClicked.AddDynamic(this,&UUW_LobbyCharacterPart::OnPrevClicked);
	Next->OnClicked.AddDynamic(this,&UUW_LobbyCharacterPart::OnNextClicked);
	Palette->OnClicked.AddDynamic(this, &UUW_LobbyCharacterPart::OnPaletteClicked);
}

void UUW_LobbyCharacterPart::OnPrevClicked()
{	
	if (--Index <= 0)
		Index = MaxCount-1;

	for (auto i : Parts)
		Parent->Target->ChangeAppearance(i, Index);

	FString numb = FString::FromInt(Index);
	FString add;
	for (int32 i = numb.Len(); i < 2; ++i)add += '0';
	FString text = PartText + add + numb;
	Text->SetText(FText::FromString(text));
}

void UUW_LobbyCharacterPart::OnNextClicked()
{
	if (++Index >= MaxCount)Index = 0;

	for (auto i : Parts)
		Parent->Target->ChangeAppearance(i, Index);

	FString numb = FString::FromInt(Index);
	FString add;
	for (int32 i = numb.Len(); i < 2; ++i)add += '0';
	FString text = PartText + add + numb;
	Text->SetText(FText::FromString(text));
}

void UUW_LobbyCharacterPart::OnPaletteClicked()
{
	Parent->CurrentTab = this;
	OnPaletteButtonClicked.ExecuteIfBound();
}

void UUW_LobbyCharacterPart::Init(UUW_LobbyCharacter* InParent)
{
	Parent = InParent;
}

void UUW_LobbyCharacterPart::ChangeColor(float X, float Y)
{
	FLinearColor color = GetColor(X, Y);

	for (auto i : Parts)
		Parent->Target->ChangeColorData(i, ColorParameterName, color);
}

FLinearColor UUW_LobbyCharacterPart::GetColor(float X, float Y)
{
	FLinearColor result;
	result = OnCreateColor.Execute(X,Y);
	return result;
}

/////////////////////////
// UUW_LobbyCharacter
/////////////////////////

void UUW_LobbyCharacter::NativeConstruct()
{
	Super::NativeConstruct();

	ColorPalette->OnMouseButtonDownEvent.BindUFunction(this, "OnColorPaletteMouseButtonDown");
	Confirm->OnClicked.AddDynamic(this, &UUW_LobbyCharacter::OnConfirmClicked);
	Cancel->OnClicked.AddDynamic(this, &UUW_LobbyCharacter::OnCancelClicked);

	Hair->Init(this);
	Hair->OnCreateColor.BindUFunction(this,"Palette_RGB");
	Hair->OnPaletteButtonClicked.BindUFunction(this,"SetRGBPallette");

	Eyebrows->Init(this);
	Eyebrows->OnCreateColor.BindUFunction(this,"Palette_RGB");
	Eyebrows->OnPaletteButtonClicked.BindUFunction(this,"SetRGBPallette");

	Head->Init(this);
	Head->OnCreateColor.BindUFunction(this,"Palette_RGB");
	Head->OnPaletteButtonClicked.BindUFunction(this,"SetRGBPallette");

	FacialHair->Init(this);
	FacialHair->OnCreateColor.BindUFunction(this,"Palette_RGB");
	FacialHair->OnPaletteButtonClicked.BindUFunction(this,"SetRGBPallette");

	Skin->Init(this);
	Skin->OnCreateColor.BindUFunction(this,"Palette_Skin");
	Skin->OnPaletteButtonClicked.BindUFunction(this,"SetSkinPallette");

	CurrentTab = Hair;

	Target = Cast<ALobbyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ALobbyCharacter::StaticClass()));
}

FReply UUW_LobbyCharacter::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	float y = InMouseEvent.GetGestureDelta().Y;
	
	if (Target)Target->OnWheel(y);

	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

FEventReply UUW_LobbyCharacter::OnColorPaletteMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	float x, y;

	FVector2D pos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	FVector2D topLeft = USlateBlueprintLibrary::GetLocalTopLeft(ColorPalette->GetCachedGeometry());
	FVector2D size = USlateBlueprintLibrary::GetLocalSize(ColorPalette->GetCachedGeometry());

	pos = (pos - topLeft) / size;
	x = pos.X;y = pos.Y;

	if (CurrentTab)
		CurrentTab->ChangeColor(x, y);

	return UWidgetBlueprintLibrary::Handled();
}

FLinearColor UUW_LobbyCharacter::Palette_RGB(float X, float Y)
{
	FVector4 k = FVector4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);

	FVector hsv = FVector(X, 1, 1 - Y);
	float s = hsv.Y;
	float v = hsv.Z;

	FVector p;
	p.X = abs(UKismetMathLibrary::Fraction(hsv.X + k.X) * 6.0 - k.W);
	p.Y = abs(UKismetMathLibrary::Fraction(hsv.X + k.Y) * 6.0 - k.W);
	p.Z = abs(UKismetMathLibrary::Fraction(hsv.X + k.Z) * 6.0 - k.W);

	FVector result;
	result.X = v * UKismetMathLibrary::Lerp(k.X, UKismetMathLibrary::FClamp(p.X - k.X, 0, 1), s);
	result.Y = v * UKismetMathLibrary::Lerp(k.X, UKismetMathLibrary::FClamp(p.Y - k.X, 0, 1), s);
	result.Z = v * UKismetMathLibrary::Lerp(k.X, UKismetMathLibrary::FClamp(p.Z - k.X, 0, 1), s);

	return FLinearColor(result.X, result.Y, result.Z, 1);
}

FLinearColor UUW_LobbyCharacter::Palette_Skin(float X, float Y)
{
	FLinearColor colorA, colorB;
	ColorPalette->GetDynamicMaterial()->GetVectorParameterValue(FHashedMaterialParameterInfo("ColorA"),colorA);
	ColorPalette->GetDynamicMaterial()->GetVectorParameterValue(FHashedMaterialParameterInfo("ColorB"),colorB);

	float saturation;
	ColorPalette->GetDynamicMaterial()->GetScalarParameterValue(FHashedMaterialParameterInfo("Saturation"), saturation);
	
	FLinearColor result;
	result.R = UKismetMathLibrary::Lerp(colorA.R, colorB.R,X);
	result.G = UKismetMathLibrary::Lerp(colorA.G, colorB.G,X);
	result.B = UKismetMathLibrary::Lerp(colorA.B, colorB.B,X);

	float k = 1 - Y;
	k *= (1 - saturation);
	k += saturation;

	result *= k;
	result.A = 1;

	return result;
}

void UUW_LobbyCharacter::OnConfirmClicked()
{
	//save datas and back to session
	//TODO::~~~
	USaveManager::QueryAllSaveInterfaces();
	USaveManager::SaveGame();
}

void UUW_LobbyCharacter::OnCancelClicked()
{
	//reset character and back to session
	//TODO::~~~
}

void UUW_LobbyCharacter::SetRGBPallette()
{
	ColorPalette->SetBrushFromMaterial(Material_RGB);
}

void UUW_LobbyCharacter::SetSkinPallette()
{
	ColorPalette->SetBrushFromMaterial(Material_Skin);
}
