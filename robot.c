#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "robot.h"
#include <string.h>
#include <unistd.h>

// Client http
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

//Chercher l'adresse ip dans le fichier
#include <netdb.h>
#include <sys/stat.h> 

void initialiser(robot r)
{
    r.x = 0;
    r.y = 0;
}

int coordonneesx()
{
    int t;
    int xx;
    char* contenu = malloc(sizeof(char)*500);
    char x[2]= "";
    contenu = contenuF("position2.txt"); // contenu contient le contenu du fichier positon2.txt
    t = 0;
    while (t < 500)
    {
        // Si x est entre 0 et 9 inclus
    	if ((contenu[t] == '+')  && (contenu[t+2] == '+')) // Par convention x est entre + et +
    	{
    		x[0] = '0';
    		x[1] = contenu[t+1];	
    		xx = atoi(x); // Transformation de x en entier
    	}
        // Si x est superieur a 9
    	else if ((contenu[t] == '+')  && (contenu[t+3] == '+'))
        {
    		x[0] = contenu[t+1]; 
    		x[1] = contenu[t+2];	
    		xx = atoi(x);
    	}
    	t++;
    }
    return xx;
}

int coordonneesy()
{
    int t;
    int yy;
    char* contenu = malloc(sizeof(char)*500);
    char* y = malloc(sizeof(char)*2);
    contenu = contenuF("position2.txt"); // contenu contient le contenu du fichier positon2.txt
    t = 0;
    while (t < 500)
    {
        // Si y est entre 0 et 9 inclus
    	if ((contenu[t] == '+')  && (contenu[t+2] == '"')) // Par convention y est entre + et +
    	{
    		y[0] = '0';
    		y[1] = contenu[t+1];	
    		yy = atoi(y); // Transformation de y en entier
    	}
        // Si y est superieur a 9
    	else if ((contenu[t] == '+')  && (contenu[t+3] == '"'))
        {
    		y[0] = contenu[t+1];
    		y[1] = contenu[t+2];	
    		yy = atoi(y);
    	}
    	t++;
    }
    return yy;
}

char* numBeebPos() // Recuperation de numBeeb pour determiner l'unicité
{
    int t, v;
    char* contenu = malloc(sizeof(char)*500);
    char* wts = malloc(sizeof(char)*400);
    contenu = contenuF("position2.txt");
    t = 0;
    while (t < 500)
    {
    	if ((contenu[t] == 'w') && (contenu[t+1] == 't') && (contenu[t+2] == 's') ) // wts est un numero unique de la requete
		{
			v = 0;
			t = t+6;
			while(contenu[t]!=',')
			{
				wts[v] = contenu[t];
				t++;
				v++;				
			}			
		}
        t++;
    }
    return wts;
}

char* numBeeb() // Recuperation de numBeeb pour determiner l'unicité
{
    int t, v;
    char* contenu = malloc(sizeof(char)*500);
    char* wts = malloc(sizeof(char)*400);
    contenu = contenuF("but2.txt");
    t = 0;
    while (t < 500)
    {
    	if ((contenu[t] == 'w') && (contenu[t+1] == 't') && (contenu[t+2] == 's') )
    	{
    		v = 0;
    		t = t+6;
    		while(contenu[t] != ',')
    		{
    			wts[v] = contenu[t];
    			t++;
    			v++;    				
    		}    			
    	}
        t++;
    }
    return wts;
}

int find(char *fileName, char *strSearch)
{
    FILE* f;
    char c;
    int found = 0;
    int pos = 0;
    int length;
    f = fopen(fileName, "r");
    if (f == NULL)
    {
        return 0;
    }
    length = strlen(strSearch);
    while (fread(&c, 1, 1, f) > 0 && !found)
    {
        if (c == strSearch[pos])
        {
            pos++;
        }
        else
        {
            if(pos != 0)
            {
                fseek(f, -pos, SEEK_CUR);
                pos = 0;
            }
        }
        found = (pos == length);
    }
    fclose(f);
    return found;
}

char* contenuF(const char *filename)
{
    FILE* fichier = NULL;
    char* chaine = malloc(sizeof(char)*500); // Chaîne vide de taille TAILLE_MAX
    fichier = fopen(filename, "r");
    if (fichier != NULL)
    {
        fgets(chaine, 500, fichier); 
        fclose(fichier);
    }
	return chaine;
} 

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
    char headers[300] = "Host: api.beebotte.com\r\nContent-Type: application/json\r\nX-Auth-Token: ";
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

    // Allocation de l'espace memoire pour le message
    message=malloc(message_size);

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
    
    // Obtention de l'adresse IP
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80); // Port 80
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    // Obtention de l'adresse IP
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
    }
    
    // Envoi de la requete
    total = strlen(message);
    sent = 0;
    do
    {
        bytes = write(sockfd, message+sent, total-sent);
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