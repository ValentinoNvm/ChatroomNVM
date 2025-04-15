#pragma once
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include "functions.h"
int CheckRecv(int l);

class Server{
public:
    int InitSocket(int domain, int type, int protocol);
    void BindSocket(int socket, sockaddr* address, socklen_t length);
    void StartListenSocket(int socket, int queue);
    int AcceptClient(int socket, sockaddr* address, socklen_t* length);
    static int CloseConnection(int socket);
};

class Client{
public:
    Client(int socket) : m_socket(socket) { }  //lista inizializzazione costruttore per incapsulamento
    int RecvData(void* buff, size_t size, int flags);
    void SendData(const void* buff, size_t size, int flags);
private:
    int m_socket;
};


class FileTransfer{
public:
    void RecvFile(std::string nome, std::streampos size, Client& client, void* buff, size_t ss, int flags);
    static std::string ExtractExtension(const std::string filename);
    // void CalcName(const std::string name, const std::string name1, const std::string name2);
    int SendFile();
};

class Logger{
public:
    static int LogError(int sock);
};


