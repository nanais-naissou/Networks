/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/	


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{
    int socket  = h_socket(AF_INET, SOCK_STREAM);
    struct sockaddr_in *p_adr_distant;
    adr_socket(service, serveur, SOCK_STREAM, &p_adr_distant);
    h_connect(socket, p_adr_distant);
    
	int number;
	char message[255];

	while (1) {
        printf("Entrez un nombre n (< 10 et > 3)de couleurs à deviner (= difficulté) : ");
		if (scanf("%d", &number) != 1 || number > 9 || number < 3) {
			int c;
			while ((c = getchar()) != '\n' && c != EOF) {
			}
            printf("Ce n'est pas un nombre valide. Essayez encore.\n");
        } else {
            break;
        }
    }

	sprintf(message, "%d", number); //Envoie le nombre de couleurs à deviner

	h_writes(socket, message, 1);

	bool game = TRUE;

	printf("On est parti pour le jeu avec %d couleurs.\n", number);
	printf("Couleurs: r j v b o c g f\n");

	while (game) {
		char n_guess[number+1];
		char scan_fmt[16];
		printf("Donne moi ton guess : ");
		while(1){
			snprintf(scan_fmt, sizeof(scan_fmt), "%%%ds", number);
			if (scanf(scan_fmt, n_guess) != 1 || strlen(n_guess) != (size_t)number) {
				printf("Donne la bonne taille en vrai chef\n");
			}
			else {
					break;
			}
		}
		h_writes(socket, n_guess, number);

		char statut;
		char len_msg[4];
		char rendu[255];
		int rendu_len;

		if (h_reads(socket, &statut, 1) <= 0) {
			printf("Connexion fermee par le serveur.\n");
			break;
		}

		h_reads(socket, len_msg, 3);
		len_msg[3] = '\0';
		rendu_len = atoi(len_msg);
		if (rendu_len < 0) {
			rendu_len = 0;
		}
		if (rendu_len > 254) {
			rendu_len = 254;
		}

		if (rendu_len > 0) {
			h_reads(socket, rendu, rendu_len);
		}
		rendu[rendu_len] = '\0';

		printf("\n%s\n", rendu);

		if (statut == 'c') {
			printf("Bravo mon gars tu as trouve.\n");
			game = FALSE;
		}
	}
	
    h_close(socket);

 }

/*****************************************************************************/

