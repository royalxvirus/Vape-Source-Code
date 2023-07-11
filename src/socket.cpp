#include "socket.h"

#define INCOMING_BUFFER_SIZE 0x3200
#define OUTGOING_BUFFER_SIZE 0x500000

#define PACKET_ID_MAPPING_LOOKUP 0x67

// todo cleanup.
char* VAPE_RecieveSocketMessage(PSOCKET_CTX ctx) {

	int read;
	char* tempBuffer;
	char* returnBuffer;
	int index;
	char readBuffer[8];

	readBuffer[0] = '\0';
	tempBuffer = new char[INCOMING_BUFFER_SIZE];
	index = 0;
	do {
		read = recv(ctx->socket, readBuffer, 1, NULL);
		returnBuffer = tempBuffer;
		if (readBuffer[0] == '\n') break;
		// resize return buffer if payload recieved is larger than the payload size.
		if (INCOMING_BUFFER_SIZE - 1 < index) {
			returnBuffer = new char[index + INCOMING_BUFFER_SIZE];
			memcpy(returnBuffer, tempBuffer, index);
			delete[] tempBuffer;
		}
		returnBuffer[index] = readBuffer[0];
		index = index + 1;
		tempBuffer = returnBuffer;
	} while (0 < read);
	returnBuffer[index] = '\0'; // insert end of buffer.

	return returnBuffer;
}

void Vape_SendSocketMessage(PSOCKET_CTX ctx, char* payload){

	int len;
	int index;

	index = -1;
	do {
		index++;
	} while (payload[index] != '\0');

	len = index;

	memcpy(ctx->buffer, payload, len);

	send(ctx->socket, ctx->buffer, len, 0);
	send(ctx->socket, "\n", 1, 0);

	memset(ctx->buffer, 0, OUTGOING_BUFFER_SIZE);
}

void SocketOpen(PSOCKET_CTX ctx, void* unknown, int i) {

	ctx->socket = INVALID_SOCKET;
	// 2000 for Lite, not used in V4
	ctx->some_int = i;
	ctx->buffer = new char[OUTGOING_BUFFER_SIZE];

	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);


	addrinfo hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result;
	getaddrinfo("127.0.0.1", NULL, &hints, &result);

	addrinfo* next;
	next = result;

	while (next != NULL) {
		ctx->socket = socket(next->ai_family, next->ai_socktype, next->ai_family);
		if (ctx->socket == INVALID_SOCKET) {
			// goto SetupForgeEventListener;
		}
		if (connect(ctx->socket, next->ai_addr, next->ai_addrlen) != -1) break;
		closesocket(ctx->socket);
		ctx->socket = INVALID_SOCKET;
		next = next->ai_next;
	}
	freeaddrinfo(result);

	if (ctx->socket != INVALID_SOCKET) {
		const char opt = '\x01';
		setsockopt(ctx->socket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	}
}

void SocketClose(PSOCKET_CTX ctx, void* param_2, bool param_3)
{
	//undefined2* puVar1;

	if (param_3) {
		//puVar1 = (undefined2*)param_1->buffer;
		//*puVar1 = 0x3031;
		//*(undefined*)(puVar1 + 1) = 0x31;
		ctx->buffer[3] = '\0';
		send(ctx->socket, ctx->buffer, 3, 0);
		send(ctx->socket, "\n", 1, 0);
		memset(ctx->buffer, 0, OUTGOING_BUFFER_SIZE);
	}
	shutdown(ctx->socket, 1);
	closesocket(ctx->socket);
	WSACleanup();
	free(ctx->buffer);
	return;
}

typedef void(*pPacketHandler)(void* param_2, PSOCKET_CTX ctx);


// LiteDLL
pPacketHandler HandleSocketPacket(int packetId) {

	return NULL;

	/*if (packetId < 0x3e9) {
		if (packetId == 1000) {
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8de0;
			//return ppuVar1;
		}
		switch (packetId) {
		case 101:
			return [](void*, PSOCKET_CTX ctx) {

			};
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_LAB_1801d8e58;
			//return ppuVar1;
		case 102:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8e40;
			//return ppuVar1;
		case 103:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8e38;
			//return ppuVar1;
		case 104:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8e50;
			//return ppuVar1;
		case 105:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_LAB_1801d8e48;
			//return ppuVar1;
		case 106:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8e20;
			//return ppuVar1;
		case 107:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_LAB_1801d8e18;
			//return ppuVar1;
		case 108:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_LAB_1801d8e30;
			//return ppuVar1;
		case 110:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8e28;
			//return ppuVar1;
		case 111:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8e00;
			//return ppuVar1;
		case 112:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8df8;
			//return ppuVar1;
		case 113:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_LAB_1801d8e10;
			//return ppuVar1;
		case 114:
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8e08;
			//return ppuVar1;
		}
	}
	else {
		if (packetId == 1001) {
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8dd8;
			//return ppuVar1;
		}
		if (packetId == 1002) {
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_LAB_1801d8df0;
			//return ppuVar1;
		}
		if (packetId == 1003) {
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8de8;
			//return ppuVar1;
		}
		if (packetId == 1004) {
			//ppuVar1 = (undefined**)operator_new(8);
			//*ppuVar1 = (undefined*)&PTR_FUN_1801d8dd0;
			//return ppuVar1;
		}
	}*/
}


void e(void* param_1, void* param_2, PSOCKET_CTX ctx) {
	while (true) {
		int packetId;
		char* _Str;
		pPacketHandler handler;

		while (true) {
			_Str = VAPE_RecieveSocketMessage(ctx);
			packetId = atoi(_Str);
			free(_Str);
			if (packetId == 100) break;
			handler = HandleSocketPacket(packetId);
			if (handler != NULL) {
				handler(param_2, ctx);
				free(handler);
			}
		}
	}
}
