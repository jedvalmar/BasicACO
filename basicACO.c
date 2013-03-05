#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>


#define VERBOSE

typedef struct {
    float *matrix;
    int width;
    int height;
} graph_t;

graph_t  get_distance_matrix();
void initialize_feromone(float * incomplete_graph, float * originalGraph, int number_of_ants);
void calc_path(int * ant_city_travel, float * graph, int number_of_ants, float * feromone_matrix, int alpha, int beta);
float calculateDistanceBetween(int coord_a_x , int coord_a_y, int coord_b_x, int coord_b_y);
void initialize_ant_distribution(int * ant_matrix, int number_of_ants);
void choose_city(int number_of_ants,int * ant_city_travel, float * path, int * visitedCities, float * feromone_path, int alpha, int beta, int nextCity);
float rand_FloatRange(float a, float b);

int main(int argc, char const *argv[])
{
    graph_t instance = get_distance_matrix();
    float *distanceMatrix = instance.matrix;

    #ifdef VERBOSE
    printf("%d\n", instance.width);
    printf("%d\n", instance.height);
    for (int i = 0; i < instance.width; ++i)
    {
        for (int j = 0; j < instance.height; ++j)
        {
            printf(" %.2f ", *distanceMatrix);
            distanceMatrix++;
        }
        printf("\n");
    }
    #endif

    int alpha, beta;
    printf("alpha: \n");
    scanf("%d",&alpha);
    printf("beta: \n");
    scanf("%d",&beta);

    int * ant_matrix = (int *)malloc( sizeof(int) * (instance.width * instance.height));
    int * matrix_pointer = ant_matrix;
     for (int i = 0; i < instance.width; ++i)
    {
        for (int j = 0; j < instance.height; ++j)
        {
            *matrix_pointer = 0;
            matrix_pointer++;
        }
    }
    initialize_ant_distribution(ant_matrix, instance.width);

    int counter = 0;

    for (int i = 0; i < instance.width; ++i)
    {
        for (int j = 0; j < instance.height; ++j, counter++)
        {
            printf("%d ",ant_matrix[counter]);
        }
        printf("\n");
    }

   


    int * ant_travel_current_path = ant_matrix;
    int number_of_ants = instance.width;
    float * the_distance_matrix = instance.matrix;
    float * feromone_matrix = (float *)malloc(sizeof(float) * (number_of_ants * number_of_ants));
    float * pointerToFeromoneMatrix = feromone_matrix;

    initialize_feromone(pointerToFeromoneMatrix, the_distance_matrix, number_of_ants);

     //testing probability function
    // TODO : implement probability function within a cycle

    calc_path(ant_travel_current_path, the_distance_matrix, number_of_ants, feromone_matrix, alpha, beta);

    // TODO : delete this when path its actually being iterated by its paths
    printf("path taken by first ant\n");
    for (int i = 0; i < number_of_ants; ++i)
    {
        printf("%d ", ant_travel_current_path[i]);
    }

    printf("\n");

    free(feromone_matrix);
    free(instance.matrix);
    return 0;
}



void initialize_ant_distribution(int * ant_matrix, int number_of_ants)
{
    int random_city;
    int cont = 0;
    int * arrayOfConcurrency = (int *) malloc(sizeof(int)* number_of_ants);

    for (int i = 0; i < number_of_ants; ++i)
    {
        arrayOfConcurrency[i] = 0;
    }
    int * begining = ant_matrix;

    while(cont < number_of_ants -1){
        while(1){
            struct timeval t;
            gettimeofday(&t,NULL);
            srand(t.tv_sec+t.tv_usec);
            random_city = rand() % number_of_ants;
            if (arrayOfConcurrency[random_city]==0)
            {
                printf("value = %d\n",random_city);
                arrayOfConcurrency[random_city]=1;
                break;
            }
        }

        *(begining + (cont * number_of_ants)) = random_city;
        cont++;
    }
    
}

 graph_t  get_distance_matrix()
 {    
    FILE * positions;
    int number_of_elements;

    positions = fopen("positions.txt","r");

    fscanf(positions,"%d",&number_of_elements);

    #ifdef VERBOSE
    printf("%d\n", number_of_elements);
    #endif

    graph_t aGraph;

    // iteration trough list of positions

    int list_of_positions[number_of_elements][3];

    for (int i = 0; i < number_of_elements; ++i) {
        for (int j = 0; j < 3; ++j) {
            fscanf(positions,"%d",&list_of_positions[i][j]);
        }
    }
           

    #ifdef VERBOSE
    for (int i = 0; i < number_of_elements; ++i)
    {
        for (int j = 0; j < 3; ++j) {
            printf("%d ",list_of_positions[i][j]);
        }
        printf("\n");
    }
    #endif


    aGraph.width = number_of_elements;
    aGraph.height = number_of_elements;

    aGraph.matrix = (float *) malloc(sizeof(float) * (number_of_elements * number_of_elements));

    float * f_pos = aGraph.matrix;

    for (int i = 0; i < aGraph.width; ++i) {
        for (int j = 0; j < aGraph.height; ++j) {
            *f_pos = 0;

            if(i != j) {
                int coord_a_x =  list_of_positions[i][1];
                int coord_a_y = list_of_positions[i][2];
                int coord_b_x = list_of_positions[j][1];
                int coord_b_y = list_of_positions[j][2];

                *f_pos = calculateDistanceBetween(coord_a_x, coord_a_y, coord_b_x, coord_b_y);
            }
            
            f_pos++;
        }
    }
    return aGraph;
}

float calculateDistanceBetween(int coord_a_x , int coord_a_y, int coord_b_x, int coord_b_y)
{
    float result;

    result = pow((coord_a_x - coord_b_x), 2);
    result = result + pow ((coord_a_y - coord_b_y) , 2);
    result = sqrt(result);

    return result;
}


void calc_path(int * ant_city_travel, float * graph, int number_of_ants, float * feromone_matrix, int alpha, int beta){

    int * visitedCities = (int *)malloc(sizeof(int) * number_of_ants);
    int currentCity;
    float * eval_path;
    float * feromone_path;
    for (int i = 0; i < number_of_ants; ++i)
    {
        visitedCities[i] = 0;
    }

    for (int i = 0; i < number_of_ants -1; ++i)
    {
        currentCity = ant_city_travel[i];
        visitedCities[currentCity] = 1;
        eval_path = &graph[currentCity * number_of_ants];
        feromone_path = &feromone_matrix[currentCity * number_of_ants];

        choose_city(number_of_ants, ant_city_travel, eval_path, visitedCities, feromone_path, alpha, beta , i+1);
    }



    free(visitedCities);

}


// here is the bussiness

void choose_city(int number_of_ants,int * ant_city_travel, float * path, int * visitedCities, float * feromone_path, int alpha, int beta, int nextCity){
    printf("\n");
    int citiesAvailable;
    float denominator = 0;
    float inverse = 0;
    float  * desicionVector = (float *) malloc(sizeof(float) * number_of_ants);
    int * cityIndex = (int *)malloc(sizeof(int) * number_of_ants);
    int cont = 0;
    for (int i = 0; i < number_of_ants; ++i)
    {
        if (visitedCities[i] == 0 && path[i]!=0)
        {
            inverse = 1/path[i];
            denominator += (pow(feromone_path[i] , alpha) * pow(inverse, beta)); 
            cont++;
        }
        desicionVector[i] = 0;
        cityIndex[i] = i;
    }

    
    float probability = 0;

    for (int i = 0; i < number_of_ants ; ++i)
    {
         if (visitedCities[i] == 0 && path[i]!=0)
        {
            inverse = 1/path[i];
            probability = (  (pow(feromone_path[i] , alpha)  *  pow(inverse, beta))  ) / denominator;
            desicionVector[i] = probability;

        }
    }


    //sort
    float temp;
    int citytemp;
    for (int i = 0; i < number_of_ants; ++i)
    {
        for (int j = i+1; j < number_of_ants; ++j)
        {
            if (desicionVector[j] > desicionVector[i])
            {
               temp = desicionVector[i];
               desicionVector[i] = desicionVector[j];
               desicionVector[j] = temp;
               citytemp = cityIndex[i];
               cityIndex[i] = cityIndex[j];
               cityIndex[j] = citytemp;

            }
        }
    }

    // printing desicion vector
    for (int i = 0; i < number_of_ants; ++i)
    {
        printf("%.3f ", desicionVector[i] );
    }
    printf("\n");


    //printing visitedCities
    for (int i = 0; i < number_of_ants; ++i)
    {
        printf("%d ", visitedCities[i]);
    }
    printf("\n");

    float random_city;
    random_city = rand_FloatRange(0, 1);
    printf("%f\n",random_city );

    float acumulator = 0;
    for (int i = 0; i < number_of_ants; ++i)
    {
        acumulator += desicionVector[i];
        if (acumulator >= random_city)
        {
            printf("%.3f if greater than %.3f, at city %d (%d)\n ", acumulator,random_city, cityIndex[i],i);
            ant_city_travel[nextCity] = cityIndex[i];
            break;
        }
        
    }



    free(desicionVector);

}

float rand_FloatRange(float a, float b)
{
    return ((b-a)*((float)rand()/RAND_MAX))+a;
}


void initialize_feromone(float * incomplete_graph, float * originalGraph, int number_of_ants){
    for (int i = 0; i < number_of_ants; ++i)
    {
        for (int j = 0; j < number_of_ants; ++j,originalGraph++, incomplete_graph++)
        {
            if (*originalGraph != 0)
            {
                *incomplete_graph = 0.001;
                
            }
            else
            {
                *incomplete_graph = 0;
            }
            printf("%.3f ", *incomplete_graph );
        }
        printf("\n");
    }
}