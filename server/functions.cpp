#include "functions.h"
#include <cerrno>  // per errno
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <fstream>

int CheckRecv(int l)
{
    if (l == -1) {
        std::cout << "errore: " << strerror(errno) << std::endl; // stampa messaggio
    } else {
        std::cout << "operazione completata con successo." << std::endl; // OK
    }
    return l;
}

int Server::InitSocket(int domain, int type, int protocol)
{
    int sock = socket(domain, type, protocol);
    Logger::LogError(sock); // controlla errore
    return sock;
}

void Server::BindSocket(int socket, sockaddr* address, socklen_t length)
{
    bind(socket, address, length);
    Logger::LogError(socket); // controlla bind
}

void Server::StartListenSocket(int socket, int queue)
{
    listen(socket, queue);
    Logger::LogError(socket); // controlla listen
}

int Server::AcceptClient(int socket, sockaddr* address, socklen_t* length)
{
    int acc = accept(socket, address, length);
    Logger::LogError(acc); // controlla accept
    return acc;
}

int Server::CloseConnection(int socket)
{
    close(socket);
    return 0;
}

int Client::RecvData(void* buff, size_t size, int flags)
{
    int rec = recv(m_socket, buff, size, flags);
    return rec;
}

void Client::SendData(const void* buff, size_t size, int flags)
{
    int snd = send(m_socket, buff, size, flags);
    Logger::LogError(snd); // controlla send
}

void FileTransfer::RecvFile(std::string nome, std::streampos size, Client& client, void* buff, size_t ss, int flags)
{
    std::streampos totalrec = 0;
    std::ofstream file(nome, std::ios::binary);

    if (file.is_open()) {
        std::cout << "file creato, ricevo.. " << std::endl; // avvia ricezione
        while (totalrec < size) {
            std::cout << "ho ricevuto un buffer" << std::endl; // chunk ricevuto
            ssize_t received = client.RecvData(buff, ss, flags);
            if (received < 0) break;
            file.write(reinterpret_cast<const char*>(buff), received);
            totalrec += received;
        }
        file.close(); // chiude file
    }
}

int Logger::LogError(int sock)
{
    if (sock == -1) {
        std::cout << "c'è stato un errore" << std::endl;
    } else {
        std::cout << "tutto apposto daje " << std::endl;
    }
    return sock;
}