#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "CustomDamageType.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EReactionType : uint8
{
	None UMETA(ToolTip = "Nothing happened or Just glowing character's materials"),
	Normal UMETA(ToolTip = "Play Hit Montage"),
	KnockBack UMETA(ToolTip = "Play KnockBack Montage with tick force"),
	KnockDown UMETA(ToolTip = "Play KnockDown Montage with tick force"),
	Max
};

UCLASS()
class DUNGEON_API UCustomDamageType : public UDamageType
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageType)
		EReactionType ReactionType;
};
