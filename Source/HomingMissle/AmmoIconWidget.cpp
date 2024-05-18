// (Hyd-ra)


#include "AmmoIconWidget.h"
#include "Styling/SlateBrush.h"


void UAmmoIconWidget::SetIconImage(UTexture2D* Image)
{
	IconImage = Cast<UImage>(GetWidgetFromName(FName("Ammo_Image")));
	if (IconImage)
	{
		IconImage->SetBrushFromTexture(Image, false);
	}
}

void UAmmoIconWidget::SetImageActive(const bool Active)
{
	IconImage = Cast<UImage>(GetWidgetFromName(FName("Ammo_Image")));
	if (IconImage)
	{
		if (Active)
		{
			IconImage->SetBrushTintColor(FLinearColor(1.f,1.f, 1.f, 1.f));
		}
		else
		{
			IconImage->SetBrushTintColor(FLinearColor(0.1f,0.1f, 0.1f, 0.5f));
		}
	}
}
