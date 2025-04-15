#include "functions.h"
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <fstream>

int Checksock(int func){
    if(func == -1){
        std::cout << "errore di socket" << strerror(errno) << std::endl;
    }
    return 0;
}


void Config::ConfigAdddr(sockaddr_in*& addr){
    addr->sin_family = AF_INET;
    addr->sin_port = htons(8080);
    //inet_pton(AF_INET, "127.0.0.1", &(addr->sin_addr));
    addr->sin_addr.s_addr=htonl(INADDR_ANY);
}

void Config::ChooseOption(int &scelta){
    int tmp;
    std::cout << "cosa vuoi fare?(0: invio file, 1: messaggi, 2: switch mode, 9: exit): ";
    std::cin >> tmp;
    std::cin.ignore();
    scelta = static_cast<uint8_t>(tmp);
}


int Client::InitSocket(int domain, int type, int protocol ){
    int sock = socket(domain, type, protocol);
    Checksock(sock);
    return sock;
}

int Client::ConnectSocket(int sock, sockaddr* addr , size_t address){
    int s =connect(sock, addr, address);
    Checksock(s);
    return s;
}


int Server::ChooseComm(int sock, const void* buff, size_t dim, int flags){
    send(sock, buff, dim, flags);
    Checksock(sock);
    return sock;
}

int Server::snd(int sock, const void* buff, size_t size, int flags){
    int s = send(sock, buff, size, flags);
    Checksock(s);
    return s;
}

int Server::rcv(int sock, void* buff, size_t size, int flags){
    int s = recv(sock, buff, size, flags);
    Checksock(s);
    return s;
}


int FileManager::MultipleSend(int socket, std::streampos size, std::vector<char>& buffer, std::string nome ){
    //invio dimensione file e dimensione buffer
    send(socket, reinterpret_cast<const char*>(&size), sizeof(size), 0); //dimensione file

    u_int32_t buffsize = buffer.size();
    send(socket, reinterpret_cast<const char*>(&buffsize), sizeof(buffsize), 0);  //dimensione buffer

    uint32_t filenamelen=nome.size();
    send(socket, reinterpret_cast<const  char*>(&filenamelen), sizeof(filenamelen), 0 ); //lunghezza nome

    send(socket, nome.c_str(), filenamelen, 0 ); //nome
    return socket;
}

void FileManager::SendFile(std::ifstream& stream, std::vector <char> buffer, int socket){
    while(stream.read(buffer.data(), buffer.size()) || stream.gcount()>0){
        int bytestosend = stream.gcount();
        send(socket, buffer.data(), bytestosend,0);
    }

}


void FileManager::ChooseFile( std::string &nome){
    std::cout << "devi scegliere il nome del file.. " << std::endl;
    std::cin >> nome;
    std::cin.ignore();
}

std::streampos FileManager::CalcSendFile(std::streampos size, std::string name){
    //opening file
    std::ifstream file (name, std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()){
        size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::cout << "Dimensione file: " << size << std::endl;
    }
    return size;
}

