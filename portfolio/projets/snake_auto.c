#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <math.h>


// taille du serpent
#define TAILLE 10 
// dimensions du plateau
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40
// trous dans le plateau
#define TROU_VERTICAL HAUTEUR_PLATEAU/2
#define TROU_HORIZONTAL LARGEUR_PLATEAU/2
// position initiale des têtes des serpents
#define X_INITIAL1 40
#define Y_INITIAL1 14
#define X_INITIAL2 40
#define Y_INITIAL2 26
// nombre de pommes à manger pour gagner
#define NB_POMMES 10
// temporisation entre deux déplacements du serpent (en microsecondes)
#define ATTENTE 100000 //200000
// caractères pour représenter les serpents
#define CORPS 'X'
#define TETE1 '1'
#define TETE2 '2'
// caractères pour les éléments du plateau
#define BORDURE '#'
#define VIDE ' '
#define POMME '6'
// touche pour arrêter le programme
#define STOP 'a'
//nombre de pavés
#define NB_PAVES 6


// positions prédéfinies des pommes
int lesPommesX[NB_POMMES] = {75, 75, 78, 2, 8, 78, 74, 2, 72, 5};
int lesPommesY[NB_POMMES] = {8, 39, 2, 2, 5, 39, 33, 38, 35, 2};

//positions prédéfinies des pavés
int lesPavesX[NB_PAVES] = { 3, 74, 3, 74, 38, 38};
int lesPavesY[NB_PAVES] = { 3, 3, 34, 34, 21, 15};
// définition d'un type pour le plateau : tPlateau
typedef char tPlateau[LARGEUR_PLATEAU + 1][HAUTEUR_PLATEAU + 1];


// Prototypes des fonctions
void initPlateau(tPlateau plateau);// Initialise le plateau avec des bordures et des trous
void dessinerPlateau(tPlateau plateau);// Affiche le plateau à l'écran
void afficher(int, int, char);// Affiche un caractère à une position donnée
void effacer(int x, int y);// Efface un caractère à une position donnée
void dessinerSerpent(int lesX[], int lesY[], char tete);// Dessine un serpent sur le plateau
void progresser1(int lesX[], int lesY[], char *direction, tPlateau plateau, bool *pommeMangee, int cibleX, int cibleY, int lesX2[], int lesY2[]);// Gère le déplacement du serpent 1
void progresser2(int lesX[], int lesY[], char *direction, tPlateau plateau, bool *pommeMangee, int cibleX, int cibleY, int lesX1[], int lesY1[]);// Gère le déplacement du serpent 2
void gotoxy(int x, int y);// Positionne le curseur à une coordonnée spécifique
void ajouterPaves(tPlateau plateau);//ajoute les pavés
int kbhit();// Vérifie si une touche a été pressée
bool estCaseValide(int x, int y, tPlateau plateau, int lesX1[], int lesY1[], int lesX2[], int lesY2[]) ;
void trouverMeilleurChemin(int *prochainX, int *prochainY, int actuelX, int actuelY, int cibleX, int cibleY, tPlateau plateau, int lesX1[], int lesY1[], int lesX2[], int lesY2[]);


int main() {
    int lesX1[TAILLE], lesY1[TAILLE];
    int lesX2[TAILLE], lesY2[TAILLE];
    char direction1 = 'd', direction2 = 'q';
    tPlateau lePlateau;
    bool pommeMangee1 = false, pommeMangee2 = false;
    int nbDeplacements1 = 0, nbDeplacements2 = 0;
    int pommesMangees1 = 0, pommesMangees2 = 0;


    // Initialisation des positions des serpents
    for (int i = 0; i < TAILLE; i++) {
        lesX1[i] = X_INITIAL1 - i;
        lesY1[i] = Y_INITIAL1;
        lesX2[i] = X_INITIAL2 - i;
        lesY2[i] = Y_INITIAL2;
    }

    // Initialisation du plateau et affichage
    initPlateau(lePlateau);
    system("clear");
    dessinerPlateau(lePlateau);

    ajouterPaves(lePlateau);

    // Placement de la première pomme
    int pommeIndex = 0;
    afficher(lesPommesX[pommeIndex], lesPommesY[pommeIndex], POMME);




    // Boucle principale
    while (pommesMangees1 + pommesMangees2 < NB_POMMES) {
        // Vérification de la touche STOP
        if (kbhit()) {
            char touche = getchar();
            if (touche == STOP) {
                break;
            }
        }

        progresser1(lesX1, lesY1, &direction1, lePlateau, &pommeMangee1, lesPommesX[pommeIndex], lesPommesY[pommeIndex], lesX2, lesY2);

        if (pommeMangee1) {
            pommesMangees1++;
            pommeIndex++;
            pommeMangee1 = false;
            if (pommeIndex < NB_POMMES) {
                afficher(lesPommesX[pommeIndex], lesPommesY[pommeIndex], POMME);
            }
        }

        dessinerSerpent(lesX1, lesY1, TETE1);
        nbDeplacements1++;

        progresser2(lesX2, lesY2, &direction2, lePlateau, &pommeMangee2, lesPommesX[pommeIndex], lesPommesY[pommeIndex], lesX1, lesY1);

        if (pommeMangee2) {
            pommesMangees2++;
            pommeIndex++;
            pommeMangee2 = false;
            if (pommeIndex < NB_POMMES) {
                afficher(lesPommesX[pommeIndex], lesPommesY[pommeIndex], POMME);
            }
        }

        dessinerSerpent(lesX2, lesY2, TETE2);
        nbDeplacements2++;
        usleep(ATTENTE);
    }
    // Affichage du résultat
    gotoxy(1, HAUTEUR_PLATEAU + 2);
    printf("Serpent 1 : %d déplacements et %d pommes mangées \n", nbDeplacements1, pommesMangees1);
    printf("Serpent 2 : %d déplacements et %d pommes mangées \n", nbDeplacements2, pommesMangees2);

    return EXIT_SUCCESS;
}


// Initialise le plateau en ajoutant les bordures et les trous
void initPlateau(tPlateau plateau) {
    for (int i = 1; i <= LARGEUR_PLATEAU; i++) {
        for (int j = 1; j <= HAUTEUR_PLATEAU; j++) {
            plateau[i][j] = VIDE;
        }
    }

    // Ajout des bordures avec trous

    for (int i = 1; i <= LARGEUR_PLATEAU; i++) {
        plateau[i][1] = (i == TROU_HORIZONTAL) ? VIDE : BORDURE; // Bordure haute
        plateau[i][HAUTEUR_PLATEAU] = (i == TROU_HORIZONTAL) ? VIDE : BORDURE; // Bordure basse
    }
    for (int j = 1; j <= HAUTEUR_PLATEAU; j++) {
        plateau[1][j] = (j == TROU_VERTICAL) ? VIDE : BORDURE; // Bordure gauche
        plateau[LARGEUR_PLATEAU][j] = (j == TROU_VERTICAL) ? VIDE : BORDURE; // Bordure droite
    }
}

// Affiche le plateau à l'écran
void dessinerPlateau(tPlateau plateau) {
    for (int i = 1; i <= LARGEUR_PLATEAU; i++) {
        for (int j = 1; j <= HAUTEUR_PLATEAU; j++) {
            afficher(i, j, plateau[i][j]);
        }
    }
}

// Affiche un caractère à une position donnée
void afficher(int x, int y, char car) {
    gotoxy(x, y);
    printf("%c", car);
    fflush(stdout);
}

// Efface un caractère à une position donnée
void effacer(int x, int y) {
    afficher(x, y, VIDE);
}

// Dessine un serpent sur le plateau
void dessinerSerpent(int lesX[], int lesY[], char tete) {
    for (int i = 1; i < TAILLE; i++) {
        afficher(lesX[i], lesY[i], CORPS);
    }
    afficher(lesX[0], lesY[0], tete);
}

// Calcule la distance entre deux points
int distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Vérifie si une case est valide (ni pavé, ni corps d'un serpent)
// Vérifie si une case est valide (ni pavé, ni bordure, ni corps du serpent)
// Vérifie si une case est valide (ni bordure, ni corps du serpent1, ni corps du serpent2)
bool estCaseValide(int x, int y, tPlateau plateau, int lesX1[], int lesY1[], int lesX2[], int lesY2[]) {
    if (plateau[x][y] == BORDURE) return false;

    // Vérifie si le serpent1 occupe cette case
    for (int i = 0; i < TAILLE; i++) {
        if (lesX1[i] == x && lesY1[i] == y) return false;
    }

    // Vérifie si le serpent2 occupe cette case
    for (int i = 0; i < TAILLE; i++) {
        if (lesX2[i] == x && lesY2[i] == y) return false;
    }

    return true;
}



// Trouve le meilleur chemin ou une direction alternative
void trouverMeilleurChemin(int *prochainX, int *prochainY, int actuelX, int actuelY, int cibleX, int cibleY, tPlateau plateau, int lesX1[], int lesY1[], int lesX2[], int lesY2[]) {
    int dx[] = {1, -1, 0, 0};  // Directions pour X
    int dy[] = {0, 0, 1, -1};  // Directions pour Y
    int minDistance = 9999;
    bool cheminTrouve = false;

    for (int i = 0; i < 4; i++) {
        int testX = actuelX + dx[i];
        int testY = actuelY + dy[i];

        if (estCaseValide(testX, testY, plateau, lesX1, lesY1, lesX2, lesY2)) {
            int distanceActuelle = distance(testX, testY, cibleX, cibleY);

            if (distanceActuelle < minDistance) {
                minDistance = distanceActuelle;
                *prochainX = testX;
                *prochainY = testY;
                cheminTrouve = true;
            }
        }
    }

    if (!cheminTrouve) {
        *prochainX = actuelX;
        *prochainY = actuelY;
    }
}



// Gère le déplacement du serpent1 avec contournement avancé
// Gère le déplacement du serpent1
// Gère le déplacement du serpent1
void progresser1(int lesX[], int lesY[], char *direction, tPlateau plateau, bool *pommeMangee, int cibleX, int cibleY, int lesX2[], int lesY2[]) {
    effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

    // Décalage du corps du serpent
    for (int i = TAILLE - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Calcule la nouvelle position de la tête
    int prochainX = lesX[0];
    int prochainY = lesY[0];
    trouverMeilleurChemin(&prochainX, &prochainY, lesX[0], lesY[0], cibleX, cibleY, plateau, lesX2, lesY2, lesX, lesY);

    // Vérifie si la nouvelle tête est valide
    if (!estCaseValide(prochainX, prochainY, plateau, lesX, lesY, lesX2, lesY2)) {
        printf("Collision détectée pour le serpent 1 ! Jeu terminé.\n");
        exit(1); // Arrête le jeu ou gérer la fin de partie
    }

    // Mise à jour de la tête
    lesX[0] = prochainX;
    lesY[0] = prochainY;

    // Vérifie si une pomme est mangée
    if (lesX[0] == cibleX && lesY[0] == cibleY) {
        *pommeMangee = true;
    }
}

// Gère le déplacement du serpent2 (logique identique à progresser1)
void progresser2(int lesX[], int lesY[], char *direction, tPlateau plateau, bool *pommeMangee, int cibleX, int cibleY, int lesX1[], int lesY1[]) {
    effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

    // Décalage du corps du serpent
    for (int i = TAILLE - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Calcule la nouvelle position de la tête
    int prochainX = lesX[0];
    int prochainY = lesY[0];
    trouverMeilleurChemin(&prochainX, &prochainY, lesX[0], lesY[0], cibleX, cibleY, plateau, lesX1, lesY1, lesX, lesY);

    // Vérifie si la nouvelle tête est valide
    if (!estCaseValide(prochainX, prochainY, plateau, lesX1, lesY1, lesX, lesY)) {
        printf("Collision détectée pour le serpent 2 ! Jeu terminé.\n");
        exit(1); // Arrête le jeu ou gérer la fin de partie
    }

    // Mise à jour de la tête
    lesX[0] = prochainX;
    lesY[0] = prochainY;

    // Vérifie si une pomme est mangée
    if (lesX[0] == cibleX && lesY[0] == cibleY) {
        *pommeMangee = true;
    }
}


// Positionne le curseur à une coordonnée spécifique
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void ajouterPaves(tPlateau plateau) {
    for (int i = 0; i < NB_PAVES; i++) {
        // Dessiner un pavé de 5x5 à partir des coordonnées (lesPavesX[i], lesPavesY[i])
        for (int dx = 0; dx < 5; dx++) {
            for (int dy = 0; dy < 5; dy++) {
                int x = lesPavesX[i] + dx;
                int y = lesPavesY[i] + dy;
                plateau[x][y] = BORDURE; // Remplir le plateau avec le caractère de bordure
                afficher(x, y, BORDURE); // Afficher chaque cellule du pavé à l'écran
            }
        }
    }
}


// Vérifie si une touche a été pressée
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}