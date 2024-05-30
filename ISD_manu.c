#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000
#define K 500
#define T 10


clock_t start, end;
double cpu_time_used;

void afficherMatrice(int rows, int cols, int matrice[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrice[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void afficherVecteur(int vecteur[N-K]) {
    for (int i = 0; i < N-K; i++) {
        printf("%d ", vecteur[i]);
    }
    printf("\n");
}

void echangerLignes(int cols, int matrice[][cols], int ligne1, int ligne2) {
    for (int i = 0; i < cols; i++) {
        int temp = matrice[ligne1][i];
        matrice[ligne1][i] = matrice[ligne2][i];
        matrice[ligne2][i] = temp;
    }
}

void permutationAleatoire(int permutation[N]) {
    for (int i = 0; i < N; i++) {
        permutation[i] = i;
    }
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = temp;
    }
}

void appliquerPermutation(int matrice[N-K][N], int permutation[N], int matriceP[N-K][N]) {
    for (int i = 0; i < N-K; i++) {
        for (int j = 0; j < N; j++) {
            matriceP[i][j] = matrice[i][permutation[j]];
        }
    }
}

void generateParityCheckMatrix(int H[N-K][N]) {
    for (int i = 0; i < N-K; i++) {
        for (int j = 0; j < N; j++) {
            H[i][j] = rand() % 2;
        }
    }
}

void generateErrorVector(int e[N], int t) {
    for (int i = 0; i < N; i++) {
        e[i] = 0;
    }
    for (int i = 0; i < t; i++) {
        int pos;
        do {
            pos = rand() % N;
        } while (e[pos] == 1);
        e[pos] = 1;
    }
}

void computeSyndrome(int H[N-K][N], int e[N], int s[N-K]) {
    for (int i = 0; i < N-K; i++) {
        s[i] = 0;
        for (int j = 0; j < N; j++) {
            s[i] ^= e[j] & H[i][j];
        }
    }
}

void gaussJordanElimination(int H[N-K][N], int U[N-K][N-K]) {
    for (int i = 0; i < N-K; i++) {
        for (int j = 0; j < N-K; j++) {
            U[i][j] = (i == j) ? 1 : 0;
        }
    }

    for (int i = 0; i < N-K; i++) {
        if (H[i][i] == 0) {
            for (int j = i + 1; j < N-K; j++) {
                if (H[j][i] == 1) {
                    echangerLignes(N, H, i, j);
                    echangerLignes(N-K, U, i, j);
                    break;
                }
            }
        }
        for (int j = 0; j < N-K; j++) {
            if (j != i && H[j][i] == 1) {
                for (int k = 0; k < N; k++) {
                    H[j][k] ^= H[i][k];
                }
                for (int k = 0; k < N-K; k++) {
                    U[j][k] ^= U[i][k];
                }
            }
        }
    }
}

int checkWeight(int s[N-K]) {
    int weight = 0;
    for (int i = 0; i < N-K; i++) {
        if (s[i] == 1) {
            weight++;
        }
    }
    return weight;
}

void findInversePermutation(int permutation[N], int inverseP[N]) {
    for (int i = 0; i < N; i++) {
        inverseP[permutation[i]] = i;
    }
}

void applyInversePermutation(int eP[N], int inverseP[N], int e[N]) {
    for (int i = 0; i < N; i++) {
        e[i] = eP[inverseP[i]];
    }
}

int main() {
    srand(time(NULL));
    

    // Commencer à mesurer le temps
    start = clock();

    int H[N-K][N];
    generateParityCheckMatrix(H);

    int e[N];
    generateErrorVector(e, T);

    int s[N-K];
    computeSyndrome(H, e, s);

    printf("Parity-check matrix H:\n");
    afficherMatrice(N-K, N, H);

    printf("Error vector e:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", e[i]);
    }
    printf("\n\n");

    printf("Syndrome s:\n");
    afficherVecteur(s);

    int permutation[N];
    int inverseP[N];
    int HP[N-K][N];
    int U[N-K][N-K];
    int sU[N-K];
    int eP[N];

    while (1) {
        permutationAleatoire(permutation);
        appliquerPermutation(H, permutation, HP);
        gaussJordanElimination(HP, U);

        for (int i = 0; i < N-K; i++) {
            sU[i] = 0;
            for (int j = 0; j < N-K; j++) {
                sU[i] ^= s[j] & U[i][j];
            }
        }

        if (checkWeight(sU) <= T) {
            for (int i = 0; i < N; i++) {
                eP[i] = 0;
            }
            for (int i = 0; i < N-K; i++) {
                eP[i] = sU[i];
            }
            for (int i = N-K; i < N; i++) {
                eP[i] = 0;
            }
            findInversePermutation(permutation, inverseP);
            applyInversePermutation(eP, inverseP, e);
            break;
        }
    }

    printf("Recovered error vector e:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", e[i]);
    }
    printf("\n");

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Temps d'execution : %f secondes\n", cpu_time_used);

    return 0;
}