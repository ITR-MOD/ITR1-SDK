/*
* Copyright (c) 2024 bHaptics Inc.
 *
 * This project incorporates components from the bHaptics SDK, which is utilized under the terms of the bHaptics SDK Agreement.
 * For the complete terms and conditions governing the use of the bHaptics SDK, please refer to https://bhaptics.gitbook.io/license-sdk/.
 *
 * Use of the bHaptics SDK is strictly in accordance with the aforementioned agreement. Unauthorized use, sharing, modification,
 * or distribution of the SDK outside the terms of this agreement is prohibited.
 */
#pragma once

#include "CoreMinimal.h"

struct ConnectInfo
{
	FString SenderIp;
	int32 Port;
	bool Valid = false;

	ConnectInfo() = default;

	ConnectInfo(const FString& SenderIp, const int32 Port)
		: SenderIp(SenderIp), Port(Port), Valid(true) {}
};

class UDPListener
{
public:
	static void SetupUDP();
	static void DestroyUDP();
	static ConnectInfo TryReceiveUDPData();
private:
};
