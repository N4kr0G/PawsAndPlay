#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PawsPlayerController.generated.h"

// Adds the HUD and the two action keys: E = watch ad, F = feed.
UCLASS()
class APawsPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void OnWatchAd();
	void OnFeed();
	class APawsGameMode* GM() const;
};
