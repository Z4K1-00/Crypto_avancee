#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_N 100
#define MAX_K 50

// Fonction pour calculer le poids (nombre de bits à 1) d'un vecteur
int weight(int *vec, int len) {
    int w = 0;
    for (int i = 0; i < len; i++) {
        if (vec[i] == 1) {
            w++;
        }
    }
    return w;
}

// Fonction pour afficher une matrice
void print_matrix(int matrix[MAX_K][MAX_K], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Fonction pour afficher un vecteur
void print_vector(int *vec, int len) {
    for (int i = 0; i < len; i++) {
        printf("%d ", vec[i]);
    }
    printf("\n");
}

// Fonction pour choisir aléatoirement k colonnes parmi n
void choose_random_columns(int *cols, int k, int n) {
    int i, j, temp;
    for (i = 0; i < n; i++) {
        cols[i] = i;
    }
    for (i = 0; i < k; i++) {
        j = i + rand() % (n - i);
        temp = cols[i];
        cols[i] = cols[j];
        cols[j] = temp;
    }
}

// Fonction pour inverser une matrice k x k
int invert_matrix(int mat[MAX_K][MAX_K], int inv[MAX_K][MAX_K], int k) {
    int aug[MAX_K][2*MAX_K];
    int temp;

    // Initialisation de la matrice identitaire
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            inv[i][j] = (i == j) ? 1 : 0;
        }
    }

    // Matrice augmentée
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            aug[i][j] = mat[i][j];
            aug[i][j + k] = inv[i][j];
        }
    }

    // Élimination de Gauss-Jordan
    for (int i = 0; i < k; i++) {
        // Recherche du pivot
        if (aug[i][i] == 0) {
            for (int j = i + 1; j < k; j++) {
                if (aug[j][i] == 1) {
                    // Échange des lignes
                    for (int l = 0; l < 2 * k; l++) {
                        temp = aug[i][l];
                        aug[i][l] = aug[j][l];
                        aug[j][l] = temp;
                    }
                    break;
                }
            }
        }

        // Élimination
        for (int j = 0; j < k; j++) {
            if (i != j && aug[j][i] == 1) {
                for (int l = 0; l < 2 * k; l++) {
                    aug[j][l] ^= aug[i][l];
                }
            }
        }
    }

    // Extraction de l'inverse
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            inv[i][j] = aug[i][j + k];
        }
    }

    return 1; // Inversion réussie
}

// Fonction principale de décodage
int decode(int G[MAX_K][MAX_N], int y[MAX_N], int k, int n, int t) {
    int cols[MAX_N];
    int M[MAX_K][MAX_K], M_inv[MAX_K][MAX_K];
    int x[MAX_K];
    int e[MAX_N];

    while (1) {
        choose_random_columns(cols, k, n);

        // Construire la sous-matrice M
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                M[i][j] = G[i][cols[j]];
            }
        }

        printf("Sous-matrice M choisie:\n");
        print_matrix(M, k, k);

        // Tenter d'inverser M
        if (invert_matrix(M, M_inv, k)) {
            printf("Inverse de la sous-matrice M:\n");
            print_matrix(M_inv, k, k);

            // Calculer x = M_inv * y'
            for (int i = 0; i < k; i++) {
                x[i] = 0;
                for (int j = 0; j < k; j++) {
                    x[i] ^= (M_inv[i][j] * y[cols[j]]);
                }
            }

            // Calculer e = y - xG
            for (int i = 0; i < n; i++) {
                e[i] = y[i];
                for (int j = 0; j < k; j++) {
                    e[i] ^= (x[j] * G[j][i]);
                }
            }

            // Vérifier le poids de e
            if (weight(e, n) == t) {
                // Décodage réussi
                printf("Decodage reussi !\n");
                printf("Message x original: ");
                print_vector(x, k);

                printf("Erreur e trouvee: ");
                print_vector(e, n);

                return 1;
            }
        }
    }
}

int main() {
    srand(time(NULL));

    int k = 2;
    int n = 5;
    int t = 1;

    // Génération automatique de la matrice G et du vecteur y
    int G[MAX_K][MAX_N];
    int y[MAX_N];
    
    // Génération de la matrice G aléatoire
    printf("Matrice generatrice G:\n");
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            G[i][j] = rand() % 2;
            printf("%d ", G[i][j]);
        }
        printf("\n");
    }

    // Génération du vecteur x aléatoire
    int x[MAX_K];
    for (int i = 0; i < k; i++) {
        x[i] = rand() % 2;
    }

    // Calcul de y = xG + e
    for (int i = 0; i < n; i++) {
        y[i] = 0;
        for (int j = 0; j < k; j++) {
            y[i] ^= (x[j] * G[j][i]);
        }
    }

    // Ajout d'erreurs de poids t
    for (int i = 0; i < t; i++) {
        int pos;
        do {
            pos = rand() % n;
        } while (y[pos] == 1); // S'assurer de ne pas ajouter une erreur déjà présente
        y[pos] ^= 1;
    }

    // Affichage du vecteur y
    printf("Vecteur y:\n");
    print_vector(y, n);

    // Calcul du temps d'exécution
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    decode(G, y, k, n, t);
    end = clock();
    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Temps d'execution: %f secondes\n", cpu_time_used);

    return 0;
}
