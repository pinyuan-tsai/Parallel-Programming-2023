#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

long long int helper(long long int local_toss, int seed) {
    long long int  number_in_circle = 0;
    unsigned int seeds = seed;
    for (long long int toss = 0; toss < local_toss; toss++)
    {
        double x = ((double)rand_r(&seeds) - (double)(RAND_MAX) / 2) / ((double)(RAND_MAX) / 2);
        double y = ((double)rand_r(&seeds) - (double)(RAND_MAX) / 2) / ((double)(RAND_MAX) / 2);
        double distance_squared = x * x + y * y;
        if ( distance_squared < 1 || distance_squared == 1)
            number_in_circle++;
    }
    return number_in_circle;
}

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---
    //printf("start \n");
    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Status status;
    int dest = 0, tag = 0;
    long long int work_size = tosses / world_size;
    long long int local_count, total_count = 0;
    local_count = helper(work_size, world_rank);
    // TODO: binary tree redunction
    for (int i = 2; i < world_size + 1; i *= 2){
        if(world_rank % i == i / 2){
            dest = world_rank - (i / 2);
            MPI_Send(&local_count,1 , MPI_LONG, dest, tag, MPI_COMM_WORLD);
        }

        else if(world_rank % i == 0){
            dest = world_rank + (i / 2);
            total_count = local_count;
            MPI_Recv(&local_count, 1, MPI_LONG, dest, tag, MPI_COMM_WORLD, &status);
            total_count += local_count;
            local_count = total_count;
        }
    }
    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = 4 * total_count /(( double ) tosses);
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
