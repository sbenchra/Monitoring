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

//Chercher l'adresse IP dans le fichier
#include <netdb.h>
#include <sys/stat.h> 

int main ()
{
	/////////// SAISIE DES ADRESSES IP ///////////
	robot r[6];
	int i = 0;
	for(i=0 ; i<6 ; i++)
	{ 
		int j = 0;
		char* c = malloc(sizeof(char)*200);
		initialiser(r[i]);
		printf("Inserez l'adresse ip:\n");
		scanf("%s", c);
		while(j<20)
		{
			r[i].ip[j] = c[j];
			j++;
		}
		r[i].x = 0;
		r[i].y = 0;
		//printf("%d\n\n",r[i].x);
		j = 0;
		//printf("Le robot %d est sur la position %s", i,r[i].ip);
		free(c);
	}

	//////////// ENVOI DE L'ID DE LA PARTIE SUR BEEBOTTE //////////////
	int id = random();
	char mess3[90] = "";
	char idchar[10] = "";
	sprintf(mess3, "data = ");
	sprintf(idchar, "%d", id);
	strcat(mess3, idchar);
	char *infoApublier[4]; // exemple de 4 infos a publier
	infoApublier[0] = "type_msg=IdPartie";
	infoApublier[1] = "type_ent=Monitoring";
	infoApublier[2] = "num=1";
	infoApublier[3] =mess3;
	// 	Ne fonctionne pas car pas d'espace memoire encore alloue a infoApublier[3] ) 
    
	char *channel = "testVB";
	// Par convention dans FAR on parle sur ressource "msg" sur laquelle on envoie une chaine contenant les couples clef:valeur separes par des virgules
	char *ressource = "msg"; 
	    
	// canal partie0 : 1494793564147_KNl54g97mG89kQSZ
	// canal testVB : 1494771555601_5SGQdxJaJ8O1HBj4
	char *channelKey = "1494771555601_5SGQdxJaJ8O1HBj4";

	// Envoie a BeeBotte en methode POST
	envoyerABeebotte(channel, channelKey, ressource, infoApublier);

	// Recherche  d'un caractere dans un fichier, car on va ecrire la sortie du curl dans un fichier
	time_t debut, fin;
	int S1 = 0;
	int S2 = 0;
	debut = time(NULL);
	fin = time(NULL);
	//char* timebis;
	int m = 0;

	char buffer[254] = "";
	strcat(buffer, "curl http://api.beebotte.com/v1/public/data/read/vberry/testVB/msg?limit=1 > but.txt");	// Appel de la derniere information	  
	system(buffer);
	printf(".\n");

	char buffer6[254] = "";
	strcat(buffer6, "tr -d '\r\n' < but.txt > but1.txt"); // Initialise wts	  
	system(buffer6);
	char* buffer7 = malloc(sizeof(char)*13);
	buffer7 = numBeeb();

	char buffer11[254] = "";
	strcat(buffer11, "curl http://api.beebotte.com/v1/public/data/read/vberry/testVB/msg?limit=1 > position.txt"); // Appel de la derniere information	  
	system(buffer11);
	printf(".\n");

	char buffer16[254]= "";
	strcat(buffer16, "tr -d '\r\n' < position.txt > position2.txt"); // Initialise wts	  
	system(buffer16);
	char* buffer17 = malloc(sizeof(char)*13);
	buffer17 = numBeebPos();

	while(fin-debut <= 15*60) // incrementation du temps
	{
		//////////// POSITIIIION //////////
		int p = 0;

		char buffer18[254] = "";
		strcat(buffer18, "curl http://api.beebotte.com/v1/public/data/read/vberry/testVB/msg?limit=1 > position.txt"); // Appel de la derniere information	  
		system(buffer18);
		printf(".\n");

		char buffer19[254] = "";
		strcat(buffer19, "tr -d '\r\n' < position.txt > position2.txt"); // Initialise wts	  
		system(buffer19);
		char* buffer110 = malloc(sizeof(char)*13);
		buffer110 = numBeebPos();
		//printf("%s\n",buffer110);
		//printf("%s\n",buffer17);
		if(strcmp(buffer110, buffer17) != 0)
		{
			printf("%d", find("postion2.txt", "SP"));
			if (find("position2.txt", "SP") ) 
			{
				char* file2 = "position2.txt";
				int d = 0;
				for(d=0 ; d<6 ; d++) // Chercher les joueurs a qui on a ajoute une position
		  		{
		  			//printf("%s\n position",r[d].ip);
		  			if (find(file2, r[d].ip)) // Chercher le robot pour lequel il existe une nouvelle position
					{
						r[d].x = coordonneesx();
						r[d].y=coordonneesy();
					}
				}
			}
			buffer17 = buffer110; // Changement de valeur initiale pour verifier si le numero de la requete a change
		}
		int l = 0;


		//////////////// BUT /////////////////////////
		char buffer8[254] = "";
		strcat(buffer8, "curl http://api.beebotte.com/v1/public/data/read/vberry/testVB/msg?limit=1 > but.txt"); // Appel de la derniere information	  
		system(buffer8);
		printf(".\n");

		char buffer9[254] = "";
		strcat(buffer9, "tr -d '\r\n' < but.txt > but2.txt"); // Initialise wts	  
		system(buffer9);
		char* buffer10 = malloc(sizeof(char)*13);
		buffer10 = numBeeb();

		if(strcmp(buffer10, buffer7) != 0) // Si le numero de la requete "wts" a change
		{
			if (find("but2.txt", "VB")) // Si le dernier ajout est de type VB 
			{
				char* file1 = "but2.txt";
				for(l=0 ; l<3 ; l++) //Chercher les joueurs qui marquent
				{
		   			// printf("%s %d\n",r[l].ip,l);
					if (find(file1,r[l].ip)) 
					{
						S1++; // Incremente le score de la premiere equipe si celle-ci marque			
					}
				}
				for(l=3 ; l<6 ; l++)
				{
					if (find(file1,r[l].ip))
					{
						S2++; // Incremente le score de la seconde equipe si celle-ci marque
					}
		   		}
		   	}
		   	buffer7 = buffer10; // Changement de valeur initiale
		}
		printf("Le score est de : %d - %d\n", S1, S2); // Affichage du score
		m = fin-debut; // Calcul de temps ecoule
		fin = time(NULL);
		if (m < 60)
		{
			printf("Temps écoulé : %d secondes\n", m); // Incrementation du temps ecoule
		}
		else
		{
			int minute  = m/60%60;
			int seconde = m%60;
			printf("Temps écoulé : %d:%d secondes\n", minute, seconde); // Affichage du temps ecoule en minutes
		}
		int q;
		for(q=0 ; q<6 ; q++) // Chercher les joueurs A qui on a ajoute une position
		{
			printf("Le robot %d est sur la position %d.%d\n", q,r[q].x, r[q].y); // Affichage des positions
		}
		sleep(1); // Attendre une seconde
	}
	printf("Partie terminée\n");
	return 0;
}
