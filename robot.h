#include <stdio.h>
#include <stdlib.h>

struct robot
{
	char ip[20];
	int x;
	int y;
};
typedef struct robot robot;

void initialiser(robot r); // Initialise le robot

int find(char *fileName, char *strSearch); // Cherche une chaine de caractere dans un fichier

long file_size(const char *filename); // Renvoie la taille d'un fichier

int envoyerABeebotte(char *canal, char *clefCanal, char *ressource, char *data[]); // Envoie les informations sur Beebotte

char* contenuF(const char *filename); // Renvoie le contenu d'un fichier

char* numBeeb(); // Renvoie le numero Beebotte d'un fichier but

char* numBeebPos(); // Renvoie le numero Beebotte d'un fichier position

int coordonneesx(); // Renvoie le x issu d'un fichier texte

int coordonneesy(); // Renvoie le y issu d'un fichier texte