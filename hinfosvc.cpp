#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <iostream>
#define UNEXPECTED_ARGUMENT_ERROR 1
#define INVALID_PORT_NUMBER_ERROR 2

std::string convert_to_string(char* c, int length)
{
    std::string s;
    for (int i = 0; i < length; i++)
    {
        s += c[i];
    }
    return s;
}

std::string get_hostname()
{
    FILE* fp = popen("cat /proc/sys/kernel/hostname", "r");
    char buffer [100];
    fgets(buffer, 100, fp);
    fclose(fp);
    std::string hostname = convert_to_string(buffer, strlen(buffer));
    return hostname;
}



std::string get_cpuname()
{
    FILE* fp = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk -F \":\" '{ sub (\" \", \"\", $2); print $2}'", "r");
    char buffer [100];
    fgets(buffer, 100, fp);
    fclose(fp);
    std::string cpuname = convert_to_string(buffer, strlen(buffer));
    return cpuname;
}


std::string get_respond(char* buffer)
{
    std::string request = convert_to_string(buffer, strlen(buffer));
    std::string respond;
    if (request.rfind("GET /hostname HTTP", 0) == 0)
    {
        respond = get_hostname();
    }
    else if (request.rfind("GET /cpu-name HTTP", 0) == 0)
    {
        respond = get_cpuname();
    }
    else
    {
        respond = "400 bad request\n";
    }
    return respond;
}

int start_connection(int argc, char* argv[])
{
    char* errorPtr;
    long port = strtol(argv[1], &errorPtr, 10);
    if (*errorPtr != '\0' || port > 65535 || port < 0)
    {
        std::cerr << "invalid port number!";
        return 1;
    }
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0)
    {
        std::cerr << "error while creating socket!";
        return 1;
    }
    int sockopt = 1;
    setsockopt(client_socket, IPPROTO_TCP, (SO_REUSEPORT|SO_REUSEADDR), &sockopt, sizeof(int));
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    auto addrlen = sizeof (address);
    if (bind(client_socket, (struct sockaddr*)&address, addrlen) < 0)
    {
        std::cerr << "error while binding socket!";
        return 1;
    }
    if (listen (client_socket, 10) < 0)
    {
        std::cerr << "error while listening to socket!";
        return 1;
    }
    while (true)
    {
        int connection = accept (client_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        std::string message = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
        char buffer[100];
        read(connection, buffer, 100);
        message = message + get_respond(buffer);
        write (connection, message.c_str(), message.length());
        close (connection);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "unexpected argument!";
        return 1;
    }
    start_connection(argc, argv);
    close (client_socket);
}
