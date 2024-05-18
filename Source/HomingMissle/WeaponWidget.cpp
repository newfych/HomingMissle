// (Hyd-ra)


#include "WeaponWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "Blueprint/WidgetTree.h"

/** Add Ammo Icons To Horizontal Panel */
void UWeaponWidget::AddIconsToPanel(TArray<UTexture2D*> AmmoIcons)
{
	if (AmmoIcons.Num()>0)
	{
		UHorizontalBox* AmmoBox = Cast<UHorizontalBox>(GetWidgetFromName(FName("Ammo_HorizontalBox")));
		if (!AmmoBox) return;
		for (int32 i = 0; i < AmmoIcons.Num(); ++i)
		{
			UTexture2D* AmmoIcon = AmmoIcons[i];
			if (UAmmoIconWidget* IconWidget = WidgetTree->ConstructWidget<UAmmoIconWidget>(AmmoIconWidgetClass))
			{
				AmmoIconArray.Add(IconWidget);
				UHorizontalBoxSlot* NewSlot = AmmoBox->AddChildToHorizontalBox(IconWidget);
				
				//Set Align as Center in Horizontal and Vertical direction.
				NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
				
				IconWidget->SetIconImage(AmmoIcon);
			}
		}
	}
}

void UWeaponWidget::SetActiveIcon(const int32 Index)
{
	for (int32 i = 0; i < AmmoIconArray.Num(); i++)
	{
		if (Index == i)
		{
			AmmoIconArray[i]->SetImageActive(true);
		}
		else
		{
			AmmoIconArray[i]->SetImageActive(false);
		}
	}
}

void UWeaponWidget::SetCrosshairImage(UTexture2D* Image) const
{
	if (UImage* CrossHairImage = Cast<UImage>(GetWidgetFromName(FName("CrossHair_Image"))))
	{
		CrossHairImage->SetBrushFromTexture(Image, false);
	}
}


