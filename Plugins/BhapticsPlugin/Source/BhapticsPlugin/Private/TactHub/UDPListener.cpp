/*
* Copyright (c) 2024 bHaptics Inc.
 *
 * This project incorporates components from the bHaptics SDK, which is utilized under the terms of the bHaptics SDK Agreement.
 * For the complete terms and conditions governing the use of the bHaptics SDK, please refer to https://bhaptics.gitbook.io/license-sdk/.
 *
 * Use of the bHaptics SDK is strictly in accordance with the aforementioned agreement. Unauthorized use, sharing, modification,
 * or distribution of the SDK outside the terms of this agreement is prohibited.
 */
#include "UDPListener.h"
#include "Sockets.h"
#include "MessageSender.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

FSocket* UDPSocket;
int32 UDP_PORT = 15884;

ConnectInfo Invalid = ConnectInfo();

void UDPListener::SetupUDP()
{
	if (UDPSocket != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("UDP Socket already bound to port %d"), UDP_PORT);
		return;
	}
	
	UDPSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_DGram, TEXT("UDPReceiverSocket"), false);
	const FIPv4Address Addr = FIPv4Address::Any;
	const FIPv4Endpoint Endpoint(Addr, UDP_PORT);

	if (UDPSocket->Bind(*Endpoint.ToInternetAddr()))
	{
		UE_LOG(LogTemp, Log, TEXT("UDP Socket bound to port %d"), UDP_PORT);

		UDPSocket->SetNonBlocking(true);
		UDPSocket->SetRecvErr(true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to bind UDP Socket"));
		UDPSocket = nullptr;
	}
}

void UDPListener::DestroyUDP()
{
	if (UDPSocket != nullptr)
	{
		UDPSocket->Close();

		UE_LOG(LogTemp, Log, TEXT("DestroyUDP"));

		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(UDPSocket);
		UDPSocket = nullptr;
	}
}

ConnectInfo UDPListener::TryReceiveUDPData()
{
	if (!UDPSocket) return Invalid;
    
	uint32 Size;
	const TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	while (UDPSocket->HasPendingData(Size))
	{
		TArray<uint8> ReceivedData;
		ReceivedData.SetNumUninitialized(Size);

		int32 BytesRead = 0;
		if (UDPSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *Sender))
		{
			const FString Value = MessageSender::StringFromBinaryArray(ReceivedData);

			const int32 Port = MessageSender::TryParseUDPMessage(Value);

			if (Port < 0)
			{
				return Invalid;
			}


			uint32 SenderIP;
			Sender->GetIp(SenderIP);
			const int32 SenderPort = Sender->GetPort();

			const FString SenderIPStr = FIPv4Address(SenderIP).ToString();

			return ConnectInfo(SenderIPStr, Port);
		}

	}
	return Invalid;
}
