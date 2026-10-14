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

#include "Sockets.h"

class TcpConnector
{
public:
	TcpConnector();
	TcpConnector(const FString& AppId, const FString& ApiKey);
	
	bool SendData(const FString& DataToSend);

	void CloseConnection();
	void DisposeConnection();
	bool ConnectToServer(const FString& IP, const int32 Port);
	void TryReadData();
	bool IsConnected = false;

private:
	FSocket* Socket = nullptr;
	FString AppId = "";
	FString ApiKey = "";
};
