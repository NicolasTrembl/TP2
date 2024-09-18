#include "Graphe.h"
#include <stdio.h>

/* affichage des successeurs du sommet num*/
void afficher_successeurs(pSommet * sommet, const int num) {

    printf("> sommet %d :\n",num);

    pArc arc=sommet[num]->arc;

    while(arc!=NULL) {
        printf(" %d ",arc->sommet);
        arc=arc->arc_suivant;
    }

}

// Ajouter l'arête entre les sommets s1 et s2 du graphe
pSommet* CreerArete(pSommet* sommet,const int s1, const int s2) {
    if (sommet==NULL) return NULL;

    if(sommet[s1]->arc==NULL) {
        pArc Newarc=(pArc)malloc(sizeof(struct Arc));
        Newarc->sommet=s2;
        Newarc->arc_suivant=NULL;
        sommet[s1]->arc=Newarc;
        return sommet;
    }

    pArc temp=sommet[s1]->arc;

    while( !(temp->arc_suivant==NULL)) {
        temp=temp->arc_suivant;
    }

    pArc Newarc=(pArc)malloc(sizeof(struct Arc));
    Newarc->sommet=s2;
    Newarc->arc_suivant=NULL;

    if(temp->sommet>s2) {
        Newarc->arc_suivant=temp->arc_suivant;
        Newarc->sommet=temp->sommet;
        temp->sommet=s2;
        temp->arc_suivant=Newarc;
        return sommet;
    }

    temp->arc_suivant=Newarc;
    return sommet;

}

// créer le graphe
Graphe* CreerGraphe(const int ordre) {
    Graphe * Newgraphe=(Graphe*)malloc(sizeof(Graphe));
    Newgraphe->pSommet = (pSommet*)malloc(ordre*sizeof(pSommet));

    for(int i = 0; i < ordre; i++) {
        Newgraphe->pSommet[i] = (pSommet) malloc(sizeof(struct Sommet));
        Newgraphe->pSommet[i]->valeur=i;
        Newgraphe->pSommet[i]->arc=NULL;
    }
    return Newgraphe;
}

void freeGraphe(Graphe* graphe) {
    if(graphe==NULL) return;


    for (int i = 0; i < graphe->ordre; i++) {
        if (graphe->pSommet[i] == NULL) continue;
        pArc parc_to_delete = graphe->pSommet[i]->arc;
        while(parc_to_delete->arc_suivant!=NULL) {
            const pArc temp = parc_to_delete->arc_suivant;
            free(parc_to_delete);
            parc_to_delete=temp;
        }
        free(graphe->pSommet[i]);
    }

    free(graphe);
}


/* La construction du réseau peut se faire à partir d'un fichier dont le nom est passé en paramètre
Le fichier contient : ordre, taille,orientation (0 ou 1)et liste des arcs */
Graphe * lire_graphe(char * nomFichier) {
    char path[55];
    sprintf(path, "%s.txt", nomFichier);

    FILE * ifs = fopen(path,"r");
    int taille, orientation, ordre, s1, s2;

    printf("Essaie d'ouvrir %s (%s)\n", nomFichier, path);

    if (!ifs) {
        printf("Fichier %s n'existe pas ou n'a pas pu etre ouvert\n", path);
        exit(-1);
    }

    fscanf(ifs,"%d",&ordre);

    Graphe* graphe = CreerGraphe(ordre); // créer le graphe d'ordre sommets

    fscanf(ifs,"%d",&taille);
    fscanf(ifs,"%d",&orientation);

    graphe->orientation=orientation;
    graphe->ordre=ordre;

    // créer les arêtes du graphe
    for (int i = 0; i < taille; ++i) {
        fscanf(ifs,"%d%d",&s1,&s2);
        graphe->pSommet=CreerArete(graphe->pSommet, s1, s2);

        if(!orientation) { // si non oriente, faire arrete inverse
            graphe->pSommet=CreerArete(graphe->pSommet, s2, s1);
        }
    }

    return graphe;
}

/*affichage du graphe avec les successeurs de chaque sommet */
void graphe_afficher(Graphe* graphe) {
    if (graphe == NULL) {
        return;
    }
    printf("graphe - ");

    if (graphe->orientation) {
        printf("oriente\n");
    } else {
        printf("non oriente\n");
    }

    printf("ordre = %d\n",graphe->ordre);

    printf("listes d'adjacence :\n");

    for (int i = 0; i < graphe->ordre; i++) {
        afficher_successeurs(graphe->pSommet, i);
        printf("\n");
    }

}


#pragma region fifo

typedef struct node {
    pSommet value;
    struct node* next;
} Node;

typedef Node* Fifo;

Fifo creerFifo(const pSommet valeur) {
    Fifo first = malloc(sizeof(Node));
    first->next = NULL;
    first->value = valeur;
    return first;
}

Fifo ajouteFifo(Fifo fifo, const pSommet valeur) {
    if (fifo == NULL) return creerFifo(valeur);

    Node* last = fifo;

    while (last != NULL) {
        last = last->next;
    }

    last = malloc(sizeof(Node));
    last->value = valeur;
    last->next = NULL;
    return last;
}

Fifo pop(const Fifo fifo, int* value) {
    const Fifo new_first = fifo->next;


    if (fifo->value == NULL) {
        *value = -1;
    } else {

        *value = fifo->value->valeur;
    }



    free(fifo);



    return new_first;
}

bool fileVide(const Fifo fifo) {
    return fifo == NULL;
}


#pragma endregion

/* BFS
 * Suivant :
 * BFS(graphe G, sommet s) {
 * f=CreerFile();
 * Enfiler(f, s);
 * debut
 * TANT-QUE NON FileVide(f) FAIRE
 *    x=Défiler(f);
 *    Marquer(x)
 *    TANT-QUE ExisteFils(x) FAIRE
 *             z=FilsSuivant(x);
 *             SI NonMarqué(z) Enfiler(f, z);
 *    FIN-TANT-QUE
 * FIN-TANT-QUE
 * fin
 * }
*/

void bfs(Graphe* graphe, pSommet origine) {
    Fifo fifo = ajouteFifo(NULL, origine);

    bool marked[graphe->ordre];



    while (!fileVide(fifo)) {
        int value;
        fifo = pop(fifo, &value);


        marked[value] = true;



        struct Arc* arc = graphe->pSommet[value]->arc;

        while (arc != NULL) {
            arc = arc->arc_suivant;

            if (!marked[arc->sommet]) {
                fifo = ajouteFifo(fifo, graphe->pSommet[arc->sommet]);
                printf("Sommet %d", arc->sommet);
            }

        }
    }
    printf("vide\n");


    free(fifo);


}


int main() {
    Graphe * g;

    char nom_fichier[50];

    printf("entrer le nom du fichier du labyrinthe:");
    gets(nom_fichier);

    g = lire_graphe(nom_fichier);

    ///saisie du numéro du sommet initial pour lancer un BFS puis un DSF
    printf("numero du sommet initial : \n");

    /// afficher le graphe
    graphe_afficher(g);

    printf("affiché\n");

    bfs(g, g->pSommet[0]);

    freeGraphe(g);

    return 0;
}
