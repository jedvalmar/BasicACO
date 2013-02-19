#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define VERBOSE

typedef struct {
    float *matrix;
    int width;
    int height;
} graph_t;

graph_t  get_distance_matrix();
void initialize_feromone(float * incomplete_graph);
float calc_prob(int ant, float * array_of_prob, int elements, int * eval_array, int * graph);
float calculateDistanceBetween(int coord_a_x , int coord_a_y, int coord_b_x, int coord_b_y);


int main(int argc, char const *argv[])
{
    graph_t instance = get_distance_matrix();

    #ifdef VERBOSE
    printf("%d\n", instance.width);
    printf("%d\n", instance.height);
    #endif

    return 0;
}

 graph_t  get_distance_matrix()
 {    
    FILE * positions;
    char * name;
    int number_of_elements;

    positions = fopen("positionsArchive.txt","r");
    
    fscanf(positions, "%s",name);

    fscanf(positions,"%d",&number_of_elements);

    #ifdef VERBOSE
    puts(name);
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

    result = pow(coord_a_x - coord_b_x, 2);
    result = result + pow (coord_a_y - coord_b_y , 2);
    result = sqrt(result);

    return result;
}