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

#ifndef _SOCKETCORE_RPCSOCKET_H
#define _SOCKETCORE_RPCSOCKET_H

#include <winsock2.h>
#include <ws2tcpip.h>

namespace RPCSocket
{
typedef struct RPCSocketContext
{
    SOCKET skt;
    char *buffer;
    int key;
} * PRPCSocketContext;

// A simple function definition to handle packet IDs.
// Usages include modifying values from the launcher in Lite, and the server socket.
typedef void (*PacketHandler)(IN RPCSocketContext ctx);

//
enum PacketId
{

    GET_FIELD_FORGE = 8,
    GET_METHOD_FORGE = 9,

    UNKNOWN_12 = 12,

    SEND_USERNAME = 21,

    GET_CLASS = 42,
    GET_FIELD = 43,
    GET_METHOD = 44,
    DCALLBACK = 100,

    END_OF_MESSAGE = 200, // client-side marker

    UNKNOWN_201 = 201,
    UNKNOWN_202 = 202,
    UNKNOWN_203 = 203,

    GET_ENCRYPTION_KEY = 300,

    UNKNOWN_400 = 400,

    SEND_VERSION = 500,

    GET_CLASSES = 600,
    GET_STRINGS = 601,

    GET_TEXTURES = 602,
    GET_SETTINGS = 603,

    SEND_SETTINGS = 604,
    GET_PROFILE = 605,
    SELF_DESTRUCT = 606,

    UNKNOWN_609 = 609,

    V4_UPDATE_STATUS = 610,

    // length
    // b64 json
    SYNC_SETTINGS = 613,
};

enum V4PacketIDs
{
};

enum LitePacketIDs
{
    // enabled 0/1
    // unknown (0)
    // if module is gui, DLL responds with '1'
    LITE_UPDATE_MODULE_STATE = 101,

    // module id
    // slider id
    // value (a double e.g. 8.00, will be sent as 800)
    LITE_UPDATE_RANGE_SETTING = 102,

    // module id
    // setting id
    // toggled 0/1
    LITE_UPDATE_TOGGLE_SETTING = 103,

    // length of friends sent, if length is 0, do not read any more information.
    // length of all characters to read (excluding delimiters?)
    // list
    LITE_UPDATE_FRIENDS_STATE = 104,

    // length of macros sent, if length is 0, do not read any more information.
    // item (bind contents)
    // bind (key code)
    // id   (UUID)
    // delay
    // double click 0/1
    // double click delay
    LITE_UPDATE_MACROS = 105,

    // [Window]{<window name>]
    // Pos=<x>,<y>
    // Size=<width>,<height>
    // Collapsed=<0/1>
    LITE_SYNC_GUI_LAYOUT = 110,

    // module id
    // setting id
    // item index
    LITE_UPDATE_DROPDOWN_SETTING = 112,

    // length of blocks sent, if length is 0, do not read any more information.
    // block id
    // hash?
    // block name
    // enabled
    // red
    // green
    // blue
    LITE_UPDATE_SEARCH_BLOCKS = 113,

    // module id
    // setting id?
    // tick
    LITE_RAINBOW_TICK = 114,
};

// Sends a packet ID to the socket, delimited with \n.
void SocketSendPacketId(IN PRPCSocketContext ctx, IN int packetId);

// Sends the contents to the socket XOR'd with the socket's key & delimited with \n.
void SocketSendXORContents(IN PRPCSocketContext ctx, IN char *contents);

// Sends the contents to the socket, delimited with \n.
void SocketSendContents(IN PRPCSocketContext ctx, IN char *contents);

// Recieves contents from the socket XOR'd with the socket's key & delimited with \n.
char *SocketRecieveXORContents(IN PRPCSocketContext ctx);

// Recieves contents from the socket, delimited with \n.
char *SocketRecieveContents(IN PRPCSocketContext ctx);

namespace Client
{
// Connect to the RPC socket on the specified port.
PRPCSocketContext Connect(IN int port);

// Closes the RPC socket.
void CloseSocket(IN PRPCSocketContext ctx);

} // namespace Client

namespace Server
{
// Creates the RPC socket context at a random available port.
PRPCSocketContext CreateSocket(OUT int *ptrPort);

} // namespace Server

}; // namespace RPCSocket

#endif