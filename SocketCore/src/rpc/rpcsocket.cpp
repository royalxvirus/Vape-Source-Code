/**
 * The MIT License (MIT)
 *
 * Copyright (C) 2022 Decencies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include "rpcsocket.h"
#include <memory>

namespace RPCSocket
{
void SocketSendPacketId(PRPCSocketContext ctx, int packetId)
{
    char idBuffer[4];
    _itoa(packetId, idBuffer, 10);
    SocketSendContents(ctx, idBuffer);
}

void SocketSendXORContents(PRPCSocketContext ctx, char *contents)
{
}

void SocketSendContents(PRPCSocketContext ctx, char *contents)
{
    int len = strlen(contents);
    memmove(ctx->buffer, contents, len);
    send(ctx->skt, ctx->buffer, len, 0);
    send(ctx->skt, "\n", 1, 0);
    memset(ctx->buffer, 0, 0x500000);
}

char *SocketRecieveXORContents(PRPCSocketContext ctx)
{
}

char *SocketRecieveContents(PRPCSocketContext ctx)
{
    int recieved;
    char recievedBuffer[8];
    char *packetBuffer = new char[0x32000];
    char *overflowBuffer;
    int index = 0;

    do
    {
        recieved = recv(ctx->skt, recievedBuffer, 1, 0);
        if (recievedBuffer[0] != '\n')
            break;
        if (index >= 0x32000)
        {
            overflowBuffer = new char[index + 0x32000];
            memmove(overflowBuffer, packetBuffer, index);
            memset(packetBuffer, 0, index);
            delete[] packetBuffer;
            packetBuffer = overflowBuffer;
        }
        packetBuffer[index++] = recievedBuffer[0];
    } while (recieved > 0);

    packetBuffer[index] = '\0';

    return packetBuffer;
}

namespace Client
{
PRPCSocketContext Connect(int port)
{
}

void CloseSocket(PRPCSocketContext ctx)
{
    // what de hell boi
    ctx->buffer[0] = 12337;
    ctx->buffer[1] = 49;
    ctx->buffer[2] = 0;

    send(ctx->skt, ctx->buffer, 3, 0);
    send(ctx->skt, "\n", 1, 0);
    memset(ctx->buffer, 0, 0x500000);
    shutdown(ctx->skt, SD_SEND);
    closesocket(ctx->skt);
    WSACleanup();

    free(ctx->buffer);
}
} // namespace Client

namespace Server
{
PRPCSocketContext CreateSocket(int *port)
{
    PRPCSocketContext ctx = {};

    ctx->buffer = NULL;
    ctx->skt = INVALID_SOCKET;
    ctx->key = 0;
    *port = 6666;

    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);

    addrinfo hints = {0};
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo *res = NULL;

    char *portBuf = new char[8];
    _itoa(*port, portBuf, 10);

    if (getaddrinfo("127.0.0.1", portBuf, &hints, &res) == 0)
    {
        SOCKET skt;

        addrinfo *addr = res;
        do
        {
            skt = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            if (skt != INVALID_SOCKET)
            {
                if (connect(skt, addr->ai_addr, addr->ai_addrlen) != INVALID_SOCKET)
                    break;

                closesocket(skt);
                skt = INVALID_SOCKET;
            }

            addr = addr->ai_next;
        } while (addr);

        freeaddrinfo(res);

        if (skt != INVALID_SOCKET)
        {
            ctx->buffer = new char[0x500000];
            ctx->skt = skt;
        }
    }

    return ctx;
}
} // namespace Server

} // namespace RPCSocket