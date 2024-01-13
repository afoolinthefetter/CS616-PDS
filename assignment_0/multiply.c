#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>

#define dim 2048
#define num_threads 8


int **a, **b, **c, **d, **c_plus_d, **c_plus_d_transposed, **test_a;
pthread_mutex_t lock;


void transpose() {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            c_plus_d_transposed[j][i] = c_plus_d[i][j];
        }
    }
}


void *multiplier(void *arg) {
    int thread_id = *(int *)arg;
    int interval = dim / num_threads;
    int start_row = thread_id * interval;
    int end_row = (thread_id + 1) * interval;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < dim; j++) {
            int sum = 0;
            for (int k = 0; k < dim; k++) {
                sum += b[i][k] * c_plus_d_transposed[j][k];
            }
            pthread_mutex_lock(&lock);
            a[i][j] += sum;
            pthread_mutex_unlock(&lock);
        }
    }

    return NULL;
}

void generate_random_matrix(int **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = rand() % 100;
        }
    }
}

int** adder() {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            c_plus_d[i][j] = c[i][j] + d[i][j];
        }
    }
    return c_plus_d;
}


void check_multiplication_result(){
    for(int i=0; i < dim; i++){
        for(int j=0;j<dim;j++){
            int sum = 0;
            for(int k =0; k<dim; k++){
                sum += b[i][k] * c_plus_d[k][j];
            }
            test_a[i][j] += sum;
        }
    }
}



int main(void)
{
    srand(time(NULL));

    struct timeval multiply_start, multiply_stop;
    struct timeval add_start, add_stop;
    pthread_t *threads;

    pthread_mutex_init(&lock, NULL);


    threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

    b = (int **)malloc(sizeof(int *) * dim);
    c = (int **)malloc(sizeof(int *) * dim);
    d = (int **)malloc(sizeof(int *) * dim);
    a = (int **)malloc(sizeof(int *) * dim);
    c_plus_d = (int **)malloc(sizeof(int *) * dim);
    c_plus_d_transposed = (int **)malloc(sizeof(int *) * dim);
    test_a = (int **)malloc(sizeof(int *) * dim);

    for (int i = 0; i < dim; i++)
    {
        b[i] = (int *)malloc(sizeof(int) * dim);
        c[i] = (int *)malloc(sizeof(int) * dim);
        d[i] = (int *)malloc(sizeof(int) * dim);
        c_plus_d[i] = (int *)malloc(sizeof(int) * dim);
        c_plus_d_transposed[i] = (int *)malloc(sizeof(int) * dim);
        a[i] = (int *)calloc(dim, sizeof(int));  // calloc used to initialize matrix a with 0s
        test_a[i] = (int *)calloc(dim, sizeof(int)); // calloc used to initialize matrix a with 0s
    }

    generate_random_matrix(b, dim);
    generate_random_matrix(c, dim);
    generate_random_matrix(d, dim);

    gettimeofday(&add_start, NULL);
    adder();
    gettimeofday(&add_stop, NULL);
    

    transpose();

    gettimeofday(&multiply_start, NULL);
    for (int i = 0; i < num_threads; ++i)
    {
        int *thread_id;
        thread_id = (int *)malloc(sizeof(int));
        *thread_id = i;
        pthread_create(&threads[i], NULL, multiplier, (void *)thread_id);
    }

    for (int i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&multiply_stop, NULL);

    printf("Multiplication time: %3.5f seconds\n", ((multiply_stop.tv_sec - multiply_start.tv_sec) * 1000000 + multiply_stop.tv_usec - multiply_start.tv_usec) / 1000000.0);
    printf("\nMultiplication done\n");
    return 0;
}
