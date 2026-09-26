/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/	

#include<stdio.h>
#include <curses.h>
#include <time.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>
#include <string.h>

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

void serveur_appli (char *service);   /* programme serveur */
static void jouer_une_partie(int socket);


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}


/******************************************************************************/
static void jouer_une_partie(int socket)
{
    char tampon[256];
    int nb_lus;
    int bien_places = 0;
    int mal_places = 0;
    char couleurs[] = "rjvbocgf"; /* rouge, jaune, vert, bleu, orange, cyan, gris, fuchsia */

    const char *ansi_couleurs[] = {
        "\033[31m",      // rouge
        "\033[33m",      // jaune
        "\033[32m",      // vert
        "\033[34m",      // bleu
        "\033[38;5;208m",// orange
        "\033[36m",      // cyan
        "\033[90m",      // gris
        "\033[35m"       // fuschia (magenta)
    };

    nb_lus = h_reads(socket, tampon, 1);
    if (nb_lus <= 0) {
        return;
    }
    tampon[nb_lus] = '\0';

    int n_couleurs = atoi(tampon);
    if (n_couleurs < 3 || n_couleurs > 9) {
        return;
    }

    printf("Nouvelle partie avec %d couleurs\n", n_couleurs);

    char rep[n_couleurs + 1];
    for (int i = 0; i < n_couleurs; i++) {
        int index = rand() % (int)strlen(couleurs);
        rep[i] = couleurs[index];
    }

    rep[n_couleurs] = '\0';
    printf("Le mot secret est : %s\n", rep);
    fflush(stdout);

    bool game = TRUE;
    while (game) {
        char n_guess[n_couleurs + 1];
        char message[255];
        char len_msg[4];
        char statut = 'r';
        int etat[n_couleurs];
        int util_rep[n_couleurs];
        int util_guess[n_couleurs];

        n_guess[n_couleurs] = '\0';
        for (int i = 0; i < n_couleurs; i++) {
            util_rep[i] = 0;
            util_guess[i] = 0;
            etat[i] = 0;
        }

        nb_lus = h_reads(socket, n_guess, n_couleurs);
        if (nb_lus <= 0) {
            break;
        }
        n_guess[nb_lus] = '\0';

        bien_places = 0;
        mal_places = 0;

        /* Comptage des bien placees */
        for (int i = 0; i < n_couleurs; i++) {
            if (rep[i] == n_guess[i]) {
                bien_places++;
                util_rep[i] = 1;
                util_guess[i] = 1;
                etat[i] = 2;
            }
        }

        /* Comptage des mal placees */
        for (int i = 0; i < n_couleurs; i++) {
            if (util_guess[i] == 0) {
                for (int j = 0; j < n_couleurs; j++) {
                    if (rep[j] == n_guess[i] && util_rep[j] == 0 && util_guess[i] == 0) {
                        mal_places++;
                        util_rep[j] = 1;
                        util_guess[i] = 1;
                        etat[i] = 1;
                        break;
                    }
                }
            }
        }

        int offset = snprintf(message, sizeof(message), "\033[1mCorrection :\033[0m ");
        for (int i = 0; i < n_couleurs && offset < (int)sizeof(message) - 1; i++) {
            const char *bg = "\033[40m";
            const char *fg = "\033[37m";

            if (etat[i] == 2) {
                bg = "\033[42m";
            } else if (etat[i] == 1) {
                bg = "\033[43m";
            }

            for (int j = 0; couleurs[j] != '\0'; j++) {
                if (n_guess[i] == couleurs[j]) {
                    fg = ansi_couleurs[j];
                    break;
                }
            }

            offset += snprintf(message + offset, sizeof(message) - offset, "%s%s•\033[0m ", bg, fg);
        }

        if (offset < (int)sizeof(message) - 1) {
            snprintf(message + offset, sizeof(message) - offset, " BP:%d MP:%d", bien_places, mal_places);
        }

        printf("Bien placees: %d, Mal placees: %d\n", bien_places, mal_places);

        if (bien_places == n_couleurs) {
            printf("Partie terminee: combinaison trouvee.\n");
            statut = 'c';
            game = FALSE;
        }

        snprintf(len_msg, sizeof(len_msg), "%03d", (int)strlen(message));
        h_writes(socket, &statut, 1);
        h_writes(socket, len_msg, 3);
        h_writes(socket, message, strlen(message));
    }
}

/******************************************************************************/	
void serveur_appli(char *service)

/* Procedure correspondant au traitement du serveur de votre application */

{
    int socket_ecoute, socket;
    pid_t pid_fils;
    int nb_connex = 5;
    struct sockaddr_in *p_adr_serveur, adr_client;

    srand((unsigned int)time(NULL));
    signal(SIGCHLD, SIG_IGN);

    socket_ecoute = h_socket(AF_INET, SOCK_STREAM);
    adr_socket(service, NULL, SOCK_STREAM, &p_adr_serveur);
    h_bind(socket_ecoute, p_adr_serveur);
    h_listen(socket_ecoute, nb_connex);

    printf("Serveur concurrent en attente sur le port %s (file max: %i)\n", service, nb_connex);

    while (1) {
        socket = h_accept(socket_ecoute, &adr_client);
        if (socket < 0) {
            printf("Echec accept, attente du client suivant...\n");
            continue;
        }

        pid_fils = fork();
        if (pid_fils < 0) {
            printf("Echec fork, fermeture de la connexion client.\n");
            h_close(socket);
            continue;
        }

        if (pid_fils == 0) {
            h_close(socket_ecoute);
            srand((unsigned int)(time(NULL) ^ getpid()));
            printf("Client connecte, debut de partie (fils %d).\n", getpid());
            jouer_une_partie(socket);
            h_close(socket);
            exit(0);
        }

        h_close(socket);
        printf("Client delegue au fils %d, serveur pret pour d'autres clients.\n", pid_fils);
    }
}
