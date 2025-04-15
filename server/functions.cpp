#include "functions.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <fstream>
int CheckRecv(int l)
{
    if(l == -1){
        std::cout << "errore: " << strerror(errno) << std::endl;
    }
    else{
        std::cout << "operazione completata con successo." << std::endl;
    }
    return l;

}

int Server::InitSocket(int domain, int type, int protocol)
{
    int sock = socket(domain, type, protocol);
    Logger::LogError(sock);

    return sock;
}

void Server::BindSocket(int socket, sockaddr* address, socklen_t length)
{
    bind(socket, address, length);
    Logger::LogError(socket);

}


void Server::StartListenSocket(int socket, int queue)
{
    int list = listen(socket, queue);
    Logger::LogError(list);

}


int Server::AcceptClient(int socket, sockaddr* address, socklen_t* length){

    int acc = accept(socket, address, length);
    Logger::LogError(acc);
    return acc;
}

int Server::CloseConnection(int socket){
    close(socket);
    return 0;
}

int Client::RecvData( void* buff, size_t size, int flags)
{
    int rec = recv(m_socket, buff, size, flags);

    return rec;
}

void Client::SendData(const void* buff, size_t size, int flags)
{
    int snd = send(m_socket, buff, size, flags);
    Logger::LogError(snd);
}



void FileTransfer::RecvFile(std::string nome, std::streampos size, Client& client, void* buff, size_t ss, int flags)
{


    std::streampos totalrec = 0;
    std::ofstream file(nome, std::ios::binary);

    if(file.is_open()){
        std::cout << "file creato, ricevo.. " << std::endl;

        while(totalrec < size){
            std::cout << "ho ricevuto un buffer" << std::endl;
            ssize_t received = client.RecvData(buff, ss, flags);
            if(received <0) break;
            file.write(reinterpret_cast<const char*>(buff), received);
            totalrec +=received;
        }
        file.close();

    }
    else{
        std::cout << "impossibile aprire file per scrittura" << std::endl;
    }

}


std::string FileTransfer::ExtractExtension(const std::string filename)
{
    size_t pos = filename.rfind('.');
    if(pos != std::string::npos){
        return filename.substr(pos); //include il punto
    }
    return "";
}




int Logger::LogError(int sock)
{
    if(sock == -1){
        std::cout << "c'è stato un errore PORCODDIO" << std::endl;
    }
    else{
        std::cout << "tutto apposto daje " << std::endl;
    }
    return sock;
}
