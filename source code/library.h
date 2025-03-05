#ifndef __LIB_C_H
#define __LIB_C_H

/*******************************************     NEED TO BE TYPIED IN MAIN FUCTION     *******************************************/
#if defined(_WIN32) /*for windows*/
    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
    #endif
    // for network
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    // for ERROR type
    #include <winerror.h>
    #include <conio.h> //  _kbhit() : whether terminal input is waiting
    // for Microsoft Visual C
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "libssl.lib")
    #pragma comment(lib, "libcrypto.lib")
    #pragma comment(lib, "iphlpapi.lib")
    // if for MinGW : gcc ~.c -o ~.exe -liphlpapi -lws2_32 -lssl -lcrypto
#else /*for unix-based*/
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>
#endif
/*******************************************     NEED TO BE TYPIED IN MAIN FUCTION     *******************************************/

/*******************************************     difference between the Berkeley socket and Winsock APIs     *******************************************/

#if defined(_WIN32)
    #define ISVALIDSOCKET(s) ( (s) != INVALID_SOCKET )
    #define CLOSESOCKET(s) closesocket( (s) )    
    #define GETSOCKETERRORNO() ( WSAGetLastError() )
    #if !defined(IPV6_V6ONNLY)
    #define IPV6_V6ONLY 27
    #endif
#else
    #define SOCKET int
    #define ISVALIDSOCKET(s) ( (s) >= 0 )
    #define CLOSESOCKET(s) close( (s) )  
    #define GETSOCKETERRORNO() ( errno )
#endif

/*******************************************     difference between the Berkeley socket and Winsock APIs     *******************************************/

/*******************************************     for encrypt/decrypt     *******************************************/

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

/*******************************************     for encrypt/decrypt     *******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BUFFER 4095ULL
#define RESPONSE_SIZE 4095ULL

#define NAME_LEN 15
#define PASS_LEN 11
#define ID_LEN 32

#define MB 1048576ULL
#define KB 1024ULL

#endif