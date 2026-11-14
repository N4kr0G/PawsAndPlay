#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PawsHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UBorder;

// HUD built in C++ (no widget asset): points, progress to next portion, device status, hints, ad overlay.
UCLASS()
class UPawsHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;

private:
	UTextBlock* MakeText(int32 Size, const FLinearColor& Color);
	UBorder* MakePanel(UWidget* Content, float Alpha = 0.55f);

	UPROPERTY() TObjectPtr<UTextBlock> PointsText;
	UPROPERTY() TObjectPtr<UProgressBar> PortionBar;
	UPROPERTY() TObjectPtr<UTextBlock> PortionText;
	UPROPERTY() TObjectPtr<UTextBlock> FeederText;
	UPROPERTY() TObjectPtr<UTextBlock> MessageText;
	UPROPERTY() TObjectPtr<UBorder> AdOverlay;
	UPROPERTY() TObjectPtr<UTextBlock> AdText;
};
