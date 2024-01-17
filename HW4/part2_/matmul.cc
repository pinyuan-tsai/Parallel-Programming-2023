// #include <stdio.h>
// #include <stdlib.h>
// #include <mpi.h>

// // Read size of matrix_a and matrix_b (n, m, l) and whole data of matrixes from stdin
// //
// // n_ptr:     pointer to n
// // m_ptr:     pointer to m
// // l_ptr:     pointer to l
// // a_mat_ptr: pointer to matrix a (a should be a continuous memory space for placing n * m elements of int)
// // b_mat_ptr: pointer to matrix b (b should be a continuous memory space for placing m * l elements of int)
// inline void read(int &x)
// {
// 	x = 0;
//     char c = getchar();
// 	while(c < '0' || c > '9')
//         c = getchar();
// 	while(c >= '0' && c <= '9') {
// 		x = x * 10 + c - '0';
// 		c = getchar();
// 	} 
// }

// void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr,
//                         int **a_mat_ptr, int **b_mat_ptr)
// {
//     int world_rank, world_size;
//     MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
//     int *ptr;
//     if(world_rank == 0){
//         //printf("input : n m l \n");
//         scanf("%d %d %d", n_ptr, m_ptr, l_ptr);

//         int n = *n_ptr, m = *m_ptr, l = *l_ptr;
//         //printf("n = %d m = %d l = %d \n", n, m, l);
//         *a_mat_ptr = (int*)calloc(n * m, sizeof(int));
//         *b_mat_ptr = (int*)calloc(m * l, sizeof(int));
        
//         // construct a matrix (n x m)
//         // for (int i = 0; i < n; i++){
//         //     for (int j = 0; j < m; j++){
//         //         ptr = *a_mat_ptr + m * i + j;
//         //         //printf("%p \n ", ptr);
//         //         scanf("%d", ptr);
//         //     }
//         // }
//         //construct b transfer matrix
//         // for (int i = 0; i < m; i++){
//         //     for (int j = 0; j < l; j++){
//         //         ptr = *b_mat_ptr + m * j + i;
//         //         //printf("%p \n ", ptr);
//         //         scanf("%d", ptr);
//         //     }
//         // }
        
// 		for(int i = 0 ; i < n*m ; i++){
// 			//scanf("%d", *a_mat_ptr+i);
// 			read(*(*a_mat_ptr + i));
// 		}
// 		for(int i = 0 ; i < m ; i++){
// 			for(int j = 0 ; j < l ; j++){
// 				//scanf("%d", *b_mat_ptr+i+j*(*m_ptr));
// 				read(*(*b_mat_ptr + i + j * m));
// 			}
// 		}

//     }

//     MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     MPI_Bcast(m_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     MPI_Bcast(l_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     if(world_rank != 0){
//         *a_mat_ptr = (int*)malloc(sizeof(int) * (*n_ptr) * (*m_ptr));
//         *b_mat_ptr = (int*)malloc(sizeof(int) * (*m_ptr) * (*l_ptr));
//     }
//     MPI_Bcast(*a_mat_ptr, (*n_ptr) * (*m_ptr), MPI_INT, 0, MPI_COMM_WORLD);
//     MPI_Bcast(*b_mat_ptr, (*m_ptr) * (*l_ptr), MPI_INT, 0, MPI_COMM_WORLD);
//     //printf("%d \n", world_rank);
//     // if(world_rank == 2){
//     //     for (int i = 0; i < *n_ptr; i++){
//     //         for (int j = 0; j < *m_ptr; j++){
//     //             ptr = *a_mat_ptr + *m_ptr * i + j;
//     //             printf("%d  ", *ptr);
//     //             //scanf("%d", ptr);
//     //         }
//     //         printf("\n");
//     //     }
//     // }
    
// }

// // Just matrix multiplication (your should output the result in this function)
// // n:     row number of matrix a
// // m:     col number of matrix a / row number of matrix b
// // l:     col number of matrix b
// // a_mat: a continuous memory placing n * m elements of int
// // b_mat: a continuous memory placing m * l elements of int
// void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){
// 	int world_rank, world_size;
// 	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
// 	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
// 	int *C, *ans;
// 	int mat_size, start, end, jobs;
//     int a_idx, b_idx, c_idx;
//     int sum;
//     mat_size = n * l;
//     C =   (int*)calloc(mat_size, sizeof(int));
// 	ans = (int*)calloc(mat_size, sizeof(int));
//     jobs = mat_size / world_size;
//     start = world_rank * jobs;
//     if (world_rank == world_size - 1)
//     {
//         jobs += mat_size % world_size;
//     }
//     //printf("%d %d %d \n", world_rank, jobs, start);
//     a_idx = (start / l) * m;
//     b_idx = m * (start % l);
//     while(jobs > 0 ){
//         sum = 0;
//         // n m l
//         // 4 6 5
//         //a_idx = (start / l) * m;
//         //b_idx = m * (start % l);
//         c_idx = start;
//         // 1 1 1 1 1 1   1 1 1 1 1 1    . . . . .
//         // 2 2 2 2 2 2   2 2 2 2 2 2    . . . . .
//         // 3 3 3 3 3 3   3 3 3 3 3 3    . . . . .
//         // 4 4 4 4 4 4   4 4 4 4 4 4    . . . . .
//         //               5 5 5 5 5 5
//         //sum = 0;
//         //int k = start % l;
//         for(int k = c_idx % l ; k < l ; k++){
//             sum = 0;
//             for (int j = 0; j < m; j++)
//             {
//                 sum += a_mat[a_idx + j] * b_mat[b_idx];
//                 b_idx++;
//             }
//             C[start] = sum;
//             start++;
//             jobs--;
//         }
//         a_idx +=m;
//         b_idx = 0;
//     }

//     // for (int i = 0; i < jobs; i++){
//     //     sum = 0;
//     //     // n m l
//     //     // 4 6 5
//     //     a_idx = (start / l) * m;
//     //     b_idx = m * (start % l);
//     //     c_idx = start;
//     //     // 1 1 1 1 1 1   1 1 1 1 1 1    . . . . .
//     //     // 2 2 2 2 2 2   2 2 2 2 2 2    . . . . .
//     //     // 3 3 3 3 3 3   3 3 3 3 3 3    . . . . .
//     //     // 4 4 4 4 4 4   4 4 4 4 4 4    . . . . .
//     //     //               5 5 5 5 5 5
//     //     //sum = 0;
//     //     //int k = start % l;
        
//     //     for (int j = 0; j < m; j++)
//     //     {
//     //         sum += a_mat[a_idx + j] * b_mat[b_idx];
//     //         b_idx++;
//     //     }
//     //     C[start] = sum;
//     //     start++;
        
//     // }
	
// 	MPI_Reduce(C, ans, mat_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// 	if(world_rank == 0){
//         c_idx = 0;
//         for (int i = 0; i < n; i++)
//         {
//             for(int j = 0 ; j < l ; j++){
// 				printf("%d ", ans[c_idx]);
// 				c_idx++;
// 			}
// 			//printf("\n");
// 			putchar('\n');
//         }
//     }
// }
// // Remember to release your allocated memory
// void destruct_matrices(int *a_mat, int *b_mat)
// {
//     int world_rank;
// 	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
// 	if(world_rank == 0){
// 		free(a_mat);
// 		free(b_mat);
// 	}
// }

//new
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Read size of matrix_a and matrix_b (n, m, l) and whole data of matrixes from stdin
//
// n_ptr:     pointer to n
// m_ptr:     pointer to m
// l_ptr:     pointer to l
// a_mat_ptr: pointer to matrix a (a should be a continuous memory space for placing n * m elements of int)
// b_mat_ptr: pointer to matrix b (b should be a continuous memory space for placing m * l elements of int)
inline void read(int &x)
{
	x = 0;
    char c = getchar();
	while(c < '0' || c > '9')
        c = getchar();
	while(c >= '0' && c <= '9') {
		x = x * 10 + c - '0';
		c = getchar();
	} 
}


void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr,
                        int **a_mat_ptr, int **b_mat_ptr)
{
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int *ptr;
    if(world_rank == 0){
        //printf("input : n m l \n");
        scanf("%d %d %d", n_ptr, m_ptr, l_ptr);

        int n = *n_ptr, m = *m_ptr, l = *l_ptr;
        //printf("n = %d m = %d l = %d \n", n, m, l);
        *a_mat_ptr = (int*)calloc(n * m, sizeof(int));
        *b_mat_ptr = (int*)calloc(m * l, sizeof(int));
        
        // // construct a matrix (n x m)
        // for (int i = 0; i < n; i++){
        //     for (int j = 0; j < m; j++){
        //         ptr = *a_mat_ptr + m * i + j;
        //         //printf("%p \n ", ptr);
        //         scanf("%d", ptr);
        //     }
        // }
        // //construct b transfer matrix
        // for (int i = 0; i < m; i++){
        //     for (int j = 0; j < l; j++){
        //         ptr = *b_mat_ptr + m * j + i;
        //         //printf("%p \n ", ptr);
        //         scanf("%d", ptr);
        //     }
        // }

        for(int i = 0 ; i < n*m ; i++){
			//scanf("%d", *a_mat_ptr+i);
			read(*(*a_mat_ptr + i));
		}
		for(int i = 0 ; i < m ; i++){
			for(int j = 0 ; j < l ; j++){
				//scanf("%d", *b_mat_ptr+i+j*(*m_ptr));
				read(*(*b_mat_ptr + i + j * m));
			}
		}
    }

    MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(m_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(l_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(world_rank != 0){
        *a_mat_ptr = (int*)malloc(sizeof(int) * (*n_ptr) * (*m_ptr));
        *b_mat_ptr = (int*)malloc(sizeof(int) * (*m_ptr) * (*l_ptr));
    }
    MPI_Bcast(*a_mat_ptr, (*n_ptr) * (*m_ptr), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(*b_mat_ptr, (*m_ptr) * (*l_ptr), MPI_INT, 0, MPI_COMM_WORLD);
    //printf("%d \n", world_rank);
    // if(world_rank == 2){
    //     for (int i = 0; i < *n_ptr; i++){
    //         for (int j = 0; j < *m_ptr; j++){
    //             ptr = *a_mat_ptr + *m_ptr * i + j;
    //             printf("%d  ", *ptr);
    //             //scanf("%d", ptr);
    //         }
    //         printf("\n");
    //     }
    // }
    
}

// Just matrix multiplication (your should output the result in this function)
// n:     row number of matrix a
// m:     col number of matrix a / row number of matrix b
// l:     col number of matrix b
// a_mat: a continuous memory placing n * m elements of int
// b_mat: a continuous memory placing m * l elements of int
void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat){
	int world_rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int *C, *ans;
	int mat_size, start, end, jobs;
    int a_idx, b_idx, c_idx;
    int sum;
    mat_size = n * l;
    C =   (int*)calloc(mat_size, sizeof(int));
	ans = (int*)calloc(mat_size, sizeof(int));
    jobs = mat_size / world_size;
    start = world_rank * jobs;
    if (world_rank == world_size - 1)
    {
        jobs += mat_size % world_size;
    }
    //printf("%d %d %d \n", world_rank, jobs, start);
    // a_idx = (start / l) * m;
    // b_idx = m * (start % l);
    // while(jobs > 0 ){
    //     sum = 0;
    //     // n m l
    //     // 4 6 5
    //     //a_idx = (start / l) * m;
    //     //b_idx = m * (start % l);
    //     c_idx = start;
    //     // 1 1 1 1 1 1   1 1 1 1 1 1    . . . . .
    //     // 2 2 2 2 2 2   2 2 2 2 2 2    . . . . .
    //     // 3 3 3 3 3 3   3 3 3 3 3 3    . . . . .
    //     // 4 4 4 4 4 4   4 4 4 4 4 4    . . . . .
    //     //               5 5 5 5 5 5
    //     //sum = 0;
    //     //int k = start % l;
    //     for(int k = c_idx % l ; k < l ; k++){
    //         sum = 0;
    //         for (int j = 0; j < m; j++)
    //         {
    //             sum += a_mat[a_idx + j] * b_mat[b_idx];
    //             b_idx++;
    //         }
    //         C[start] = sum;
    //         start++;
    //         jobs--;
    //     }
    //     a_idx += m;
    //     b_idx = 0;
    // }

    for (int i = 0; i < jobs; i++){
        sum = 0;
        // n m l
        // 4 6 5
        a_idx = (start / l) * m;
        b_idx = m * (start % l);
        //c_idx = start;
        // 1 1 1 1 1 1   1 1 1 1 1 1    . . . . .
        // 2 2 2 2 2 2   2 2 2 2 2 2    . . . . .
        // 3 3 3 3 3 3   3 3 3 3 3 3    . . . . .
        // 4 4 4 4 4 4   4 4 4 4 4 4    . . . . .
        //               5 5 5 5 5 5
        //sum = 0;
        //int k = start % l;
        
        for (int j = 0; j < m; j++)
        {
            sum += a_mat[a_idx + j] * b_mat[b_idx];
            b_idx++;
        }
        C[start] = sum;
        start++;
        
    }
	
	MPI_Reduce(C, ans, mat_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(world_rank == 0){
        c_idx = 0;
        for (int i = 0; i < n; i++)
        {
            for(int j = 0 ; j < l ; j++){
				printf("%d ", ans[c_idx]);
				c_idx++;
			}
			//printf("\n");
			putchar('\n');
        }
    }
}
// Remember to release your allocated memory
void destruct_matrices(int *a_mat, int *b_mat)
{
    int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	if(world_rank == 0){
		free(a_mat);
		free(b_mat);
	}
}


