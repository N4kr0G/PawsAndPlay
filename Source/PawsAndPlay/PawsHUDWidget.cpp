#include "PawsHUDWidget.h"
#include "PawsGameMode.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	const FLinearColor Cream(1.f, 0.95f, 0.85f);
	const FLinearColor Orange(1.f, 0.55f, 0.15f);
	const FLinearColor Green(0.35f, 0.9f, 0.45f);
	const FLinearColor Red(1.f, 0.35f, 0.3f);
}

UTextBlock* UPawsHUDWidget::MakeText(int32 Size, const FLinearColor& Color)
{
	UTextBlock* T = WidgetTree->ConstructWidget<UTextBlock>();
	FSlateFontInfo Font = T->GetFont();
	Font.Size = Size;
	T->SetFont(Font);
	T->SetColorAndOpacity(Color);
	T->SetShadowOffset(FVector2D(1.f, 1.f));
	return T;
}

UBorder* UPawsHUDWidget::MakePanel(UWidget* Content, float Alpha)
{
	UBorder* B = WidgetTree->ConstructWidget<UBorder>();
	B->SetBrushColor(FLinearColor(0.05f, 0.03f, 0.02f, Alpha));
	B->SetPadding(FMargin(18.f, 14.f));
	B->SetContent(Content);
	return B;
}

TSharedRef<SWidget> UPawsHUDWidget::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>();
		WidgetTree->RootWidget = Root;

		// Top-left: score card
		UVerticalBox* Score = WidgetTree->ConstructWidget<UVerticalBox>();
		UTextBlock* Title = MakeText(26, Orange);
		Title->SetText(FText::FromString(TEXT("PAWS & PLAY")));
		Score->AddChildToVerticalBox(Title);
		PointsText = MakeText(20, Cream);
		Score->AddChildToVerticalBox(PointsText);
		PortionBar = WidgetTree->ConstructWidget<UProgressBar>();
		PortionBar->SetFillColorAndOpacity(Orange);
		Score->AddChildToVerticalBox(PortionBar)->SetPadding(FMargin(0.f, 6.f));
		PortionText = MakeText(14, Cream);
		Score->AddChildToVerticalBox(PortionText);
		UCanvasPanelSlot* S = Root->AddChildToCanvas(MakePanel(Score));
		S->SetPosition(FVector2D(24.f, 24.f));
		S->SetAutoSize(true);
		PortionBar->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.f, 1.6f), FVector2D::ZeroVector, 0.f));

		// Top-right: remote device status (out-of-game data)
		FeederText = MakeText(16, Cream);
		S = Root->AddChildToCanvas(MakePanel(FeederText));
		S->SetAnchors(FAnchors(1.f, 0.f));
		S->SetAlignment(FVector2D(1.f, 0.f));
		S->SetPosition(FVector2D(-24.f, 24.f));
		S->SetAutoSize(true);

		// Bottom-center: action hints + last feedback
		UVerticalBox* Bottom = WidgetTree->ConstructWidget<UVerticalBox>();
		MessageText = MakeText(18, Cream);
		MessageText->SetJustification(ETextJustify::Center);
		Bottom->AddChildToVerticalBox(MessageText)->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
		UTextBlock* Hints = MakeText(16, Orange);
		Hints->SetJustification(ETextJustify::Center);
		Hints->SetText(FText::FromString(TEXT("[WASD] Gez   [Boşluk] Zıpla   [E] Reklam izle   [F] Mama dök")));
		Bottom->AddChildToVerticalBox(Hints);
		S = Root->AddChildToCanvas(MakePanel(Bottom));
		S->SetAnchors(FAnchors(0.5f, 1.f));
		S->SetAlignment(FVector2D(0.5f, 1.f));
		S->SetPosition(FVector2D(0.f, -24.f));
		S->SetAutoSize(true);

		// Full-screen ad overlay
		AdText = MakeText(40, Cream);
		AdText->SetJustification(ETextJustify::Center);
		AdOverlay = MakePanel(AdText, 0.92f);
		AdOverlay->SetHorizontalAlignment(HAlign_Center);
		AdOverlay->SetVerticalAlignment(VAlign_Center);
		AdOverlay->SetVisibility(ESlateVisibility::Collapsed);
		S = Root->AddChildToCanvas(AdOverlay);
		S->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		S->SetOffsets(FMargin(0.f));
	}
	return Super::RebuildWidget();
}

void UPawsHUDWidget::NativeTick(const FGeometry& Geometry, float DeltaTime)
{
	Super::NativeTick(Geometry, DeltaTime);
	const APawsGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<APawsGameMode>() : nullptr;
	if (!GM || !PointsText)
	{
		return;
	}

	const int32 Cost = FMath::Max(1, GM->PointsPerPortion);
	PointsText->SetText(FText::FromString(FString::Printf(TEXT("Mama Puanı: %d"), GM->Points)));
	PortionBar->SetPercent(FMath::Min(1.f, float(GM->Points) / Cost));
	PortionBar->SetFillColorAndOpacity(GM->Points >= Cost ? Green : Orange);
	PortionText->SetText(FText::FromString(GM->Points >= Cost
		? FString::Printf(TEXT("Hazır! [F] ile 1 porsiyon dök  •  Gönderilen: %d"), GM->PortionsSent)
		: FString::Printf(TEXT("1 porsiyon = %d puan  •  Gönderilen: %d"), Cost, GM->PortionsSent)));

	FeederText->SetText(FText::FromString(GM->FeederInfo));
	FeederText->SetColorAndOpacity(GM->bFeederOnline || GM->FeederUrl.IsEmpty() ? Green : Red);
	MessageText->SetText(FText::FromString(GM->Message));

	const bool bAd = GM->IsAdPlaying();
	AdOverlay->SetVisibility(bAd ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (bAd)
	{
		AdText->SetText(FText::FromString(FString::Printf(
			TEXT("REKLAM\n\nBu reklam bir patiliye mama oluyor\n\n%d"), FMath::CeilToInt(GM->AdSecondsLeft()))));
	}
}
