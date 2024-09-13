#include "Widgets/UW_InventoryPopup.h"
#include "Global.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"

#include "Objects/ItemObject.h"
#include "Objects/Weapon.h"
#include "Abilities/AttributeSetBase.h"

#define Common(str) "<Grade.Common>" + str + "</>"
#define Uncommon(str) "<Grade.Uncommon>" + str + "</>"
#define Rare(str) "<Grade.Rare>" + str + "</>"
#define Unique(str) "<Grade.Unique>" + str + "</>"

void UUW_InventoryPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bOn)
		RefreshLocation();
}

void UUW_InventoryPopup::RefreshLocation()
{
	//find cursor loaction
	FGeometry geo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(GetWorld());
	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	mousePos /= geo.GetLocalSize();
	UCanvasPanelSlot* slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
	FVector2D anchSize = GetCachedGeometry().GetLocalSize();
	anchSize /= geo.GetLocalSize();

	FVector2D ad = FVector2D(0, 0);
	if (mousePos.X + anchSize.X > 1)
		ad.X = 1 - (mousePos.X + anchSize.X);
	if (mousePos.Y + anchSize.Y > 1)
		ad.Y = 1 - (mousePos.Y + anchSize.Y);

	slot->SetAnchors(FAnchors(mousePos.X + ad.X, mousePos.Y + ad.Y));
}

FString UUW_InventoryPopup::MakeEhancementText(const FSkillEnhancement& Data, EItemGrade Grade)
{
	FString result = "";

	{
		int32 dotIdx;
		FString skill = Data.SkillTag.ToString();
		if (skill.FindLastChar('.', dotIdx))skill = skill.RightChop(dotIdx + 1);
		result += skill;
		result += TEXT("의 ");
	}

	{
		FString statusValue = FString::Printf(TEXT("%.2f"), Data.EnhanceStatus);

		switch (Grade)
		{
		case EItemGrade::Common:statusValue = Common(statusValue); break;
		case EItemGrade::Uncommon:statusValue = Uncommon(statusValue); break;
		case EItemGrade::Rare:statusValue = Rare(statusValue); break;
		case EItemGrade::Unique:statusValue = Unique(statusValue); break;
		default:break;
		}

		if (Data.EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cost.Additive"))
			result += TEXT("마나 소모량 ") + statusValue + TEXT("감소");
		else if (Data.EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cost.Multiplicitive"))
			result += TEXT("마나 소모량 ") + statusValue + TEXT("%만큼 감소");
		else if (Data.EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cooldown.Additive"))
			result += TEXT("재사용 대기시간 ") + statusValue + TEXT("초 감소");
		else if (Data.EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Cooldown.Multiplicitive"))
			result += TEXT("재사용 대기시간 ") + statusValue + TEXT("%만큼 감소");
		else if (Data.EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Damage.Additive"))
			result += TEXT("데미지 ") + statusValue + TEXT("증가");
		else if (Data.EnhanceStatusTag == FGameplayTag::RequestGameplayTag("Skill.Damage.Multiplicitive"))
			result += TEXT("데미지 ") + statusValue + TEXT("%만큼 증가");
	}

	return result;
}


void UUW_InventoryPopup::On(UItemObject* InObject)
{
	const FItemStatusData& data = InObject->GetEqquipment()->GetItemStatus();

	RefreshLocation();

	//set Name
	Name->SetText(FText::FromString(data.GetName()));

	//set Status Description
	const TArray<FGameplayAttribute>& attributes = data.GetTargetAttributes();
	const TArray<float>& values = data.GetTargetAttributeValues();
	FString statusDescription = "";

	for (int32 i = 0; i < attributes.Num(); ++i)
	{
		if (!values.IsValidIndex(i))break;
		if (attributes[i] == UAttributeSetBase::GetMaxHealthAttribute())
			statusDescription += FString::Printf(TEXT("최대체력%i증가"), values[i]);
		else if (attributes[i] == UAttributeSetBase::GetMaxManaAttribute())
			statusDescription += FString::Printf(TEXT("최대마나%i증가"), values[i]);
		else if(attributes[i] == UAttributeSetBase::GetDefenseAttribute())
			statusDescription += FString::Printf(TEXT("방어력%i증가"), values[i]);
		else if(attributes[i] == UAttributeSetBase::GetAdditiveDefenseAttribute())
			statusDescription += FString::Printf(TEXT("방어력%i추가증가"), values[i]);
		else if(attributes[i] == UAttributeSetBase::GetMultiplicitiveDefenseAttribute())
			statusDescription += FString::Printf(TEXT("방어력%i%증가"), values[i]);
		else if(attributes[i] == UAttributeSetBase::GetPowerAttribute())
			statusDescription += FString::Printf(TEXT("공격력%i증가"), values[i]);
		else if(attributes[i] == UAttributeSetBase::GetAdditivePowerAttribute())
			statusDescription += FString::Printf(TEXT("공격력%i추가증가"), values[i]);
		else if(attributes[i] == UAttributeSetBase::GetMultiplicitivePowerAttribute())
			statusDescription += FString::Printf(TEXT("공격력%i%증가"), values[i]);
		if (i == attributes.Num() - 1)break;
		statusDescription += FString::Printf(TEXT("\n"));
	}

	StatusDescription->SetText(FText::FromString(statusDescription));

	//set Effect Description
	TArray<FSkillEnhancement> enhancementDatas = data.GetEnhancementDatas();
	TArray<EItemGrade> enhancementGrades = data.GetEnhancementGrades();
	FString enhancementDescription = "";

	for (int32 i = 0; i < enhancementDatas.Num(); ++i)
	{
		if (!enhancementGrades.IsValidIndex(i))
		{
			CLog::Print(FString::Printf(TEXT("enhancementGrades %i is not valide Index"), i), -1, 10, FColor::Red);
			break;
		}
		if (enhancementGrades[i] == EItemGrade::Max)
		{
			CLog::Print(FString::Printf(TEXT("enhancementGrades %i is EItemGrade::Max"), i), -1, 10, FColor::Red);
			break;
		}
		enhancementDescription += MakeEhancementText(enhancementDatas[i], enhancementGrades[i]);

		if (i == enhancementDatas.Num() - 1)break;
		enhancementDescription += FString::Printf(TEXT("\n"));
	}

	EhancementDescription->SetText(FText::FromString(enhancementDescription));

	SetVisibility(ESlateVisibility::HitTestInvisible);
	bOn = 1;
}

void UUW_InventoryPopup::Off()
{
	SetVisibility(ESlateVisibility::Collapsed);
	bOn = 0;
}
