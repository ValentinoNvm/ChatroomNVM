// ===== client.cpp =====
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <cstdint>    // uint32_t, uint64_t
#include <endian.h>   // ntohl, htonl, be64toh, htobe64

#define PORT        8080
#define BUF_SIZE    1024

// Thread che ricevere messaggi o file dal server
void receive_loop(int sock) {
    while (true) {
        uint32_t raw_type;
        if (recv(sock, &raw_type, sizeof(raw_type), 0) <= 0) break;
        uint32_t type = ntohl(raw_type);

        if (type == 0) {
            // testo
            uint32_t raw_len;
            recv(sock, &raw_len, sizeof(raw_len), 0);
            uint32_t len = ntohl(raw_len);

            char buf[BUF_SIZE];
            recv(sock, buf, len, 0);
            buf[len] = '\0';
            std::cout << "[MSG] " << buf << std::endl;

        } else if (type == 1) {
            // file
            uint32_t raw_fn;
            recv(sock, &raw_fn, sizeof(raw_fn), 0);
            uint32_t fnlen = ntohl(raw_fn);

            char name[256];
            recv(sock, name, fnlen, 0);
            name[fnlen] = '\0';

            uint64_t netfs;
            recv(sock, &netfs, sizeof(netfs), 0);
            uint64_t fsize = be64toh(netfs);

            std::FILE* fp = fopen(name, "wb");
            uint64_t rec = 0;
            char buf[BUF_SIZE];
            while (rec < fsize) {
                ssize_t r = recv(sock, buf, BUF_SIZE, 0);
                if (r <= 0) break;
                fwrite(buf, 1, r, fp);
                rec += r;
            }
            fclose(fp);
            std::cout << "[FILE] ricevuto \"" << name << "\"" << std::endl;
        }
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &srv.sin_addr);

    if (connect(sock, (sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect"); return 1;
    }

    // lancio il thread per ricevere
    std::thread t(receive_loop, sock);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "quit") break;

        if (line.rfind("send ", 0) == 0) {
            // invio file
            std::string fname = line.substr(5);
            FILE* f = fopen(fname.c_str(), "rb");
            if (!f) {
                std::cerr << "Errore: non trovo " << fname << "\n";
                continue;
            }

            // header type
            uint32_t t = htonl(1);
            send(sock, &t, sizeof(t), 0);

            // nome file
            uint32_t fn = htonl(fname.size());
            send(sock, &fn, sizeof(fn), 0);
            send(sock, fname.c_str(), fname.size(), 0);

            // dimensione
            fseek(f, 0, SEEK_END);
            uint64_t size = ftell(f);
            rewind(f);
            uint64_t netfs = htobe64(size);
            send(sock, &netfs, sizeof(netfs), 0);

            // dati
            char buf[BUF_SIZE];
            uint64_t sent = 0;
            while (sent < size) {
                size_t r = fread(buf, 1, BUF_SIZE, f);
                send(sock, buf, r, 0);
                sent += r;
            }
            fclose(f);

        } else {
            // invio messaggio di testo
            uint32_t t = htonl(0);
            send(sock, &t, sizeof(t), 0);

            uint32_t len = htonl(line.size());
            send(sock, &len, sizeof(len), 0);
            send(sock, line.c_str(), line.size(), 0);
        }
    }

    close(sock);
    t.join();
    return 0;
}
