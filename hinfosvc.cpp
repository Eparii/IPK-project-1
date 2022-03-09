#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <array>
#include <unistd.h>
#include <iostream>


std::string convert_to_string(char* c, unsigned long length)
{
    std::string s;
    for (unsigned long i = 0; i < length; i++)
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
    FILE* fp = popen(R"(cat /proc/cpuinfo | grep "model name" | head -n 1 | awk -F ":" '{ sub (" ", "", $2); print $2}')", "r");
    char buffer [100];
    fgets(buffer, 100, fp);
    fclose(fp);
    std::string cpuname = convert_to_string(buffer, strlen(buffer));
    return cpuname;
}

std::string get_cpu_usage()
{
    std::array<long long int,10> first_data{};
    std::array<long long int,10> second_data{};
    for (int x = 0; x < 2; x++)
    {
        FILE* fp = popen("cat /proc/stat | awk 'NR==1 {for (i = 1; i < 12; i++) print $i}'", "r");
        char buffer [100];
        std::array<long long int, 10> data{};
        int i = 0;
        while (fgets(buffer, 100, fp) != nullptr)
        {
            // 0 - user | 1 -  nice | 2 - system | 3 - idle | 4 - iowait
            // 5 - irq  | 6 - softirq | 7 - steal | 8 - guest | 9 - guest_nice
            data[i] = strtoll(buffer, nullptr, 10);
            i++;
        }
        x == 0 ? first_data = data : second_data = data;
        sleep (1);
    }
    long long int firstIdle = first_data[3] + first_data[4];
    long long int secondIdle = second_data[3] + second_data[4];
    long long int firstNonIdle = first_data[0] + first_data[1] + first_data[2] + first_data[5] + first_data[6] + first_data[7];
    long long int secondNonIdle = second_data[0] + second_data[1] + second_data[2] + second_data[5] + second_data[6] + second_data[7];
    long long int firstTotal = firstIdle + firstNonIdle;
    long long int secondTotal = secondIdle + secondNonIdle;
    long long int total = secondTotal - firstTotal;
    long long int idled = secondIdle - firstIdle;
    double CPU_percentage = 100*(double)(total - idled) / (double)total;
    std::string cpu_usage = std::to_string(CPU_percentage) + "%\n";
    return cpu_usage;
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
    else if (request.rfind("GET /load HTTP", 0) == 0)
    {
        respond = get_cpu_usage();
    }
    else
    {
        respond = "400 bad request\n";
    }
    return respond;
}

int start_connection(char* argv[])
{
    char* errorPtr;
    long port = strtol(argv[1], &errorPtr, 10);
    if (*errorPtr != '\0' || port > 65535 || port < 0)
    {
        std::cerr << "invalid port number!";
        return 1;
    }
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == -1)
    {
        std::cerr << "error while creating socket!\n";
        return 1;
    }
    int sockopt = 1;
    setsockopt(client_socket, IPPROTO_TCP, (SO_REUSEPORT|SO_REUSEADDR), &sockopt, sizeof(int));
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    auto addrlen = sizeof (address);
    if (bind(client_socket, (struct sockaddr*)&address, addrlen) == -1)
    {
        std::cerr << "error while binding socket!\n";
        return 1;
    }
    if (listen (client_socket, 10) == -1)
    {
        std::cerr << "error while listening to socket!\n";
        return 1;
    }
    while (true)
    {
        int connection = accept (client_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (connection == -1)
        {
            std::cerr << "error while creating connection!\n";
            return 1;
        }
        std::string message = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
        char buffer[100];
        read(connection, buffer, 100);
        message += get_respond(buffer);
        write (connection, message.c_str(), message.length());
        close (connection);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "missing or unexpected argument!\n";
        std::cerr << "usage: ./hinfosvc PORT\n";
        return 1;
    }
    return start_connection(argv);
}
