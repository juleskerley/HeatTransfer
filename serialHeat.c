#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>

// To create an array of later
typedef struct {
    int heaterRow;
    int heaterColumn; 
    float heaterTemp;
} heaters;

void setHeaters(int width, int numHeaters,
        heaters* heatersMap, float* roomGrid){
    for (int i = 0; i < numHeaters; i++){
        roomGrid[
            (heatersMap[i].heaterRow * width)
            + heatersMap[i].heaterColumn] 
            = heatersMap[i].heaterTemp;
    }
}

int main(int argc, char* argv[]){
    if (argc != 9){
        printf("There appears to be some extra/missing values!\n");
        printf("Please enter 8 values.\n");   
        printf("./heat_transfer <num_threads> <numRows> <numCols> <base temp> <transfer_rate (0.1-1)> <timesteps> <heatFileName> <outputFileName>\n");
        exit(1);
    }

    int num_threads = atoi(argv[1]);
    
    // num Rows
    int length = atoi(argv[2]);
    // num Cols
    int width = atoi(argv[3]);
    int area = length * width;
    float* roomGridIn = malloc(area*sizeof(float));
    float* roomGridOut = malloc(area*sizeof(float));

    float baseTemp = atof(argv[4]);
    float transferRate = atof(argv[5]);
    int timesteps = atoi(argv[6]);

    char* heatFilename = argv[7];
    char* outputFilename = argv[8];
    FILE* input = fopen(heatFilename, "r");
    FILE* output = fopen(outputFilename, "w");
    
    // Grabbing the first line of the file and always
    // assuming that it is the number of heaters
    int numHeaters = 0;
    fscanf(input, "%d", &numHeaters);
    // Setting up struct to hold heater data
    heaters* heatersMap = calloc(numHeaters, sizeof(heaters));
    for (int i = 0; i < numHeaters; i++){
        fscanf(input, "%d", &heatersMap[i].heaterRow);
        printf("%d ", heatersMap[i].heaterRow);
        fscanf(input, "%d", &heatersMap[i].heaterColumn);
        printf("%d ", heatersMap[i].heaterColumn);
        fscanf(input, "%f", &heatersMap[i].heaterTemp);
        printf("%f\n", heatersMap[i].heaterTemp);
    }

    fclose(input);

    for (int i = 0; i < area; i++)
        roomGridIn[i] = baseTemp;
    
    setHeaters(width, numHeaters,
            heatersMap, roomGridIn);

}
