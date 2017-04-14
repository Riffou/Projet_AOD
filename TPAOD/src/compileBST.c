/*! \file compileBST.c
 *  \brief     This implements the applyPatch program.
 *  \author    Lucie Pansart
 *  \author    Jean-Louis Roch
 *  \version   1.0
 *  \date      30/9/2016
 *  \warning   Usage: compileBST n originalFile 
 *  \copyright GNU Public License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <time.h>


float cout_min(int i, int j, long n, float **c, int **racines) {
  // On cherche la valeur de k qui minimise : c(i, k-1) + c(k+1, j) avec i <= k <= j
  float sumTemp;
  float minTemp = 1000.0;
  int l;
  for (l = i; l <= j; l++) {
    sumTemp = 0.0;
    if ((l-1) >= i) {
      //printf("SumTemp %d %d : %f\n", i, l-1, c[i][l-1]);
      sumTemp = c[i][l-1];  
    }
    if ((l+1) <= j) {
     // printf("SumTemp %d %d : %f\n", l+1, j, c[l+1][j]);
     // printf("SumTemp %f\n", c[1][1]);
      sumTemp += c[l+1][j];
    }
    if (sumTemp < minTemp) {
      minTemp = sumTemp;
      racines[i][j] = l;
    }
  }
  return minTemp;
}

void construit_arbre(int i, int j, long n, int **racines, int **abr) {

  // on crée le noeud associé à l'arbre (i, j), e0, .., en 
  // On regarde si il a fils gauche : if ()

  // on vérifie que l'arbre a un fils gauche

 // printf("\nconstruit_arbre %d %d %d", i, j, racines[i][j]);
  if (i < racines[i][j]) {
    //printf("1");
    abr[racines[i][j]][0] = racines[i][racines[i][j] - 1];
   // printf("2");
    construit_arbre(i, racines[i][j] - 1, n, racines, abr);
    //printf("3");
  }
  // pas de fils gauche
  else {
    abr[racines[i][j]][0] = -1;
  }
  if (racines[i][j] < j) {
    abr[racines[i][j]][1] = racines[racines[i][j] + 1][j];
    construit_arbre(racines[i][j] + 1, j, n, racines, abr); 
  }
  // pas de fils droit
  else {
    abr[racines[i][j]][1] = -1;
  }
}
/**
 * Main function
 * \brief Main function
 * \param argc  A count of the number of command-line arguments
 * \param argv  An argument vector of the command-line arguments.
 * \warning Must be called with a positive long integer, n,  and a filename, freqFile, as commandline parameters and in the given order.
 * \returns { 0 if succeeds and prints C code implementing an optimal ABR on stdout; exit code otherwise}
 */
int main (int argc, char *argv[]) {
  clock_t start = clock();
  long n = 0 ; // Number of elements in the dictionary
  FILE *freqFile = NULL ; // File that contains n positive integers defining the relative frequence of dictinary elements 

  if(argc != 3){
    fprintf(stderr, "!!!!! Usage: ./compileBST n  originalFile !!!!!\n");
      exit(EXIT_FAILURE); /* indicate failure.*/
  }

  { // Conversion of parameter n in a long 
    int codeRetour = EXIT_SUCCESS;
    char *posError;
    long resuLong;
    n = atol(argv[1]);
  
   
    assert(argc >= 2);
    // Conversion of argv[1] en long
    resuLong = strtol(argv[1], &posError, 10);
    // Traitement des erreurs
    switch (errno)
    {
      case EXIT_SUCCESS :
         // Conversion du long en int
         if (resuLong > 0)
         {
            n = (long)resuLong;
            fprintf(stderr, "Number of elements in the dictionary: %ld\n", n);         
         }
         else
         {
            (void)fprintf(stderr, "%s cannot be converted into a positive integer matching the number of elements in the dicionary.\n", argv[1]) ; 
            codeRetour = EXIT_FAILURE;
         }
      break;
      
      case EINVAL :
         perror(__func__);
         (void)fprintf(stderr, "%s does not match a long integer value. \n", argv[1]);
         codeRetour = EXIT_FAILURE;
      break;
      
      case ERANGE :
         perror(__func__);
         (void)fprintf(stderr, "%s does not fit in a long int :\n" "out of bound [%ld;%ld]\n",
                       argv[1], LONG_MIN, LONG_MAX);
         codeRetour = EXIT_FAILURE;
      break;
      default :
         perror(__func__);
         codeRetour = EXIT_FAILURE;
    } // switch (errno)
    if  (codeRetour != EXIT_SUCCESS) return codeRetour ;
  }


  freqFile = fopen(argv[2] , "r" );
  if (freqFile==NULL) {fprintf (stderr, "!!!!! Error opening originalFile !!!!!\n"); exit(EXIT_FAILURE);}
    

  float *access = calloc(n, sizeof(float));
  float *proba  = calloc(n, sizeof(float));
  // avec proba_sum[k] = somme pour i = 0 à k de proba[i]
  float *proba_sum = calloc(n, sizeof(float));

  int i=0;

  float **c;
  c = malloc(n * sizeof(float*));
  for (i = 0; i < n; i++) {
    c[i] = (float*)calloc(n, sizeof(float));
  }
  
  int **racines;
  racines = malloc(n * sizeof(int*));
  for (i = 0; i < n; i++) {
    racines[i] = (int*)calloc(n, sizeof(int));
  }

  int **abr;
  abr = malloc(n * sizeof(int*));
  for (i = 0; i < n; i++) {
    abr[i] = (int*)calloc(2, sizeof(int));
  }

  int j=0;
  i = 0;
  float occurences_sum = 0;
   // On lit les occurences dans le fichier freqFile et on les stocke dans le tableau access
    fscanf(freqFile, "%d", &j);
    while(!feof(freqFile)) {
        access[i]=j;
     //   printf("Acces[%d] : %f\n\n", i, access[i]);
        occurences_sum += access[i];
        fscanf(freqFile, "%d", &j);
        i++;
    }
    // calcul des probabilités pour chaque élément de access
    for(i = 0; i < n; i++) {
      proba[i] = access[i]/occurences_sum;
      c[i][i] = proba[i];
     // printf("Proba[%d] : %f\n", i, proba[i]);
     // printf("c[%d][%d] : %f\n\n", i, i , c[i][i]);
    }
    
    // calcul des sommes des probabilités
    proba_sum[0] = proba[0];
   // printf("proba_sum[0] : %f\n", proba_sum[0]);
    for (i = 1; i < n; i++) {
      proba_sum[i] = proba_sum[i-1] + proba[i];
     // printf("proba_sum[%d] : %f\n", i, proba_sum[i]);
    }
    
    // calcul des coûts c(i,j) avec 0 <= i <= j <= n, c(i, j) contient les éléments du dictionnaire suivants : e(i), ..., e(j)

    // étape 1 : calcul des coûts c(i, i) : déjà fait dans le calcul des probabilités car c[i][i] = p[i]
    // étape 2 : calcul des coûts et des racines dans un ordre bien précis : 
    for (j = 1; j < n; j++) {
      for (i = j-1; i >= 0; i--) {
        if (i > 0) {
          c[i][j] = (proba_sum[j] - proba_sum[i-1]) + cout_min(i, j, n, c, racines);
        }
        else {
          c[i][j] = proba_sum[j] + cout_min(i, j, n, c, racines);
        }
       // printf("c[%d][%d] = %f\n\n", i, j, c[i][j]);
       // printf("racines : racines[%d][%d] = %d\n\n", i, j, racines[i][j]);
      }
    }
    // calcul des racines qui ne sont pas calculées précédemment
    for (i = 0; i < n; i++) {
      racines[i][i] = i;
    }
/*
*/
  //  printf("\n\n-- ABR --\n\n");

    // Calcul de l'ABR optimal en fonction du tableau de racines    
    construit_arbre(0, n-1, n, racines, abr);
    printf("static int BSTroot = %d;\n", racines[0][n-1]);
    printf("static int BSTtree[%ld][2] = {\n", n);
    for (i = 0; i < n-1; i++) {
      printf("{%d, %d},\n", abr[i][0], abr[i][1]);
    }
    printf("{%d, %d};\n", abr[n-1][0], abr[n-1][1]);

  fclose(freqFile);
  clock_t end = clock();
  double seconds = (double) (end - start) / CLOCKS_PER_SEC;
  printf("%.15f",seconds);

  return 0;
}