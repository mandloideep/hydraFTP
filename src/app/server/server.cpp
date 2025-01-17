#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include "../common/include/env_config.hpp"
#include "./include/constants.hpp"

std::string ls(std::string directory)
{
    DIR *d = opendir(directory.c_str());
    struct dirent *dir;
    std::string files;

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                files += dir->d_name;
                files += "\n";
            }
        }
        closedir(d);
    }

    return files;
}

int main()
{
    const int PORT = EnvConfig::getServerPort();
    const int BUFFER_SIZE = EnvConfig::getServerBufferSize();
    const int BACKLOG = EnvConfig::getServerBacklogSize();

    struct stat st;
    memset(&st, 0, sizeof(st));
    if (stat(ServerConstants::STORE_DIR.c_str(), &st) == -1)
    {

        mkdir(ServerConstants::STORE_DIR.c_str(), 0700);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // server fd
    if (sockfd == -1)
    {
        std::cerr << "socket creation failed: " << std::strerror(errno) << "\n";
        return 1;
    }
    std::cout << "Socket created\n";

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int yes = ServerConstants::SOCKET_OPTION_YES;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    { // to avoid "port already in use" error
        std::cerr << "setsockopt() failed: " << std::strerror(errno) << "\n";
        close(sockfd);
        return 1;
    }

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "failed binding to port: " << PORT << " " << std::strerror(errno) << "\n";
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        std::cerr << "failed listening: " << std::strerror(errno) << "\n";
        close(sockfd);
        return 1;
    }

    std::cout << "Server listening on port: " << PORT << "\n";

    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    while (1)
    {

        int clientfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrSize); // client fd

        if (clientfd == -1)
        {
            std::cerr << "failed accept(): " << std::strerror(errno) << "\n";
        }

        std::cout << "client connected: Address=" << inet_ntoa(clientAddr.sin_addr) << " Port=" << ntohs(clientAddr.sin_port) << "\n";

        char buffer[BUFFER_SIZE];

        // recieve name
        int bytesRecv = recv(clientfd, buffer, sizeof(buffer), 0);

        if (bytesRecv <= 0)
        {
            std::cerr << "client disconnected: " << std::strerror(errno) << "\n";
            break;
        }

        std::string clientname(buffer, bytesRecv);

        std::cout << "Received from client: \n";
        std::cout << "Name: " << clientname << "\n";
        // std::cout<<"Message from client: "<<message<<"\n";
        std::string folderdir = ServerConstants::STORE_DIR + "/" + clientname;
        if (stat(folderdir.c_str(), &st) == -1)
        {

            mkdir(folderdir.c_str(), 0700);
        }
        memset(buffer, 0, sizeof(buffer));

        while (1)
        {
            // recieve option
            bytesRecv = recv(clientfd, buffer, sizeof(buffer), 0);

            if (bytesRecv <= 0)
            {
                std::cerr << "client disconnected: " << std::strerror(errno) << "\n";
                break;
            }

            std::string option(buffer, bytesRecv);
            std::cout << "Option selected: " << option << "\n";

            memset(buffer, 0, sizeof(buffer));

            if (option == "ls")
            {
                std::string directory = std::string(ServerConstants::STORE_DIR + "/") + clientname;
                std::string files = ls(directory);

                if (files.empty())
                {
                    files = "Empty directory";
                }

                if (files.size() < static_cast<std::string::size_type>(BUFFER_SIZE))
                {
                    strcpy(buffer, files.c_str());
                }
                else
                {
                    std::cerr << "Directory files is too large for buffer\n";
                    close(clientfd);
                    continue;
                }

                // Send the directory files to the client
                int bytesSent = send(clientfd, buffer, strlen(buffer), 0);
                if (bytesSent == -1)
                {
                    std::cerr << "Failed sending directory files: " << std::strerror(errno) << "\n";
                    break;
                }
                std::cout << "Sent directory files to client\n";
            }
            else if (option == "upload")
            {
                bytesRecv = recv(clientfd, buffer, sizeof(buffer), 0); // recv file metadata (filename:filesize)
                if (bytesRecv <= 0)
                {
                    std::cerr << "client disconnected: " << std::strerror(errno) << "\n";
                    break;
                }

                std::string metadata(buffer, bytesRecv);

                int partition = metadata.find(":");
                std::string filename = metadata.substr(0, partition);
                int filesize = std::stoi(metadata.substr(partition + 1));

                std::cout << "Filename: " << filename << "\n";
                std::cout << "Filesize: " << filesize << "\n";

                int bytesSent = send(clientfd, "OK", 2, 0);
                if (bytesSent == -1)
                {
                    std::cerr << "Failed sending ACK: " << std::strerror(errno) << "\n";
                    break;
                }

                std::string filepath = folderdir + "/" + filename;

                int filefd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (filefd == -1)
                {
                    std::cerr << "Failed creating file: " << std::strerror(errno) << "\n";
                    break;
                }

                int remaining = filesize;
                while (remaining > 0)
                {
                    bytesRecv = recv(clientfd, buffer, std::min(remaining, BUFFER_SIZE), 0);
                    if (bytesRecv == 0)
                    {
                        std::cerr << "Client disconnected while transferring file\n";
                        break;
                    }
                    if (bytesRecv < 0)
                    {
                        std::cerr << "Error receiving file data: " << std::strerror(errno) << "\n";
                        close(filefd);
                        break;
                    }
                    write(filefd, buffer, bytesRecv);
                    remaining -= bytesRecv;
                }

                std::cout << "File received and saved to " << filepath << "\n";
                close(filefd);
            }
            else
            {
            }
        }
        close(clientfd);
    }

    close(sockfd);

    return 0;
}