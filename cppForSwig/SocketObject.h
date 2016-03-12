////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  Copyright (C) 2016, goatpig.                                              //
//  Distributed under the MIT license                                         //
//  See LICENSE-MIT or https://opensource.org/licenses/MIT                    //                                      
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _SOCKETOBJ_H
#define _SOCKETOBJ_H

#include <sys\types.h>
#include <string>
#include <sstream>
#include <stdint.h>
#include <functional>

#include "FcgiMessage.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>

#define WRITETOSOCKET(a, b, c) send(a, b, c, NULL)
#define READFROMSOCKET(a, b, c) recv(a, b, c, NULL)

#else
#include <socket.h>
#define closesocket close

#define WRITETOSOCKET(a, b, c) write(a, b, c)
#define READFROMSOCKET(a, b, c) read(a, b, c)

#define SOCKET void*;
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////
class BinarySocket
{
   friend class HttpSocket;
   friend class FcgiSocket;

private:
   const int maxread_ = 65536;
   
   struct sockaddr serv_addr_;
   const string addr_;
   const string port_;

   bool isFcgi_ = false;
   int fcgiRequestID_ = -1;

private:   
   SOCKET open(void);
   void close(SOCKET);
   void write(SOCKET, const char*, uint32_t);
   char* read(SOCKET);

public:
   BinarySocket(const string& addr, const string& port);

   virtual string writeAndRead(const string&);
};

///////////////////////////////////////////////////////////////////////////////
class HttpSocket : public BinarySocket
{
   friend class FcgiSocket;

private:
   string http_header_;

private:
   int32_t makePacket(char** packet, const char* msg);
   virtual string getMessage(const char* msg);

public:
   HttpSocket(const BinarySocket&);
   virtual string writeAndRead(const string&);
};

///////////////////////////////////////////////////////////////////////////////
class FcgiSocket : public HttpSocket
{
private:
   void addStringParam(const string& name, const string& val);
   FcgiMessage makePacket(const char*);
   string getMessage(const char* msg);

public:
   FcgiSocket(const HttpSocket&);
   string writeAndRead(const string&);
};

#endif