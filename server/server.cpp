// ===== server.cpp =====
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdint>    // uint32_t, uint64_t
#include <endian.h>   // ntohl, htonl, be64toh

#define PORT     8080
#define BUF_SIZE 1024

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(listener, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(listener, 5) < 0) {
        perror("listen"); return 1;
    }

    std::cout << "Server on port " << PORT << "\n";

    std::vector<int> clients;
    fd_set rdset;

    while (true) {
        FD_ZERO(&rdset);
        FD_SET(listener, &rdset);
        int maxfd = listener;

        // aggiungo client
        for (int i = 0; i < (int)clients.size(); ++i) {
            FD_SET(clients[i], &rdset);
            if (clients[i] > maxfd) maxfd = clients[i];
        }

        if (select(maxfd+1, &rdset, NULL, NULL, NULL) < 0) {
            perror("select"); break;
        }

        // nuova connessione
        if (FD_ISSET(listener, &rdset)) {
            int c = accept(listener, NULL, NULL);
            if (c >= 0) {
                clients.push_back(c);
                std::cout << "Cliente " << c << " connesso\n";
            }
        }

        // dati dai client
        for (int i = 0; i < (int)clients.size(); ++i) {
            int c = clients[i];
            if (!FD_ISSET(c, &rdset)) continue;

            uint32_t raw_type;
            ssize_t r = recv(c, &raw_type, sizeof(raw_type), 0);
            if (r <= 0) {
                close(c);
                clients.erase(clients.begin() + i);
                --i;
                continue;
            }
            uint32_t type = ntohl(raw_type);

            if (type == 0) {
                // broadcast testo
                uint32_t raw_len;
                recv(c, &raw_len, sizeof(raw_len), 0);
                uint32_t len = ntohl(raw_len);

                char msg[BUF_SIZE];
                recv(c, msg, len, 0);
                msg[len] = '\0';

                for (int j = 0; j < (int)clients.size(); ++j) {
                    if (clients[j] == c) continue;
                    uint32_t nlen = htonl(len);
                    send(clients[j], &nlen, sizeof(nlen), 0);
                    send(clients[j], msg, len, 0);
                }

            } else if (type == 1) {
                // broadcast file
                uint32_t raw_fn;
                recv(c, &raw_fn, sizeof(raw_fn), 0);
                uint32_t fnlen = ntohl(raw_fn);

                char name[256];
                recv(c, name, fnlen, 0);
                name[fnlen] = '\0';

                uint64_t netfs;
                recv(c, &netfs, sizeof(netfs), 0);
                uint64_t fsize = be64toh(netfs);

                for (int j = 0; j < (int)clients.size(); ++j) {
                    if (clients[j] == c) continue;
                    uint32_t t = htonl(1);
                    send(clients[j], &t, sizeof(t), 0);

                    uint32_t nfn = htonl(fnlen);
                    send(clients[j], &nfn, sizeof(nfn), 0);
                    send(clients[j], name, fnlen, 0);

                    send(clients[j], &netfs, sizeof(netfs), 0);

                    char buf[BUF_SIZE];
                    uint64_t rec = 0;
                    while (rec < fsize) {
                        ssize_t rr = recv(c, buf, BUF_SIZE, 0);
                        if (rr <= 0) break;
                        send(clients[j], buf, rr, 0);
                        rec += rr;
                    }
                }
            }
        }
    }

    // pulizia
    for (int c : clients) close(c); //sperimentazione for strano
    close(listener);
    return 0;
}
