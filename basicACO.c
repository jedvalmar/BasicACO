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

typedef struct {
    int id;
    int alpha;
    int beta;
} Ant;

graph_t  get_distance_matrix();
void initialize_feromone(float * incomplete_graph, float * originalGraph, int number_of_cities);
void calc_path(int * ant_city_travel, float * graph, int number_of_cities, float * feromone_matrix, Ant ant);
float calculateDistanceBetween(int coord_a_x , int coord_a_y, int coord_b_x, int coord_b_y);
void update_feromone(int * path_taken, float * feromone_matrix, int number_of_cities);
float calculate_total_distance(int * a_path, float * distanceMatrix, int number_of_cities);
void initialize_ant_distribution(int * ant_matrix, int number_of_ants, int number_of_cities);
void choose_city(int number_of_cities,int * ant_city_travel, float * path, int * visitedCities, float * feromone_path, Ant ant, int nextCity);
float rand_FloatRange(float a, float b);

int main(int argc, char const *argv[])
{
   
    int alpha, beta, iterations, numAnts;
    float * feromone_matrix;

    printf("alpha: \n");
    scanf("%d",&alpha);
    printf("beta: \n");
    scanf("%d",&beta);


    printf("number of iterations:\n");
    scanf("%d",&iterations);


    printf("Number of ants:\n");
    scanf("%d",&numAnts);

    Ant agents[numAnts];
    for (int i = 0; i < numAnts; ++i)
    {
        agents[i].id = i;
        agents[i].alpha = alpha;
        agents[i].beta = beta;
    }

    // reading the distance matrix
    graph_t instance = get_distance_matrix();

    // initializing crucial variables
    float *distanceMatrix = instance.matrix;
    int number_of_cities = instance.width;

    // feromone matrix
    feromone_matrix = (float *)malloc(sizeof(float) * (number_of_cities * number_of_cities));
    float * pointerToFeromoneMatrix = feromone_matrix;
    initialize_feromone(pointerToFeromoneMatrix, distanceMatrix , number_of_cities);

    // iteration cycle
    while(iterations > 0){

        // initializing new matrix for every iteration (it needs to get free at the end)
        int * ant_matrix = (int *)malloc( sizeof(int) * (numAnts * instance.width));
        int * matrix_pointer = ant_matrix;

        // asigning all values to zero
        for (int i = 0; i < numAnts; ++i)
        {
            for (int j = 0; j < instance.width; ++j)
            {
                *matrix_pointer = 0;
                matrix_pointer++;
            }
        }


        initialize_ant_distribution(ant_matrix, numAnts, number_of_cities);

        int counter = 0;

        for (int i = 0; i < numAnts; ++i)
        {
            for (int j = 0; j < instance.width; ++j, counter++)
            {
                printf("%d ",ant_matrix[counter]);
            }
        }

        
        int * ant_travel_current_path;
        float * total_distance_list = (float *)malloc(sizeof(float) * (numAnts));

        // implementing probability function within a cycle

        float total_traveled_distance = 0;
        for (int i = 0; i < numAnts; ++i)
        {
            ant_travel_current_path = &ant_matrix[i*number_of_cities];
            calc_path(ant_travel_current_path, distanceMatrix , number_of_cities , feromone_matrix, agents[i]);
            // measure total distance of brand new ant path (change if you want to measure other type of cost)
            total_traveled_distance = calculate_total_distance(ant_travel_current_path, distanceMatrix, number_of_cities);

            total_distance_list[i] = total_traveled_distance;
            // update feromone matrix
            update_feromone(ant_travel_current_path, feromone_matrix, number_of_cities);

        }


        printf("Ant colony path matrix\n");
        int * ant_matrix_iterator = ant_matrix;
        for (int i = 0; i < numAnts; ++i){
            for (int j = 0; j < number_of_cities; ++j,++ant_matrix_iterator)
                printf("%3d ", *ant_matrix_iterator);
            printf("\n");
        }
        //just to leave a more readble result
        printf("\n\n");

        printf("\n\nFeromone ultimate matrix\n");

        float * feromone_iterator = feromone_matrix;
        for (int i = 0; i < number_of_cities; ++i){
            for (int j = 0; j < number_of_cities; ++j,++feromone_iterator)
                printf("%.3f ", *feromone_iterator);
            printf("\n");
        }

         //just to leave a more readble result
        printf("\n\n");



        float smallest_traveling = total_distance_list[0];
        int ant_that_got_smallest_travel_path = 0;
        printf("List of total distance of every ant (ant number is equal to the city when it begins to iterate!)\n");
        for (int i = 0; i < numAnts; ++i)
        {
            if (smallest_traveling > total_distance_list[i])
            {
                smallest_traveling = total_distance_list[i];
                ant_that_got_smallest_travel_path = i;
            }
            printf("Ant %d : %.2f \n",i, total_distance_list[i]);

        }

        printf("\n");    

        printf("we can say that Ant %d got the smallest traveling path, with %.2f\n",
         ant_that_got_smallest_travel_path, smallest_traveling);
        free(total_distance_list);
        iterations--;
    }

    free(instance.matrix);
    free(feromone_matrix);
    return 0;
}



void initialize_ant_distribution(int * ant_matrix, int number_of_ants, int number_of_cities)
{
    int random_city;
    int cont = 0;
    int * begining = ant_matrix;

    while(cont < number_of_ants ){

        // Every city counts, even if they repeat themselves in the begining
        struct timeval t;
        gettimeofday(&t,NULL);
        srand(t.tv_sec+t.tv_usec * cont);
        random_city = rand() % number_of_cities;

        // this should assign each city properly at matrix start space
        *(begining + (cont * number_of_cities)) = random_city;

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


void calc_path(int * ant_city_travel, float * graph, int number_of_cities, float * feromone_matrix, Ant ant){

    int * visitedCities = (int *)malloc(sizeof(int) * number_of_cities);
    int currentCity;
    float * eval_path;
    float * feromone_path;
    for (int i = 0; i < number_of_cities; ++i)
    {
        visitedCities[i] = 0;
    }

    for (int i = 0; i < number_of_cities -1; ++i)
    {
        currentCity = ant_city_travel[i];
        visitedCities[currentCity] = 1;
        eval_path = &graph[currentCity * number_of_cities];
        feromone_path = &feromone_matrix[currentCity * number_of_cities];

        choose_city(number_of_cities, ant_city_travel, eval_path, visitedCities, feromone_path, ant, i+1);
    }



    free(visitedCities);

}


// here is the bussiness
// this could change to something else that conforms with the cost measurement of the actual problem
void choose_city(int number_of_cities,int * ant_city_travel, float * path, int * visitedCities, float * feromone_path, Ant ant, int nextCity){
    printf("\n");
    int citiesAvailable;
    float denominator = 0;
    float inverse = 0;
    float  * desicionVector = (float *) malloc(sizeof(float) * number_of_cities);
    int * cityIndex = (int *)malloc(sizeof(int) * number_of_cities);
    int cont = 0;
    for (int i = 0; i < number_of_cities; ++i)
    {
        if (visitedCities[i] == 0 && path[i]!=0)
        {
            inverse = 1/path[i];
            denominator += (pow(feromone_path[i] , ant.alpha) * pow(inverse, ant.beta)); 
            cont++;
        }
        desicionVector[i] = 0;
        cityIndex[i] = i;
    }

    
    float probability = 0;

    for (int i = 0; i < number_of_cities ; ++i)
    {
         if (visitedCities[i] == 0 && path[i]!=0)
        {
            inverse = 1/path[i];
            probability = (  (pow(feromone_path[i] , ant.alpha)  *  pow(inverse, ant.beta))  ) / denominator;
            desicionVector[i] = probability;

        }
    }


    //sort
    float temp;
    int citytemp;
    for (int i = 0; i < number_of_cities; ++i)
    {
        for (int j = i+1; j < number_of_cities; ++j)
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
    for (int i = 0; i < number_of_cities; ++i)
    {
        printf("%.3f ", desicionVector[i] );
    }
    printf("\n");


    //printing visitedCities
    for (int i = 0; i < number_of_cities; ++i)
    {
        printf("%d ", visitedCities[i]);
    }
    printf("\n");

    float random_city_factor;
    random_city_factor = rand_FloatRange(0, 1);
    printf("%f\n",random_city_factor );

    float acumulator = 0;
    for (int i = 0; i < number_of_cities; ++i)
    {
        acumulator += desicionVector[i];
        if (acumulator >= random_city_factor)
        {
            printf("%.3f is greater than %.3f, at city %d (%d)\n ", acumulator,random_city_factor, cityIndex[i],i);
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


float calculate_total_distance(int * a_path, float * distanceMatrix, int number_of_cities){

    float totalDistance, currentDistance;
    int pointA, pointB;

    // using totalDistance var as acumulator
    totalDistance = 0;

    // calculating distance of point A to point B

    for (int i = 0; i < (number_of_cities - 1); ++i)
    {
        pointA = a_path[i];
        pointB = a_path[i+1];
        currentDistance = distanceMatrix[pointA * number_of_cities + pointB];

        totalDistance += currentDistance;
    }

    return totalDistance;
}


void initialize_feromone(float * incomplete_graph, float * originalGraph, int number_of_cities){
    for (int i = 0; i < number_of_cities; ++i)
    {
        for (int j = 0; j < number_of_cities; ++j,originalGraph++, incomplete_graph++)
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



void update_feromone(int * path_taken, float * feromone_matrix, int number_of_cities)
{
    int pointA, pointB;
    float increment;
    float evaporation_factor;

    increment = 0.05;
    evaporation_factor = 0.05;
    for (int i = 0; i < (number_of_cities - 1) ; ++i){
        pointA = path_taken[i];
        pointB = path_taken[i+1];

        feromone_matrix[number_of_cities * pointA + pointB] = (feromone_matrix[number_of_cities * pointA + pointB] * (1 - evaporation_factor) ) + increment;

    }



}

