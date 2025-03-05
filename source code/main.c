#include "library.h"
#include "information.h"

int main( int argc , char **argv ) {

    clock_t start_time = clock() ;

    if ( argc < 2 ) {
        fprintf( stderr , "Tip: [lookupw_eather.o] [mod => defalute || customize]" ) ;
        exit(1) ;
    }
    
    char file_path[64] ;
    memset( file_path , 0 , sizeof( file_path ) ) ;
    if ( strncmp( argv[1] , "defalute" , 8 ) != 0 ) {
        snprintf( file_path , 64 , "F-D0047-0%c%c.json", argv[1][strlen( argv[1] )-2] , argv[1][strlen( argv[1] )-1] ) ;
    } else {
        sprintf( file_path , "defalute.json" ) ;
    }
    FILE *fp = fopen( file_path , "wb" ) ;
    if ( !fp ) {
        fprintf( stderr , "fopen() failed.\n" ) ;
        exit(2) ;
    }

    #if defined(_WIN32)
    WSADATA d ;
    if ( WSAStartup( MAKEWORD(2,2) , &d ) ) {
        fprintf( stderr , "WSAStartup() failed.\n" ) ;
        exit(3) ;
    }
    #endif

    SSL_library_init() ;
    OpenSSL_add_ssl_algorithms() ;
    SSL_load_error_strings() ;

    SSL_CTX *ctx = SSL_CTX_new( TLS_client_method() ) ;
    if ( !ctx ) {
        fprintf( stderr , "SSL_CTX_new() failed.\n" ) ;
        exit(4) ;
    }

    struct addrinfo hints , *peer_addr ;
    memset( &hints , 0 , sizeof(struct addrinfo) ) ;
    hints.ai_socktype = SOCK_STREAM ;
    if ( getaddrinfo( server_addr , server_port , &hints , &peer_addr ) ) {
        fprintf( stderr , "getaddrinfo() failed.\n" ) ;
        exit(5) ;
    }

    SOCKET server = socket( peer_addr->ai_family , peer_addr->ai_socktype , peer_addr->ai_protocol ) ;
    if ( !ISVALIDSOCKET( server ) ) {
        fprintf( stderr , "socket() failed.\n" ) ;
        exit(6) ;
    }
    freeaddrinfo( peer_addr ) ;

    if ( connect( server , peer_addr->ai_addr , peer_addr->ai_addrlen ) ) {
        fprintf( stderr , "connect() failed.\n" ) ;
        exit(7) ;
    }
    
    SSL *ssl = SSL_new( ctx ) ;
    if ( !ssl ) {
        fprintf( stderr , "SSL_new() failed.\n" ) ;
        exit(8) ;
    }

    if ( !SSL_set_tlsext_host_name( ssl , server_addr ) ) {
        fprintf( stderr , "SSL_set_tldext_host_name() failed.\n" ) ;
        exit(9) ;
    }
    
    SSL_set_fd( ssl , server ) ;
    if ( SSL_connect( ssl ) == -1 ) {
        fprintf( stderr , "SSL_connect() filed.\n" ) ;
        exit(10) ;
    }

    char requested_char[KB] = "\0" ;
    char requested_file[12] = "\0" ;
    if ( strncmp( "defalute" , argv[1] , 8 ) != 0 ) {
        strncpy( requested_file , file_path , 11 ) ;
        requested_file[11] = 0 ;
    } else {
        strncpy( requested_file , "F-D0047-073" , 11 ) ;
        requested_file[strlen(requested_file)] = 0 ;
    }

    if ( strlen( authenticate ) != 40 || strncmp( authenticate , "[Please enter your authenticate]" , 32 ) == 0 ) {
        fprintf( stderr , "Please enter your authenticate.\n" ) ;
        exit(11) ;
    }
    snprintf( requested_char , KB ,
        "%s/%s?Authorization=%s&limit=%lu&format=JSON&sort=time", request_resource , requested_file , authenticate , max_data_amount ) ;
    

    for ( unsigned short int state = 0 ; ; ) {

        if ( state == 0 ) {
RESEND :
            char send_buf[MAX_BUFFER+1] ;
            memset( send_buf , 0 , sizeof( send_buf ) ) ;
            snprintf( send_buf , MAX_BUFFER ,
                "GET %s HTTP/1.1\r\n"
                "Host: %s:%s\r\n"
                "Connection: close\r\n"
                "User-Agent: howpc\r\n"
                "Accept: application/json\r\n"
                "\r\n", requested_char , server_host , server_port ) ;
            
            int send_count = SSL_write( ssl , send_buf , strlen( send_buf ) ) ;
            if ( send_count <= 0 ) {
                goto RESEND ;
            }
        }

        char *read_head = (char *)calloc( KB , sizeof(char) ) ;
        if ( !read_head ) {
            fprintf( stderr , "Out of read buffer.\n" ) ;
            goto END ;
        }
        char *read_buf = read_head ;
        int tab = 0 ;
        unsigned long int chunk_len = 0 ;
        unsigned long int chunk_received = 0 ;
        while(1) {
            int read_count = SSL_read( ssl , read_buf , KB-1 ) ;
            if ( read_count < 1 ) {
                free( read_buf ) ;
                break ;
            }
            read_buf[read_count] = 0 ;

            char *temp = NULL ;
            if ( state == 0 && ( temp = strstr( read_buf , "\r\n\r\n" ) ) ) {
                read_buf = temp+4 ;
                state = 1 ;
            } else if ( state == 2 && ( temp = strstr( read_buf , "0\r\n\r\n" ) ) ) {
                *temp = 0 ;
            }

            if ( state ) {
                if ( state == 1 ) {
                    chunk_len = strtoul( read_buf , &temp , 16 ) ;
                    if ( temp == read_buf ) {
                        continue ;
                    }
                    if ( chunk_len == 0 ) {
                        goto END ;
                    }
                    read_buf = temp ;
                    read_buf += 2 ;
                    state = 2 ;
                }

//          FOR TEST AND DEVELOPE                *************************************************** //
                /*printf( "\n-------------------------------------------------------\n" ) ;
                printf( "%s", read_buf ) ;
                printf( "\n-------------------------------------------------------\n" ) ;*/
// ************************************************************************************************* //


                int write_len = strlen( read_buf ) ;
                if ( write_len ) {
                    for ( unsigned int i = 0 ; i < write_len ; ++i ) {
                        char ch = read_buf[i] ;
                        if ( ch != '\r' && ch != '\n' && ch != '\0' ) {
                            if ( ch == ']' || ch == '}' ) {
                                --tab ;
                                tab = tab <= 0 ? 0 : tab ;
                                fprintf( fp , "\n" ) ;
                                for ( unsigned int j = 0 ; j < tab ; ++j ) {
                                    fprintf( fp , "\t" ) ;
                                }
                            }
                            fprintf( fp , "%c", ch ) ;
                            if ( ch == '[' || ch == '{' ) {
                                ++tab ;
                                fprintf( fp , "\n" ) ;
                                for ( unsigned int j = 0 ; j < tab ; ++j ) {
                                    fprintf( fp , "\t" ) ;
                                }
                            } else if ( ch == ',' ) {
                                fprintf( fp , "\n" ) ;
                                for ( unsigned int j = 0 ; j < tab ; ++j ) {
                                    fprintf( fp , "\t" ) ;
                                }
                            }
                        }
                    }
                    chunk_received += write_len ;
                    // printf( "rec : %lu ; len : %lu ; write : %d\n", chunk_received , chunk_len , write_len ) ;
                    if ( chunk_received >= chunk_len ) {
                        chunk_received = 0 ;
                        chunk_len = 0 ;
                        state = 1 ;
                    }
                }
            }
            memset( read_head , 0 , KB ) ;
            read_buf = read_head ;
            
        }
        goto END ;
    }

END:
    clock_t end_time = clock() ;
    fprintf( stdout , "finish. Time spend : %lf msec.\n", (double)( end_time-start_time )/CLOCKS_PER_SEC ) ;
    
    SSL_shutdown( ssl ) ;
    CLOSESOCKET( server ) ;
    SSL_free( ssl ) ;
    SSL_CTX_free( ctx ) ;

    fclose( fp ) ;

    #if defined(_WIN32)
    WSACleanup() ;
    #endif

    return 0 ;
}