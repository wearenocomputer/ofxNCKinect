#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "SocketCS.h"
#pragma comment(lib, "ws2_32.lib")
#include <iostream>

using namespace std;

int NCSocket::nofSockets_= 0;

void NCSocket::Start() {
  if (!nofSockets_) {
    WSADATA info;
    if (WSAStartup(MAKEWORD(2,0), &info)) {
      throw "Could not start WSA";
    }
  }
  ++nofSockets_;
  //std::cout << "socket created"<< std::endl;
}

void NCSocket::End() {
  WSACleanup();
}

NCSocket::NCSocket() : s_(0) {
  Start();
  // UDP: use SOCK_DGRAM instead of SOCK_STREAM
  s_ = socket(AF_INET,SOCK_STREAM,0);

  if (s_ == INVALID_SOCKET) {
    throw "INVALID_SOCKET";
  }

  refCounter_ = new int(1);
}

NCSocket::NCSocket(SOCKET s) : s_(s) {
  Start();
  refCounter_ = new int(1);
};

NCSocket::~NCSocket() {
  if (! --(*refCounter_)) {
    Close();
    delete refCounter_;
  }

  --nofSockets_;
  if (!nofSockets_) End();
}

NCSocket::NCSocket(const NCSocket& o) {
  refCounter_=o.refCounter_;
  (*refCounter_)++;
  s_         =o.s_;

  nofSockets_++;
  
}

NCSocket& NCSocket::operator=(NCSocket& o) {
  (*o.refCounter_)++;

  refCounter_=o.refCounter_;
  s_         =o.s_;

  nofSockets_++;

  return *this;
}

void NCSocket::Close() {
  closesocket(s_);
}

int NCSocket::ReceiveBytesInt() {
	int currentsizeone = 0;
	int rvone = 0;
	char buf[4];
	rvone = recv(s_, buf, 4, 0);
	int value;
	memcpy(&value, buf, 4);
	return value;
}

ofBuffer NCSocket::ReceiveBytesofBuffer(int bytestoreceive) {

	int currentsize = 0;
	int rv = 0;
	
	ofBuffer buffer;
	
	do {
		char buftwo[1024];
		rv = recv(s_, buftwo, 1024, 0);

		if (rv > 0) {
			//cout << "wim says: " << currentsize << " : " << bytestoreceive << " : " << rv << endl;
			currentsize = currentsize + rv;
			buffer.append(buftwo, rv);
			
		}
		else {
			//cout << "wim says ooh where is the client" << endl;
			//Close();
			break;
		}
	} while (currentsize<bytestoreceive);
	//cout << "wim says buffer size: " << buffer.size() << endl;
	return buffer;

}

std::string NCSocket::ReceiveBytes() {
	
  std::string ret;
  char buf[1024];
 
  while (1) {
    u_long arg = 0;
    if (ioctlsocket(s_, FIONREAD, &arg) != 0)
      break;

    if (arg == 0)
      break;

    if (arg > 1024) arg = 1024;

    int rv = recv (s_, buf, arg, 0);
	//cout << rv << endl;

	
    if (rv <= 0) break;

    std::string t;

    t.assign (buf, rv);
    ret += t;

  }
 
  return ret;
}

std::string NCSocket::ReceiveLine() {
  std::string ret;
  while (1) {
    char r;

    switch(recv(s_, &r, 1, 0)) {
      case 0: // not connected anymore;
              // ... but last line sent
              // might not end in \n,
              // so return ret anyway.
        return ret;
      case -1:
        return "";
//      if (errno == EAGAIN) {
//        return ret;
//      } else {
//      // not connected anymore
//      return "";
//      }
    }

    ret += r;
    if (r == '\n')  return ret;
  }
}

void NCSocket::SendLine(std::string s) {
  s += '\n';
  int r = send(s_,s.c_str(),s.length(),0);
 // cout << r << endl;
}

void NCSocket::SendBytes(const char *buf, int len) {
  send(s_,buf,len,0);
}

SocketServer::SocketServer(int port, int connections, TypeSocket type) {
  sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));

  sa.sin_family = PF_INET;             
  sa.sin_port = htons(port);          
  s_ = socket(AF_INET, SOCK_STREAM, 0);
  if (s_ == INVALID_SOCKET) {
    throw "INVALID_SOCKET";
  }

  if(type==NonBlockingSocket) {
    u_long arg = 1;
    ioctlsocket(s_, FIONBIO, &arg);
  }

  /* bind the socket to the internet address */
  if (::bind(s_, (sockaddr *)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR) {
    closesocket(s_);
    throw "INVALID_SOCKET";
  }
  
  listen(s_, connections);        
 
}

NCSocket* SocketServer::Accept() {
  SOCKET new_sock = accept(s_, 0, 0);
  if (new_sock == INVALID_SOCKET) {
    int rc = WSAGetLastError();
    if(rc==WSAEWOULDBLOCK) {
      return 0; // non-blocking call, no request pending
    }
    else {
      throw "Invalid Socket";
    }
  }

  NCSocket* r = new NCSocket(new_sock);
  return r;
}

SocketClient::SocketClient(const std::string& _host, int _port) : NCSocket() {
	host = _host;
	port = _port;
}

bool SocketClient::connect() {
	std::string error;
	hostent *he;
	if ((he = gethostbyname(host.c_str())) == 0) {
		error = strerror(errno);
		cout << "wrong host address" << endl;
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = *((in_addr *)he->h_addr);
	memset(&(addr.sin_zero), 0, 8);
	if (::connect(s_, (sockaddr *)&addr, sizeof(sockaddr))) {
		error = strerror(WSAGetLastError());
		cout << "could not connect to server: " << error << endl;
		return false;
	}
	else {
		cout << "connected" << endl;
		return true;
	}

	return false;
}
SocketSelect::SocketSelect(NCSocket const * const s1, NCSocket const * const s2, TypeSocket type) {
  FD_ZERO(&fds_);
  FD_SET(const_cast<NCSocket*>(s1)->s_,&fds_);
  if(s2) {
    FD_SET(const_cast<NCSocket*>(s2)->s_,&fds_);
  }     

  TIMEVAL tval;
  tval.tv_sec  = 0;
  tval.tv_usec = 1;

  TIMEVAL *ptval;
  if(type==NonBlockingSocket) {
    ptval = &tval;
  }
  else { 
    ptval = 0;
  }

  if (select (0, &fds_, (fd_set*) 0, (fd_set*) 0, ptval) == SOCKET_ERROR) 
    throw "Error in select";
}

bool SocketSelect::Readable(NCSocket const* const s) {
  if (FD_ISSET(s->s_,&fds_)) return true;
  return false;
}