#include <stdio.h>      // printf, sprintf
#include <stdlib.h>     // exit, atoi, malloc, free
#include <unistd.h>     // read, write, close
#include <string.h>     // memcpy, memset
#include <sys/socket.h> // socket, connect
#include <netinet/in.h> // struct sockaddr_in, struct sockaddr
#include <netdb.h>      // struct hostent, gethostbyname

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int envoyerABeebotte(char *canal, char *clefCanal, char *ressource, char *data[])
{
    // data est un tableau de chaines (char[]), c-a-d un tableau de char a deux dimensions
    // printf("data[0] is %s\n",data[0]);
    //printf("data[3] is %s\n",data[3]);
    int i;
    char *host = "api.beebotte.com";
    /* !! TODO remplacer 'testVB' par le canal dans lequel publier (ex: partie12)
        (ici msg est la "ressource" que ce canal attend */
    char path[100] = "/v1/data/write/";
    strcat(path, canal);
    strcat(path, "/");
    strcat(path, ressource);
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total, message_size;
    char *message, response[4096];

    // Necessaire pour envoyer des donnees sur beebottle.com (noter le token du canal a la fin) :
    char headers[300] ="Host: api.beebotte.com\r\nContent-Type: application/json\r\nX-Auth-Token: ";
    strcat(headers, clefCanal);
    strcat(headers, "\r\n"); 
    
    char donnees[4096] = "{\"data\":\""; // "data" est impose par beebotte.com
    for (i=0;i<3;i++)
    {
        strcat(donnees, data[i]);
        strcat(donnees, ",");
    }
    strcat(donnees, data[3]);
    strcat(donnees, "\"}");

    // Taille du message HTTP (requete POST)
    message_size = 0;
    message_size += strlen("%s %s HTTP/1.0\r\n") + strlen("POST") + strlen(path) + strlen(headers);
    message_size += strlen("Content-Length: %d\r\n") + 10 + strlen("\r\n") + strlen(donnees); 
    
    // Aloocation de l'espace memoire pour le message
    message = malloc(message_size);

    // Construction du message POST
    sprintf(message, "POST %s HTTP/1.0\r\n", path); 
    sprintf(message + strlen(message), "%s", headers);
    sprintf(message + strlen(message), "Content-Length: %zu\r\n", strlen(donnees));
    strcat(message, "\r\n");
    strcat(message, donnees);

    // Requete a envoyer
    printf("Request:\n%s\n-------------\n", message);

    // Creation de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }
    
    // Obtention de l'adresse IP
    server = gethostbyname(host);
    if (server == NULL)
    {
        error("ERROR, no such host");
    }

    // Remplissage de la structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80); // Port 80
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    // Connexion de la socket
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }

    // Envoi de la requete
    total = strlen(message);
    sent = 0;
    do
    {
        bytes = write(sockfd,message+sent,total-sent);
        if (bytes < 0)
        {
            error("ERROR writing message to socket");
        }
        if (bytes == 0)
        {
            break;
        }
        sent += bytes;
    }
    while (sent < total);

    // Reception de la reponse
    memset(response, 0, sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do
    {
        bytes = read(sockfd, response+received, total-received);
        if (bytes < 0)
        {
            error("ERROR reading response from socket");
        }
        if (bytes == 0)
        {
            break;
        }
        received += bytes;
    }
    while (received < total);

    if (received == total)
    {
        error("ERROR storing complete response from socket");
    }

    // Fermeture de la socket
    close(sockfd); 

    // Affichage de la reponse
    printf("Response:\n%s\n", response);
    free(message);
    return 0;
}


int main()
{
    char *infoApublier[4]; // exemple de 4 infos a publier
    infoApublier[0] = "type_msg=IP";
    infoApublier[1] = "type_ent=SP";
    infoApublier[2] = "num=2";
    infoApublier[3] = "data=133.35.33.36+1+13";
    // (Attention : strcpy(infoApublier[3],"data=163.11.111.11" ne fonctionne pas car pas d'espace memoire encore alloue a infoApublier[3] ) 
    
    // !! TO DO : mettre ici le nom du "canal" ou on veut envoyer des donneees
    char *canal = "testVB";
    // Par convention dans FAR on parle sur ressource "msg" sur laquelle on envoie une chaine contenant les couples clef:valeur separes par des virgules
    char *ressource = "msg"; 
    // !! TO DO : mettre ci-dessous le token du canal !!
    // canal partie0 : 1494793564147_KNl54g97mG89kQSZ
    // canal testVB : 1494771555601_5SGQdxJaJ8O1HBj4
    char *clefCanal = "1494771555601_5SGQdxJaJ8O1HBj4";

    // Envoi a BeeBotte en methode POST
    envoyerABeebotte(canal, clefCanal, ressource, infoApublier);
}

