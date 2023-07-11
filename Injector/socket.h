#pragma once

// TODO: where are the texture packets? 6xx series.

#define PACKET_MAPPING_GET_METHOD_FORGE 9

#define PACKET_MAPPING_GET_FIELD_FORGE 8

#define PACKET_SEND_USERNAME 21

#define PACKET_MAPPING_GET_CLASS_VANILLA 42

#define PACKET_MAPPING_GET_FIELD_VANILLA 43

#define PACKET_MAPPING_GET_METHOD_VANILLA 44

#define PACKET_D_CALLBACK 100 // Unknown packet called from Java thread.

#define PACKET_UNKNOWN_200 200 // Confirmation packet? 

#define PACKET_UNKNOWN_201 201 // Error packet?

#define PACKET_UNKNOWN_202 202 // sent after 400, sends map size, then sends payload of map (forge handlers?) to socket.

#define PACKET_CLIENT_IS_FORGE 300 // Sent to socket if client is running forge.

#define PACKET_UNKNOWN_400 400 // sent before 200 packet, when checking forge stuff. (possibly setting the version)

#define PACKET_SEND_FORGE_VERSION 500 // sent after forge packet (300) in MainInitialization

#define PACKET_GET_CLASSES 600  // Called to get class bytes of a/a, a/b, a/c (and a/d in v3.x - v4.06)

#define PACKET_GET_STRINGS 601 // Sends the string map

#define PACKET_UNKNOWN_602 602

#define PACKET_UNKNOWN_603 603

#define PACKET_SEND_SETTINGS 604 // Send settings to the socket.

#define PACKET_GET_PROFILE 605 // GetProfile packet

#define PACKET_EXIT 606 // exit packet. (followed by SendPacket(exitCode))