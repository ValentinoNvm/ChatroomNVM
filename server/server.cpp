#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "functions.h"


#define BUFFER_SIZE 1024

int main(){


    std::streampos size;

    std::vector <char> risposta (BUFFER_SIZE);
    std::string nome;

    Server init;
    FileTransfer ft;

    //inizializzazione socket
    int sockettone = init.InitSocket(AF_INET, SOCK_STREAM, 0);

    //dichiarazione indirizzo
    sockaddr_in indirizzo;
    indirizzo.sin_family = AF_INET;
    indirizzo.sin_port = htons(8080);
    indirizzo.sin_addr.s_addr = INADDR_ANY;

    //binding del socket
    init.BindSocket(sockettone, (struct sockaddr*)&indirizzo, sizeof(indirizzo));
    init.StartListenSocket(sockettone,5);

    //socket per accettare client
    int acc = init.AcceptClient(sockettone, nullptr, nullptr);
    Client cli(acc); //oggetto legato al client



    //ricevere scelta presumo

    while(true){

        int scelta = 0;
        if(cli.RecvData(reinterpret_cast<void*>(&scelta), sizeof(scelta), 0 )<=0){
            std::cout << "client disconnesso o errore ricezone" << std::endl;
        }
        std::cout << "il client ha scelto: " << scelta << std::endl;
        if(scelta == 9){
            std::cout << "chiusura della connessione richiesta dal client." << std::endl;
            break;
        }
        switch(scelta){


            case 0:{
                // gestione ricezione file
                std::cout << "ricezione file: " << std::endl;

                //ricevimento dimensione totale file
                std::streampos size;
                cli.RecvData(reinterpret_cast<void*>(&size), sizeof(size),0);


                //ricevimento dimensione buffer
                uint32_t buffsize;
                cli.RecvData( reinterpret_cast<void*>(&buffsize), sizeof(buffsize),0);


                std::vector<char> buffer(buffsize);

                //ricevimento nome, lunghezza del file
                uint32_t nome_len;
                cli.RecvData(reinterpret_cast<void*>(&nome_len), sizeof(nome_len),0);

                std::vector<char> nomebuffer(nome_len+1, 0);
                cli.RecvData(nomebuffer.data(), nome_len, 0);

                std::string nomefile (nomebuffer.data(), nome_len);


               std::string destfile = "ricevuto_" + nomefile;

                //ricevimento file intero
                ft.RecvFile(destfile, size, cli, buffer.data(), buffer.size(),0);
                std::cout << "file ricevuto e salvato come:" << destfile <<std::endl;

                break;


            }
            case 1:{
                std::cout << "il client vuole iniziare una conversazione.. mi preparo.." << std::endl;

                while(true){

                    std::vector<char> risposta(BUFFER_SIZE,0);

                    //ricevi messaggio
                    if(cli.RecvData( risposta.data(), risposta.size(),0)<=0){
                        std::cout << "errore o client disconnessod urante la conversazione" << std::endl;

                        break;
                    }
                    std::string msg(risposta.data());
                    if(msg.find("exit")!=std::string::npos){
                        std::cout << "terminazione conversazione da parte del client" << std::endl;

                        break;
                    }
                    std::cout << "Client: "<< msg << std::endl;
                    std::cout << "Rispondi: ";
                    std::vector<char> mess(BUFFER_SIZE,0);

                    //scrivi risposta
                    std::cin.getline(mess.data(), mess.size());

                    //invia risposta
                    cli.SendData( mess.data(), mess.size(),0);

                }

            }
            default:
                std::cout << "operazione non riconosciuta dal client. " << std::endl;
                break;

        }
        std::cout << "sto aspettando la prossima scelta del client. " << std::endl;

    }




    Server::CloseConnection(sockettone);

}




