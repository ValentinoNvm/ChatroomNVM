#pragma once
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <vector>
//classe per inizializzazione socket lato client

class Config{
public:
    void ConfigAdddr(sockaddr_in* &addr);
     void ChooseOption(int &scelta);

};

class Client{
public:
    int InitSocket(int domain, int type, int protocol );
    int ConnectSocket(int sock, sockaddr* addr , size_t address);

};

//classe per comunicare con il server
class Server{
public:
    int ChooseComm(int sock, const void* buff, size_t dim, int flags);
    int snd(int sock, const void* buff, size_t size, int flags);
    int rcv(int sock,  void* buff, size_t size, int flags);

};

class FileManager{
public:
    void ChooseFile( std::string &nome);
    std::streampos CalcSendFile(std::streampos size, std::string name);
    int MultipleSend(int socket, std::streampos size, std::vector<char>& buffer, std::string nome);
    void SendFile(std::ifstream& file, std::vector <char> buffer, int socket);
};



int Checksock(int func);
