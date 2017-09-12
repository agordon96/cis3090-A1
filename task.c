/* TAKE NOTE THAT I INCLUDED THE INITIAL GRID FOR ITERATIONS
   THIS MEANS THE GRIDS WILL SAY "5 OUT OF 6" FOR THE 4TH ITERATION OUT OF 5
   THIS IS INTENTIONAL
   Aaron Gordon 0884023
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int gridSize, iterations;
//First num for the queues is X, second is Y, third is locked (1 for true)
int **liveQueue;
int **deadQueue;
int **readGrid;
int **writeGrid;

void displayGrid(int **grid, int iter);
void copyGrid();
void *checkNeighbours(void *dummy);
void *checkLiveQueue(void *dummy);
void *checkDeadQueue(void *dummy);

int main(int argc, char *argv[]) {
  int showGrid = 0;
  int i, j;
  long thread;
  pthread_t *threadHandles;
  time_t t;

  srand((unsigned) time(&t));

  if(argc == 4 && strcmp(argv[3], "-d") == 0) {
    showGrid = 1;
  } else if(argc == 4 && strcmp(argv[3], "-d") != 0) {
    printf("-d must be the 3rd parameter if a 3rd is given.");
    exit(0);
  } else if(argc != 3) {
    printf("Format for arguments is task <grid-size> <#-iterations> -d (-d optional).");
    exit(0);
  }

  gridSize = strtol(argv[1], NULL, 10);
  iterations = strtol(argv[2], NULL, 10);

  readGrid = malloc(gridSize * sizeof(*readGrid));
  writeGrid = malloc(gridSize * sizeof(*writeGrid));

  liveQueue = malloc(gridSize * gridSize * sizeof(*liveQueue));
  deadQueue = malloc(gridSize * gridSize * sizeof(*deadQueue));

  for(i = 0; i < gridSize; i++) {
    readGrid[i] = malloc(gridSize * sizeof(*readGrid[i]));
    writeGrid[i] = malloc(gridSize * sizeof(*writeGrid[i]));

    for(j = 0; j < gridSize; j++) {
      liveQueue[(i * gridSize) + j] = malloc(3 * sizeof(*liveQueue));
      deadQueue[(i * gridSize) + j] = malloc(3 * sizeof(*deadQueue));
    }

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

  threadHandles = malloc(3 * sizeof(pthread_t));
  for(j = 0; j < iterations; j++) {
    for(i = 0; i < (gridSize * gridSize); i++) {
      liveQueue[i][0] = -1;
      liveQueue[i][1] = -1;
      liveQueue[i][2] = 1;

      deadQueue[i][0] = -1;
      deadQueue[i][1] = -1;
      deadQueue[i][2] = 1;
    }

    pthread_create(&threadHandles[0], NULL, checkNeighbours, (void*)thread);
    pthread_create(&threadHandles[1], NULL, checkLiveQueue, (void*)thread);
    pthread_create(&threadHandles[2], NULL, checkDeadQueue, (void*)thread);

    for(thread = 0; thread < 3; thread++) {
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
    for(j = 0; j < gridSize; j++) {
      free(liveQueue[(i * gridSize) + j]);
      free(deadQueue[(i * gridSize) + j]);
    }
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

void *checkLiveQueue(void *dummy) {
  int i;

  for(i = 0; i < (gridSize * gridSize); i++) {
    while(liveQueue[i][2] == 1);//Continue looping if element is locked
    if(liveQueue[i][0] >= 0) {
      writeGrid[liveQueue[i][1]][liveQueue[i][0]] = 1;
    }
  }
}

void *checkDeadQueue(void *dummy) {
  int i;

  for(i = 0; i < (gridSize * gridSize); i++) {
    while(deadQueue[i][2] == 1);//Continue looping if element is locked
    if(deadQueue[i][0] >= 0) {
      writeGrid[deadQueue[i][1]][deadQueue[i][0]] = 0;
    }
  }
}

void *checkNeighbours(void *dummy) {
  int i, j, neighbours, posInQueue;

  for(i = 0; i < gridSize; i++) {
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

      posInQueue = (i * gridSize) + j;
      if((readGrid[i][j] == 0 && neighbours == 3) || (readGrid[i][j] == 1 && (neighbours == 3 || neighbours == 2))) {
        liveQueue[posInQueue][0] = j;
        liveQueue[posInQueue][1] = i;
        liveQueue[posInQueue][2] = 0;
        deadQueue[posInQueue][2] = 0;
      } else {
        deadQueue[posInQueue][0] = j;
        deadQueue[posInQueue][1] = i;
        deadQueue[posInQueue][2] = 0;
        liveQueue[posInQueue][2] = 0;
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
