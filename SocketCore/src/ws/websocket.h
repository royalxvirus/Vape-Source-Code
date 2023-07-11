#pragma once

namespace WebSocket
{
enum PacketId
{
    HELLO_LITE = 1,
    HASH_REQUEST = 2,
    HELLO_V4 = 4,
    XOR_KEY = 12,
    CLIENT_INFO_LITE = 49,
    FILE_REQUEST_LOADER = 47,
    SYNC = 50,
    SETTINGS = 51,
    PROFILE = 52,
    FILE_REQUEST_JAR = 53,
    STRINGS = 54,
    ASSETS = 55,
    CLIENT_INFO_V4 = 56
};
} // namespace WebSocket