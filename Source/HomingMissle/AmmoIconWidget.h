// (Hyd-ra)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "AmmoIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOMINGMISSLE_API UAmmoIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIconImage (UTexture2D* Image);

	void SetImageActive (bool Active);

private:
	UPROPERTY()
	UImage* IconImage;
};
