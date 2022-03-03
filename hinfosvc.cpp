#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define UNEXPECTED_ARGUMENT_ERROR 1
#define INVALID_PORT_NUMBER_ERROR 2

void print_hostname()
{
    FILE* fp = popen("cat /proc/sys/kernel/hostname", "r");
    char hostname [100];
    fgets(hostname, 100, fp);
    fclose(fp);
    printf ("%s\n", hostname);
}

void print_cpuname()
{
    FILE* fp = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk -F \":\" '{ sub (\" \", \"\", $2); print $2}'", "r");
    char cpuname [100];
    fgets(cpuname, 100, fp);
    fclose(fp);
    printf ("%s\n", cpuname);
}


int main(int argc, char* argv[])
{
    print_cpuname();
    print_hostname();
    if (argc != 2)
    {
        fprintf(stderr, "unexpected argument!\n");
        return 1;
    }
    char* errorPtr;
    long port = strtol(argv[1], &errorPtr, 10);
    if (*errorPtr != '\0' || port > 65535 || port < 0)
    {
        fprintf(stderr, "invalid port number!\n");
        return 1;
    }
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0)
    {
        fprintf(stderr, "error while creating socket!\n");
        return 1;
    }
}