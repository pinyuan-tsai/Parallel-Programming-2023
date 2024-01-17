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

bool fnz(long long int *recv_buffer, int size){
    for (int i = 0; i < size; i++){
        if(recv_buffer[i] == 0)
            return 0;
    }
    return 1;
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

    MPI_Win win;

    // TODO: MPI init
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //MPI_Status status;
    long long int work_size = tosses / world_size;
    long long int local_count, total_count = 0;
    local_count = helper(work_size, world_rank);
    //long long int recv_buffer[world_size];

    if (world_rank == 0)
    {
        // Master
        long long int *recv_buffer;
        MPI_Alloc_mem( (world_size - 1) * sizeof(long long int ), MPI_INFO_NULL, &recv_buffer);
        for (int i = 1; i < world_size; i++){
            recv_buffer[i - 1] = 0;
        }
        MPI_Win_create(recv_buffer, (world_size - 1) * sizeof(long long int), sizeof(long long int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

       bool ready = 0;
       while (!ready)
       {
          // Without the lock/unlock schedule stays forever filled with 0s
          MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
          ready = fnz(recv_buffer, world_size - 1);
          MPI_Win_unlock(0, win);
       }
       printf("All workers checked in using RMA\n");

       total_count = local_count;
       for (int i = 1; i < world_size; i++){
           total_count += recv_buffer[i - 1];
       }
       // Release the window
       MPI_Win_free(&win);
       // Free the allocated memory
       MPI_Free_mem(recv_buffer);
    }
    else
    {
        // Workers
        //int one = 1;
        // Worker processes do not expose memory in the window
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);

        // Register with the master
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Put(&local_count, 1, MPI_LONG_LONG_INT, 0, world_rank - 1, 1, MPI_LONG_LONG_INT, win);
        MPI_Win_unlock(0, win);

        //printf("Worker %d finished RMA\n", world_rank);

        // Release the window
        MPI_Win_free(&win);

        //printf("Worker %d done\n", world_rank);
    }

    //MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result
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