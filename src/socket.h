#pragma once

#ifndef SOCKET_H_
#define SOCKET_H_

#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct SOCKET_CTX {
	SOCKET socket;
	char* buffer;
	int some_int;
} *PSOCKET_CTX;

char* VAPE_RecieveSocketMessage(PSOCKET_CTX ctx);

void Vape_SendSocketMessage(PSOCKET_CTX ctx, char* payload);

#endif // !SOCKET_H_