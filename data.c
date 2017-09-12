/* TAKE NOTE THAT I INCLUDED THE INITIAL GRID FOR ITERATIONS
   THIS MEANS THE GRIDS WILL SAY "5 OUT OF 6" FOR THE 4TH ITERATION OUT OF 5
   THIS IS INTENTIONAL
   Aaron Gordon 0884023
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int numThreads, gridSize, iterations;
int **readGrid;
int **writeGrid;

void displayGrid(int **grid, int iter);
void copyGrid();
void *iterateGrid(void *rankParam);

int main(int argc, char *argv[]) {
  int showGrid = 0;
  int i, j;
  long thread;
  pthread_t *threadHandles;
  time_t t;

  srand((unsigned) time(&t));

  if(argc == 5 && strcmp(argv[4], "-d") == 0) {
    showGrid = 1;
  } else if(argc == 5 && strcmp(argv[4], "-d") != 0) {
    printf("-d must be the 4th parameter if a 4th is given.");
    exit(0);
  } else if(argc != 4) {
    printf("Format for arguments is data <#-threads> <grid-size> <#-iterations> -d (-d optional).");
    exit(0);
  }

  numThreads = strtol(argv[1], NULL, 10);
  gridSize = strtol(argv[2], NULL, 10);
  iterations = strtol(argv[3], NULL, 10);

  if(gridSize > numThreads) {
    numThreads = gridSize;
  }

  readGrid = malloc(gridSize * sizeof(*readGrid));
  writeGrid = malloc(gridSize * sizeof(*writeGrid));

  for(i = 0; i < gridSize; i++) {
    readGrid[i] = malloc(gridSize * sizeof(*readGrid[i]));
    writeGrid[i] = malloc(gridSize * sizeof(*writeGrid[i]));

    for(j = 0; j < gridSize; j++) {
      if(rand() % 100 <= 40) {
        readGrid[i][j] = 1;
      } else {
        readGrid[i][j] = 0;
      }
    }
  }

  if(showGrid) {
    displayGrid(readGrid, 1);
  }

  threadHandles = malloc(numThreads * sizeof(pthread_t));
  for(j = 0; j < iterations; j++) {
    for(thread = 0; thread < numThreads; thread++) {
      pthread_create(&threadHandles[thread], NULL, iterateGrid, (void*)thread);
    }

    for(thread = 0; thread < numThreads; thread++) {
      pthread_join(threadHandles[thread], NULL);
    }

    copyGrid();
    if(showGrid) {
      displayGrid(readGrid, j + 2);
    }
  }

  for(i = 0; i < gridSize; i++) {
    free(readGrid[i]);
    free(writeGrid[i]);
  }

  return 0;
}

void copyGrid() {
  int i, j;

  for(i = 0; i < gridSize; i++) {
    for(j = 0; j < gridSize; j++) {
      readGrid[i][j] = writeGrid[i][j];
    }
  }
}

void *iterateGrid(void *rankParam) {
  long rank = (long)rankParam;
  int toIterate = gridSize / numThreads;
  int firstRow = rank * toIterate;
  int lastRow = (rank + 1) * toIterate - 1;
  int i, j, neighbours;

  for(i = firstRow; i <= lastRow; i++) {
    for(j = 0; j < gridSize; j++) {
      neighbours = 0;
      if(i > 0 && readGrid[i - 1][j]) {
        neighbours++;
      }
      if(i < gridSize - 1 && readGrid[i + 1][j]) {
        neighbours++;
      }
      if(j > 0 && readGrid[i][j - 1]) {
        neighbours++;
      }
      if(j < gridSize - 1 && readGrid[i][j + 1]) {
        neighbours++;
      }
      if(i > 0 && j > 0 && readGrid[i - 1][j - 1]) {
        neighbours++;
      }
      if(i < gridSize - 1 && j < gridSize - 1 && readGrid[i + 1][j + 1]) {
        neighbours++;
      }
      if(j > 0 && i < gridSize - 1 && readGrid[i + 1][j - 1]) {
        neighbours++;
      }
      if(j < gridSize - 1 && i > 0 && readGrid[i - 1][j + 1]) {
        neighbours++;
      }

      if((readGrid[i][j] == 0 && neighbours == 3) || (readGrid[i][j] == 1 && (neighbours == 3 || neighbours == 2))) {
        writeGrid[i][j] = 1;
      } else {
        writeGrid[i][j] = 0;
      }
    }
  }

  return NULL;
}

void displayGrid(int **grid, int iter) {
  int i, j;

  printf("\nGrid %d of %d\n", iter, iterations + 1);

  for(i = 0; i < gridSize; i++) {
    printf(" _");
  }

  printf("\n");
  for(i = 0; i < gridSize; i++) {
    for(j = 0; j < gridSize; j++) {
      printf("|");
      if(grid[i][j] == 1) {
        printf("X");
      } else {
        printf(" ");
      }
    }
    printf("|\n");

    for(j = 0; j < gridSize; j++) {
      printf(" _");
    }
    printf("\n");
  }
}
