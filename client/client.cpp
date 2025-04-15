#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include "functions.h"
#define BUFFER_SIZE 1024

int main(){

    Client cl; Server serv; FileManager fm; Config cf;

    std::string  nome;  //stringa nome file

    std::vector<char> buffer (BUFFER_SIZE); //vettore per inviare file
    std::vector <char> m1 (BUFFER_SIZE); std::vector <char> m2 (BUFFER_SIZE);
    int scelta = 0;  //scelta comunicazione

    int sockettone = cl.InitSocket(AF_INET, SOCK_STREAM, 0);

    //configurazione indirizzo
    sockaddr_in indirizzo;
    sockaddr_in * ptr = &indirizzo;
    cf.ConfigAdddr(ptr);

    cl.ConnectSocket(sockettone, (struct sockaddr*)&indirizzo, sizeof(indirizzo));


    while(true){
        cf.ChooseOption(scelta); //scelgo cosaf are
        if (scelta == 9){
            serv.ChooseComm(sockettone, reinterpret_cast<const char*>(&scelta), sizeof(scelta), 0);
            break;
        }

        serv.ChooseComm(sockettone, reinterpret_cast<const char*>(&scelta), sizeof(scelta), 0);

        switch(scelta){
            case 0: {
                fm.ChooseFile(nome);
                //devo inviare al server la scelta di voler inviare un file..

                //apertura file
                std::ifstream file (nome, std::ios::in | std::ios::binary | std::ios::ate);

                if(file.is_open()){

                    std::streampos size = file.tellg();

                    file.seekg(0, std::ios::beg);

                    fm.MultipleSend(sockettone, size, buffer, nome); //qua dentro ci sono 4 send

                    fm.SendFile(file, buffer, sockettone); //invio file

                    file.close();
                }
                else std::cout << "impossibile aprire il file";

                break;
            }

            case 1: {
                std::cout << "hai scelto i messaggi";

                //serv.snd(sockettone, reinterpret_cast<const char*>(&scelta), sizeof(scelta),0); //comunicazione scelta al server

                while(true){
                    std::cout << std::endl;
                    std::cout << "tu: ";
                    std::cin.getline(m1.data(), m1.size());
                    if(std::string(m1.data()) == "exit"){
                        serv.snd(sockettone, m1.data(), m1.size(),0);
                    }
                    serv.snd(sockettone, m1.data(), m2.size(),0);
                    serv.rcv(sockettone, m2.data(), m2.size(),0);
                    std::cout << "Server: " << m2.data() << std::endl;
                }
                break;
            }
            default:
                std::cout << "operazione non valida. " << std::endl;
        }
    }

    //chiusura socket
    close(sockettone);
    return 0;
}
