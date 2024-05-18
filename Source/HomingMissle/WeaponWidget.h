// (Hyd-ra)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "AmmoIconWidget.h"

#include "WeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOMINGMISSLE_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void AddIconsToPanel(TArray<UTexture2D*> AmmoIcons);

	void SetActiveIcon(int32 Index);

	void SetCrosshairImage (UTexture2D* Image) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> AmmoIconWidgetClass;
	
private:
	UPROPERTY()
	TArray<UAmmoIconWidget*> AmmoIconArray;
};
