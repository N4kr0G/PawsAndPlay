#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PawsSaveGame.generated.h"

// Player progress kept between sessions (Saved/SaveGames/Paws.sav).
UCLASS()
class UPawsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY() int32 Points = 0;
	UPROPERTY() int32 PortionsSent = 0;
};
