#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>
#include <stdbool.h>

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

float calcTemp(int width, int height, float transferRate,
        int cell, float* grid, float baseTemp, int area){
    float result = 0;
    int numBorderCells = 8;

    bool rightBorder =
        cell%width == width-1;
    // printf("Right Border: %d ", rightBorder);
    bool leftBorder =
        cell%width == 0;
    // printf("Left Border: %d ", leftBorder);
    bool upperBorder =
        cell-width <= 0;
    // printf("Upper Border: %d ", upperBorder);
    bool lowerBorder =     
        cell+width >= area;
    // printf("Lower Border: %d\n", lowerBorder);
    
    if (!rightBorder){
        result += grid[cell+1];
        numBorderCells--;
    }
    if (!rightBorder && !upperBorder){
        result += grid[cell-width+1];
        numBorderCells--;
    }
    if (!rightBorder && !lowerBorder){
        result += grid[cell+width+1];
        numBorderCells--;
    }

    if (!upperBorder){
        result += grid[cell-width];
        numBorderCells--;
    }
    if (!lowerBorder){
        result += grid[cell+width];
        numBorderCells--;
    }

    if (!leftBorder){
        result += grid[cell-1];
        numBorderCells--;
    }
    if (!leftBorder && !upperBorder){
        result += grid[cell-width-1];
        numBorderCells--;
    }
    if (!leftBorder && !lowerBorder){
        result += grid[cell+width-1];
        numBorderCells--;
    }
    
    result += numBorderCells * baseTemp;
    result *= transferRate;
    result /= 8;
    result += grid[cell];
    return result/2;
} 

void swap(float** array1, float** array2){
    float* temp = *array1;
    *array1 = *array2;
    *array2 = temp;
}

int main(int argc, char* argv[]){
    double startTime = omp_get_wtime();
    if (argc != 9){
        printf("There appears to be some extra/missing values!\n");
        printf("Please enter 8 values.\n");   
        printf("./heat_transfer <num_threads> <numRows> <numCols> <base temp> <transfer_rate (1-1.1)> <timesteps> <heatFileName> <outputFileName>\n");
        exit(1);
    }

    int numThreads = atoi(argv[1]);
    omp_set_num_threads(numThreads);    

    // num Rows
    int height = atoi(argv[2]);
    // num Cols
    int width = atoi(argv[3]);
    int area = height * width;
    float* roomGridIn = malloc(area*sizeof(float));
    float* roomGridOut = malloc(area*sizeof(float));

    float baseTemp = atof(argv[4]);
    float transferRate = atof(argv[5]);
    int timesteps = atoi(argv[6]);
    int tenPercent = timesteps/10;


    char* heatFilename = argv[7];
    char* outputFilename = argv[8];
    FILE* input = fopen(heatFilename, "r");
    
    // Grabbing the first line of the file and always
    // assuming that it is the number of heaters
    int numHeaters = 0;
    fscanf(input, "%d", &numHeaters);
    // Setting up struct to hold heater data
    heaters* heatersMap = calloc(numHeaters, sizeof(heaters));
    for (int i = 0; i < numHeaters; i++){
        fscanf(input, "%d", &heatersMap[i].heaterRow);
        fscanf(input, "%d", &heatersMap[i].heaterColumn);
        fscanf(input, "%f", &heatersMap[i].heaterTemp);
    }

    fclose(input);
    for (int i = 0; i < area; i++)
        roomGridIn[i] = baseTemp;
    int percent = 0;
    for (int step = 0; step < timesteps; step++){
        if (step%tenPercent == 0){
            printf("%d0%% done\n", percent);
            percent++;
        }
        #pragma omp parallel for schedule(auto)
        for(int cell = 0; cell<area; cell++){
            setHeaters(width, numHeaters, heatersMap, roomGridIn);
            roomGridIn[cell] = calcTemp(width, height, transferRate,
                    cell, roomGridIn, baseTemp, area);
        }
        swap(&roomGridIn, &roomGridOut);
    }

    FILE* output = fopen(outputFilename, "w");
    
    double endTime = omp_get_wtime();
    fprintf(output, "Time to Completion: %.5f\n", endTime-startTime);
    for (int i = 1; i < area+1; i++){
        fprintf(output, "%.1f", roomGridIn[i-1]);
        if (i%width == 0 && i !=0){
            fprintf(output, "\n");
            continue;
        }
        fprintf(output, ",");
    }
    
    free(roomGridIn);
    free(roomGridOut);
}
