/**
* @file version2.c
* @brief Programme pour jouer a snake
* @author Chellali Yanis
* @version 2.0
* @date 10/11/2024
*
* Programme C permettant de bouger le serpent
* il peut tourner sans croiser et grandir
* pour se deplacer, utiliser z q s d, ou les constantes
* HAUT BAS GAUCHE DROITE, et arreter en appuyant
* sur la touche a, ou la constante STOP
* obstacles générés aléatoirement, on peut augmenter leur nombre
* trous dans les bordures permettants de se téléporter
*
*/


//  [librairies]

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

//  [constantes]
//      serpent
#define TAILLE_DEBUT 10
#define TAILLE_MAX_FIN 20
#define TETE "O"
#define CORPS "X"
#define XTETE 40
#define YTETE 20
//      déplacements
#define BAS 's'
#define STOP 'a'
#define HAUT 'z'
#define GAUCHE 'q'
#define DROITE 'd'
//      obstacles
#define NB_PAV 4
#define HAUT_PAV 5
#define LARG_PAV 5
#define HAUT_PLAT 40
#define LARG_PLAT 80
#define OBSTACLE '#'
//      autre
#define VIDE ' '
#define ACCELERATION 5000
#define TEMPORISATION 100000
#define POM '6'

// [variables globales]
char plato[LARG_PLAT][HAUT_PLAT];
int tailleSerpent = TAILLE_DEBUT;
int vitesse = TEMPORISATION;

// [procédures]
void gotoXY(int x, int y) { 
    printf("\033[%d;%df", y, x);
}

int kbhit(){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere present
		
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}

void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}
/**
* @brief Procedure qui dessine les pavés
* @param x coordonnee X du coin du pavé
* @param Y coordonnee Y du coin du pavé
*/
void dessinerPaves(int x, int y) {
    int ht, lg;
    for (lg = x; lg < x + LARG_PAV && lg < LARG_PLAT; lg++) {
        for (ht = y; ht < y + HAUT_PAV && ht < HAUT_PLAT; ht++) {
            if (lg > 0 && ht > 0) { // Éviter de dessiner sur les bordures
                plato[lg][ht] = OBSTACLE;
            }
        }
    }
}

/**
* @brief Procedure qui place les pavés
*/
void placementPaves() {
    int nbpaves;
    int x, y;

    for (nbpaves = 0; nbpaves < NB_PAV; nbpaves++) {
        do {
            x = rand() % (LARG_PLAT - LARG_PAV - 2) + 2; // de 1 à LARG_PLAT - LARG_PAV - 1
            y = rand() % (HAUT_PLAT - HAUT_PAV - 2) + 2; // On empêche les blocs d'apparaitre sur les bordures
        } while ((y < YTETE) && (y + HAUT_PAV >= YTETE)); // Éviter la position initiale du serpent

        dessinerPaves(x, y);
    }
}
/**
* @brief Procedure qui place les pommes
*/
void ajouterPomme()
{
	int x=0, y=0;
	x = (rand() % HAUT_PLAT - 2) + 2;
	y = (rand() % LARG_PLAT - 2) + 2;
	while (plato[y][x] != VIDE) //vérification que l'emplacement n'est pas déjà occupé
	{
		x = (rand() % HAUT_PLAT - 2) + 2; //au hasard à l'intérieur du plateau
		y = (rand() % LARG_PLAT - 2) + 2;
	}
	plato[y][x] = POM;
    gotoXY(y, x);
    printf("%c", POM);
}

/**
* @brief Procédure qui fait les trous
*/
void genererTrous()
{
	plato[0][HAUT_PLAT / 2] = VIDE;
	plato[LARG_PLAT - 1][HAUT_PLAT / 2] = VIDE;
	plato[LARG_PLAT / 2][0] = VIDE;
	plato[LARG_PLAT / 2][HAUT_PLAT - 1] = VIDE;
}

/**
* @brief Procedure qui initialise le plateau
*/
void dessinerPlateau() {
    int i, j;
    for (j = 0; j < HAUT_PLAT; j++) {
        for (i = 0; i < LARG_PLAT; i++) {
            gotoXY(i + 1, j + 1); // Ajustement pour correspondre à la console
            printf("%c", plato[i][j]);
        }
    }
    //ajout des pommes
    ajouterPomme();
}




/**
* @brief Procedure qui dessine le plateau
*/
void initPlateau() {
    int i, j; // i et j pour les boucles
    for (i = 0; i < LARG_PLAT; i++) {
        for (j = 0; j < HAUT_PLAT; j++) {
            // Si la case est sur le bord, ajouter un '#'
            if (i == 0 || j == 0 || i == LARG_PLAT - 1 || j == HAUT_PLAT - 1) { 
                plato[i][j] = OBSTACLE; 
            } else {
                plato[i][j] = VIDE;
            }
        }
    }
    //ajout des trous
    genererTrous();

}


/**
* @brief Procedure qui dessine le serpent avec les coordonnees
* @param x coordonnee X de la tete du serpent
* @param Y coordonnee Y de la tete du serpent
* @param lesX tableau des coordonnees X du corps du serpent
* @param lesY tableau des coordonnees Y du corps du serpent
*/
void dessineSerpent(int x, int y, int lesX[TAILLE_MAX_FIN + 1], int lesY[TAILLE_MAX_FIN + 1]) {
    int i;
    gotoXY(x, y); // Coordonnees de la tete
    printf(TETE);
    gotoXY(lesX[tailleSerpent], lesY[tailleSerpent]);
    printf("%c", VIDE); // Efface l'arriere du serpent

    for (i = tailleSerpent; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1]; // Tableau de coordonnees pour le corps
    }
    lesX[0] = x;
    lesY[0] = y;

    for (i = 1; i <= tailleSerpent; i++) { // Dessine le corps
        gotoXY(lesX[i], lesY[i]);
        printf(CORPS);
        gotoXY(0,0);
        printf("%c", OBSTACLE);
    }
}

/**
* @brief Procedure qui fait avancer le serpent avec les coordonnees
* @param x coordonnee X de la tete du serpent
* @param Y coordonnee Y de la tete du serpent
* @param lesX tableau des coordonnees X du corps du serpent
* @param lesY tableau des coordonnees Y du corps du serpent
* @param direction en gros c'es la direction
* @param collision booléen qui vérifie la collision
*/
void progresser(int x, int y, int lesX[TAILLE_MAX_FIN + 1], int lesY[TAILLE_MAX_FIN + 1], char direction, bool *collision, bool *mange) {
    /* Les deux variables sont comparees pour eviter d'aller dans la direction opposee*/
    char ancienneDirection = DROITE, nouvelleDirection = DROITE;
    int i;
    int nbPommesMangees = 0;
    dessineSerpent(x, y, lesX, lesY);

    while ((direction != STOP)&&(*collision==0)&&(nbPommesMangees!=(TAILLE_MAX_FIN - TAILLE_DEBUT))) {
        usleep(vitesse);

        // Gestion de l'entree clavier pour changer de direction
        if (kbhit()) {
            nouvelleDirection = getchar();
            // On empeche le serpent de tourner dans la direction opposee
            if ((nouvelleDirection == HAUT && ancienneDirection != BAS) ||
                (nouvelleDirection == BAS && ancienneDirection != HAUT) ||
                (nouvelleDirection == GAUCHE && ancienneDirection != DROITE) ||
                (nouvelleDirection == DROITE && ancienneDirection != GAUCHE) ||
                nouvelleDirection == STOP) {
                direction = nouvelleDirection;
            }
        }

        // Mettre a jour les coordonnees selon la direction
        switch (direction) {
            case DROITE: x++;
            break;
            case GAUCHE: x--;
            break;
            case HAUT: y--;
            break;
            case BAS: y++;
            break;
        }
        if (plato[x-1][y-1]==OBSTACLE){
            *collision = 1;
        }
        for (i = 1; i <= tailleSerpent; i++) { // Commencer à 1 car la tête est à 0
            if (x == lesX[i] && y == lesY[i]) { //si la tête est à la même position qu'un anneau
                *collision = 1; // Collision détectée
            }
        }
        if (plato[x][y] == POM) {
            tailleSerpent++; // Augmenter la taille du serpent
            vitesse -= ACCELERATION; // Augmenter la vitesse

        // Ajouter une nouvelle section à la queue
            lesX[tailleSerpent] = lesX[tailleSerpent - 1];
            lesY[tailleSerpent] = lesY[tailleSerpent - 1];
            plato[x][y] = VIDE; // Effacer la pomme de la grille
            ajouterPomme();     // Ajouter une nouvelle pomme
            nbPommesMangees++;
        }

        
    if(x < 1 && direction == GAUCHE && (y == LARG_PLAT / 2)){
		y = HAUT_PLAT- 1;
	}

	if(y < 1 && direction == HAUT && (x == HAUT_PLAT / 2)){
		x = LARG_PLAT - 1;
	}

        // Verification des limites du terminal
        if (x < 0){
            x = LARG_PLAT-1;
        } 
        if (y <= 1){
            y = HAUT_PLAT-1;
        }
        if (x >= LARG_PLAT){
            x = 0;
        } 
        if (y >= HAUT_PLAT){
            y = 1;
        }
        
        dessineSerpent(x, y, lesX, lesY);
        ancienneDirection = direction;

    }
    if (*collision==0){
        *mange = 1;
    }
}
/**
* @brief Entree du programme
* @return EXIT_SUCCESS : arret normal du programme
* Desactive l'echo du clavier,
* lance la fonction progresser puis reactive l'echo
* message de game over quand le joueur perd
*/
int main() {
    srand(time(NULL));
    disableEcho();
    bool collision = 0;
    bool mange = 0;
    int lesX[TAILLE_MAX_FIN + 1] = {0}, lesY[TAILLE_MAX_FIN + 1] = {0};
    char direction = DROITE; // Debut vers la droite
    system("clear");
    initPlateau();
    placementPaves();
    dessinerPlateau();
    progresser(XTETE, YTETE, lesX, lesY, direction, &collision, &mange);
    if (collision == 1){
        gotoXY(24, 20);
        printf("collision détectée, tu as perdu !");
        usleep(TEMPORISATION*10);
    }
    else if (mange ==1 ){
        gotoXY(24, 20);
        printf("10 pommes mangées, tu as gagné");
        usleep(TEMPORISATION*10);
    }
    enableEcho();
    gotoXY(0, 0);
    system("clear");
    
    EXIT_SUCCESS;
}