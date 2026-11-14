#include "PawsPlayerController.h"
#include "PawsGameMode.h"
#include "PawsHUDWidget.h"
#include "Blueprint/UserWidget.h"

void APawsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController())
	{
		CreateWidget<UPawsHUDWidget>(this, UPawsHUDWidget::StaticClass())->AddToViewport();
	}
}

void APawsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindKey(EKeys::E, IE_Pressed, this, &APawsPlayerController::OnWatchAd);
	InputComponent->BindKey(EKeys::F, IE_Pressed, this, &APawsPlayerController::OnFeed);
}

APawsGameMode* APawsPlayerController::GM() const
{
	return GetWorld()->GetAuthGameMode<APawsGameMode>();
}

void APawsPlayerController::OnWatchAd()
{
	if (APawsGameMode* G = GM()) G->WatchAd();
}

void APawsPlayerController::OnFeed()
{
	if (APawsGameMode* G = GM()) G->RequestFeed();
}
