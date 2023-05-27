#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define LIGNE 6
#define COLONNE 7

typedef int t_grille[LIGNE][COLONNE];

int main(int argc, char** argv);
void chargerGrille(char** argv, t_grille grille);

const int VIDE = 0;
const int JOUEUR = 1;
const int ADVERSAIRE = 2;

//Constante
#define PROBA_MAX 100
//Tableau de colonnes
typedef int tableauCol[COLONNE];

//Prototypes
int recherchePion(t_grille g);
int rechercheColonnePion(t_grille g, char pion);
int calculPoss(t_grille g, int col, char pion);
void transferGrille(t_grille g1, t_grille g2);
void triDecroissant(tableauCol tab);
void permuter(int *n1, int *n2);

int chercherLigne(t_grille laGrille, int col);
bool estVainqueur(t_grille laGrille, int lig, int col);

/*
* VOTRE STRATEGIE
*/
int maStrategie(t_grille g)
{
    // Tableau qui contiendra les probabilités de chaque colonne
    tableauCol tabProba;
    // Colonne où le joueur doit jouer son pion
    int col, colonne, NombrePion;

    // On compte le nombre de pions sur la grille
    NombrePion = recherchePion(g);

    // Si aucun pion n'est présent sur la grille, on joue dans la colonne 3
    if (NombrePion==0){
        colonne = 3;
    }
    // Si le joueur possède déjà un pion sur la grille, on joue dans la même colonne que celle où l'adversaire a placé son pion
    else if (NombrePion==JOUEUR){
        colonne = rechercheColonnePion(g, ADVERSAIRE);
    }

    // Si il y'a deux ou plus pions dans la grille, on effectue une recherche de probabilités
    else{
        int n = 0; // Le Nième Max
        for (int i = 0; i < COLONNE; i++){
            // Pour chaque colonne de la grille, on calcule le nombre de combinaisons gagnantes possibles pour le joueur et on l'ajoute au tableau
            tabProba[i] = calculPoss(g, i, JOUEUR);
            // Si l'adversaire a des combinaisons gagnantes possibles dans cette colonne, on multiplie la valeur par 2 avant de l'ajouter au tableau
            tabProba[i] += calculPoss(g, i, ADVERSAIRE) * 2;
        }
        
        triDecroissant(tabProba);

        colonne = tabProba[n]; // Le 1er Max du tableau (car ici, toujours n=0)

        // Tant que si on joue dans cette colonne cela donne la victoire à l'adversaire, on va chercher la Nème meilleur colonne
        while ((g[chercherLigne(g, colonne)+1][colonne]==estVainqueur(g, (chercherLigne(g,colonne)+1), colonne)) && (n<COLONNE)){
            n += 1;
            colonne = tabProba[n];// Le Nième Max du tableau
        }
    }

    // Si la colonne sélectionnée n'est pas pleine, on renverra cette colonne 
    if (g[0][colonne]==VIDE){
        col = colonne;
    }

    // Si la colonne séléctionnée est pleine, on renvoie une colonne au hasard non-pleine
    else {
        int meilleureClonne;
        int i = 0;

        do{
            meilleureClonne = ((long int)&meilleureClonne + i) % COLONNE; // Colonne au "hasard"
            i++;
        }while(g[0][meilleureClonne] != VIDE);

        col= meilleureClonne;
    }

    return col;
}

// La fonction principale reçoit la grille du tour et retourne le coup choisi
// Vous n'avez pas à modifier cette fonction
int main(int argc, char** argv) 
{
    t_grille grille;

    chargerGrille(argv, grille);

    return maStrategie(grille);
}

// Charge la grille du tour actuel
// Vous n'avez pas à modifier cette fonction
void chargerGrille(char** argv, t_grille grille) 
{
    for(int i = 0; i < LIGNE; i++)
        for(int j = 0; j < COLONNE; j++)
            grille[i][j] = atoi(argv[i * COLONNE + j + 1]);
}

// AJOUTS - MAIGNAN Anton  

int recherchePion(t_grille g){
    // Compteur du nombre de pions trouvés
    int compteurPion = 0;
    // Compteur du nombre de pions trouvés
    int lig=0 , col=0;

    // Tant qu'on n'a pas trouvé 2 pions ou que l'on n'a pas parcouru toutes les lignes de la grille
    while ((compteurPion!=2)&&(lig < LIGNE)){
        // Si la case actuelle de la grille contient un pion, on incrémente le compteur
        if (g[lig][col] != VIDE){
            compteurPion++;
        }
        // Si la case actuelle est vide, on passe à la colonne suivante
        else{
            col++;
        }
        // Si on a parcouru toutes les colonnes de la ligne actuelle, on passe à la ligne suivante et on réinitialise le compteur de colonne
        if (col==COLONNE){
            lig++;
            col = 0;
        }
    }
    return compteurPion;
}

int rechercheColonnePion(t_grille g, char pion){
    // Booléen indiquant si le pion a été trouvé ou non
    bool trouve;
    // Variables pour parcourir la grille et le résultat
    int lig, col, res;
    // Tant qu'on n'a pas trouvé le pion ou que l'on n'a pas parcouru toutes les lignes de la grille
    while ((trouve==false)&&(lig < LIGNE)){
        // Si la case actuelle de la grille contient le pion recherché, on met trouve à true et on enregistre la colonne dans res
        if (g[lig][col] == pion){
            trouve = true;
            res = col;
        }
        // Si la case actuelle ne contient pas le pion recherché, on passe à la colonne suivante
        else{
            col++;
        }
        // Si on a parcouru toutes les colonnes de la ligne actuelle, on passe à la ligne suivante et on réinitialise le compteur de colonne
        if (col==COLONNE){
            lig++;
            col = 0;
        }
    }

    return res;
}



int calculPoss(t_grille g, int col, char pion){
    // Grille de simulation qui sera une copie de la grille de jeu actuelle
    t_grille SimGrille;
    // Probabilité de combinaisons gagnantes pour le pion passé en paramètre dans la colonne 
    int prob = 0;
    // On copie la grille de jeu actuelle dans la grille de simulation
    transferGrille(g, SimGrille);
    // On récupère la ligne de la colonne où le pion sera joué
    int lig = chercherLigne(SimGrille, col);
    // On place le pion dans la grille de simulation
    SimGrille[lig][col] = pion;

    // Si le pion gagne la partie, on fixe la probabilité à sa valeur maximale
    if (estVainqueur(SimGrille, lig, col)){
        prob = PROBA_MAX;
    }


    /* Si la probabilité n'est pas déjà fixée à PROBA_MAX, on parcourt la grille de simulation 
    dans toutes les directions à partir de la position du pion pour compter le nombre de pions consécutifs de la même couleur */
    if (prob != PROBA_MAX){
        // On initialise les variables de parcours de la grille à la position du pion
        int l = lig;
        int c = col;

        // On parcourt la grille vers le bas
        l += 1;
        while(SimGrille[l][c]==pion){
            prob += 1;
            l += 1;
        }
        // On réinitialise la variable de parcours de lignes à la position du pion
        l = lig;


        // On parcourt la grille vers la droite
        c += 1;
        while(SimGrille[l][c]==pion){
            prob += 1;
            c += 1;
        }
        // On réinitialise la variable de parcours de colonnes à la position du pion
        c =  col;


        // On parcourt la grille vers la gauche
        c -= 1;
        while(SimGrille[l][c]==pion){
            prob += 1;
            c -= 1;
        }
        // On réinitialise la variable de parcours de colonnes à la position du pion
        c = col;


        // On parcourt la grille en diagonale vers la droite et le bas
        l += 1;
        c += 1;
        while(SimGrille[l][c]==pion){
            prob += 1;
            l += 1;
            c += 1;
        }
        // On réinitialise les variables de parcours de la grille à la position du pion
        l = lig;
        c = col;


        // On parcourt la grille en diagonale vers la gauche et le haut
        l -= 1;
        c -= 1;
        while(SimGrille[l][c]==pion){
            prob += 1;
            l -= 1;
            c -= 1;
        }
        // On réinitialise les variables de parcours de la grille à la position du pion
        l = lig;
        c = col;


        // On parcourt la grille en diagonale vers la droite et le haut
        l += 1;
        c -= 1;
        while(SimGrille[l][c]==pion){
            prob += 1;
            l += 1;
            c -= 1;
        }
        // On réinitialise les variables de parcours de la grille à la position du pion
        l = lig;
        c = col;
        

        // On parcourt la grille en diagonale vers la gauche et le bas
        l -= 1;
        c += 1;
        while(SimGrille[l][c]==pion){
            prob += 1;
            l -= 1;
            c += 1;
        }
        // On réinitialise les variables de parcours de la grille à la position du pion
        l = lig;
        c = col;
    }

    return prob;
}

void transferGrille(t_grille g1, t_grille g2){
    // On parcourt la grille g1
    for (int col=0; col<COLONNE; col++){
        for (int lig=0; lig<LIGNE; lig++){
            // On copie la valeur de la case actuelle de g1 dans la case correspondante de g2
            g2[lig][col] = g1[lig][col];
        }
    }
}

void triDecroissant(tableauCol tab){
    int temp;

    for (int i=0; i<COLONNE-1; i++){
        for (int j=i+1; j<COLONNE; j++){
            if (tab[i]<tab[j]){
                temp = tab[i];
                tab[i] = j;
                tab[j] = temp;
            }
        }
    }
}



// FONCTIONS DU PUISSANCE 4 DE BASE 

bool estVainqueur(t_grille laGrille, int lig, int col){
    // consiste à regarder si une ligne de 4 pions s'est formée autour du pion qui vient de tomber en (lig, col)
    char lePion = laGrille[lig][col];
    int cpt,i,j;

    // regarder la verticale, en comptant le nombre de pions au Sud (inutile de regarder au Nord du pion qui vient de tomber)
    i = lig;
    cpt = 0;
    while (i<LIGNE && laGrille[i][col]==lePion){
        cpt++;
        i++;
    }
    if (cpt>=4){
        return true;
    }
    // regarder l'horizontale, en comptant le nombre de pions à l'Est et à l'Ouest
    j = col;
    cpt = 0;
    // on regarde à l'est
    while (j>=0 && laGrille[lig][j]==lePion){
        cpt++;
        j--;
    }
    j = col+1;
    // on regarde à l'ouest
    while (j<COLONNE && laGrille[lig][j]==lePion){
        cpt++;
        j++;
    }
    if (cpt>=4 ){
        return true;
    }
    // regarder la diagonale descendante, en comptant le nombre de pions au Nord Ouest et au Sud Est
    i = lig;
    j = col;
    cpt = 0;
    // on regarde au Nord Ouest
    while (j>=0 && i>=0 && laGrille[i][j]==lePion){
        cpt++;
        i--;
        j--;
    }
    i = lig+1;
    j = col+1;
    // on regarde au Sud Est
    while (i<LIGNE && j<COLONNE && laGrille[i][j]==lePion){
        cpt++;
        i++;
        j++;
    }
    if (cpt>=4 ){
        return true;
    }
    // regarder la diagonale descendante, en comptant le nombre de pions au Nord Est et au Sud Ouest
    i = lig;
    j = col;
    cpt = 0;
    // on regarde au Nord Est
    while (j<COLONNE && i>=0 && laGrille[i][j]==lePion){
        cpt++;
        i--;
        j++;
    }
    i = lig+1;
    j = col-1;
    // on regarde au Sud Ouest
    while (i<LIGNE && j<COLONNE && laGrille[i][j]==lePion){
        cpt++;
        i++;
        j--;
    }
    if (cpt>=4 ){
        return true;
    }
    return false;
}

int chercherLigne(t_grille laGrille, int col){
    int ligne = -1;

    while (ligne<LIGNE-1 && laGrille[ligne+1][col]==VIDE){
        ligne++;
    }
    return ligne;
}

