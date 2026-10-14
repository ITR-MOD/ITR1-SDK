/*
* Copyright (c) 2024 bHaptics Inc.
 *
 * This project incorporates components from the bHaptics SDK, which is utilized under the terms of the bHaptics SDK Agreement.
 * For the complete terms and conditions governing the use of the bHaptics SDK, please refer to https://bhaptics.gitbook.io/license-sdk/.
 *
 * Use of the bHaptics SDK is strictly in accordance with the aforementioned agreement. Unauthorized use, sharing, modification,
 * or distribution of the SDK outside the terms of this agreement is prohibited.
 */
#include "TcpConnector.h"

#include "MessageSender.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "Interfaces/IPv4/IPv4Address.h"


TcpConnector::TcpConnector()
{
}

TcpConnector::TcpConnector(const FString& AppId, const FString& ApiKey)
{
	this->AppId = AppId;
	this->ApiKey = ApiKey;
}

bool TcpConnector::SendData(const FString& DataToSend)
{
	if (!IsConnected) {
		return false;
	}
	
	int32 BytesSent = 0;

	// Convert FString to UTF-8
	const FTCHARToUTF8 ConvertToUTF8(*DataToSend);
	const int32 Size = ConvertToUTF8.Length(); // Size without null terminator

	// Send the data
	const bool bIsSent = Socket->Send((uint8*)ConvertToUTF8.Get(), Size, BytesSent);

	// Check if the data was successfully sent and the correct number of bytes were sent
	if (bIsSent && BytesSent == Size)
	{
		return true;
	}
	if (!bIsSent)
	{
		DisposeConnection();
	}
	return false;
}

void TcpConnector::CloseConnection()
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
	DisposeConnection();
}

void TcpConnector::DisposeConnection()
{
	Socket = nullptr;
	IsConnected = false;
}

bool TcpConnector::ConnectToServer(const FString& IP, const int32 Port)
{
	if (Socket) {
		return false;
	}

	FIPv4Address Address;
	const bool Result = FIPv4Address::Parse(IP, Address);

	if (!Result)
	{
		UE_LOG(LogTemp, Log, TEXT("ConnectToServer Invalid IP: %s"), *IP);
		return false;
	}

	const TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Addr->SetIp(Address.Value);
	Addr->SetPort(Port);

	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(
		NAME_Stream, TEXT("default"), false);

	UE_LOG(LogTemp, Log, TEXT("ConnectToServer %s"), *IP);
	IsConnected = Socket->Connect(*Addr);

	if (IsConnected)
	{
		const auto Message = MessageSender::GetInitMessage(
			AppId,
			ApiKey);
		const bool Res = SendData(Message);
	} else
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to Connect %s"), *IP);
		Socket = nullptr;
	}

	
	return IsConnected;
}

void TcpConnector::TryReadData()
{
	if (IsConnected && Socket && Socket->GetConnectionState() == SCS_Connected)
	{
		uint32 Size;
		TArray<uint8> DataBuffer; // Buffer to accumulate data

		while (Socket->HasPendingData(Size))
		{
			TArray<uint8> TempBuffer;
			TempBuffer.SetNumUninitialized(Size);

			int32 BytesRead = 0;
			if (Socket->Recv(TempBuffer.GetData(), Size, BytesRead))
			{
				// Append the received bytes to the buffer
				DataBuffer.Append(TempBuffer.GetData(), BytesRead);
			}
		}

		if (DataBuffer.Num() > 0)
		{
			DataBuffer.Add('\0');
			const FString ReceivedData = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(DataBuffer.GetData())));
        
			// Invoke the delegate with the received data
			UE_LOG(LogTemp, Log, TEXT("HandleReceivedData: %s"), *ReceivedData);
			MessageSender::TryParseServerMessage(ReceivedData);
		}
	}
}
