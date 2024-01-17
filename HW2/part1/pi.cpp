#define __AVX2_AVAILABLE__
# include <iostream>
# include <cstdlib>
# include <ctime>
#include <pthread.h>
#include <random>
#include <time.h>
#include <stdlib.h>
#include "Xoshiro256Plus.h"
typedef SEFUtility::RNG::Xoshiro256Plus<SIMDInstructionSet::AVX2> Xoshiro256PlusAVX2;

using namespace std;
pthread_mutex_t mutex;
//global 4 all thread
long long int num_in_circle = 0;
void *helper(void *arg){

    int *num_job = (int *)arg;
    __m256 rand_max = _mm256_set1_ps(RAND_MAX);
    unsigned int seed = 600;
    Xoshiro256PlusAVX2 avx_rng(rand_r(&seed));
    __m256 one = _mm256_set1_ps(1.0f);
    long long int test = 0;
    long long int tmp = 0;
    __m256 total = _mm256_set1_ps(0);
    
    for (int toss = 0; toss < *num_job; toss+= 8)
    {
        
        __m256 rand_x = _mm256_cvtepi32_ps(avx_rng.next4());
        __m256 rand_y = _mm256_cvtepi32_ps(avx_rng.next4());

        // for(int i = 0; i < 8; i++){
        //     //rand_x[i] = rand() % (RAND_MAX + 1);
        //     //rand_y[i] = rand() % (RAND_MAX + 1);
        //     cout << x[i] <<" ";
        // }
        // cout <<endl;

       __m256 x = _mm256_div_ps(rand_x, rand_max);  
        __m256 y = _mm256_div_ps(rand_y, rand_max);  

        // for(int i = 0; i < 8; i++){
        //     //rand_x[i] = rand() % (RAND_MAX + 1);
        //     //rand_y[i] = rand() % (RAND_MAX + 1);
        //     cout << x[i] <<" ";
        // }
        // cout <<endl;
        //cout << INT32_MAX <<" "<<RAND_MAX;
        __m256 x_1 = _mm256_mul_ps(x, x);
        __m256 y_1 = _mm256_mul_ps(y, y);
        __m256 sum = _mm256_add_ps(x_1, y_1);
        // int tt= 0;
        // for(int i = 0; i < 8; i++){
        //     if(sum[i] <= 1.f)   tt++;
        // }

        __m256 count = _mm256_cmp_ps(sum, one, _CMP_LE_OQ);
        __m256 circle = _mm256_and_ps(one, count);
        
        total = _mm256_add_ps(circle,total);

        if(total[0] >= 134217728/8){
            total = _mm256_set1_ps(0);
            test += 134217728;
        }
        
        // for(int i = 0; i < 8 ;i++){
        //     test += int(circle[i]);
        // }
        // __m256 circle_permute = _mm256_permute2f128_ps(circle, circle, 1);
        // circle = _mm256_hadd_ps(circle, circle_permute);
        // circle = _mm256_hadd_ps(circle, circle);
        // circle = _mm256_hadd_ps(circle, circle);

        //if(circle[0] != tt) cout <<"0.0"<<endl;
        //test += int(circle[0]);
    }
    __m256 total_permute = _mm256_permute2f128_ps(total, total, 1);
    total = _mm256_hadd_ps(total, total_permute);
    total = _mm256_hadd_ps(total, total);
    total = _mm256_hadd_ps(total, total);
    //printf("%llu \n",test);
    pthread_mutex_lock(&mutex);
    // for(int i = 0; i < 8 ; i++){
    //     tmp += int(total[i]);
    // }
    // if(tmp != test) printf("%llu, %llu \n" , tmp, test);
    num_in_circle += int(total[0]);
    num_in_circle += test;
    pthread_mutex_unlock(&mutex);
    //printf("The thread ID is %lu\n", pthread_self());
    pthread_exit(NULL);
}

int main(int argc, char ** argv)
{   
    srand(time(nullptr));
    //clock_t start = clock();
    int total_thread = atoi(argv[1]);
    long long int total_toss = atoll(argv[2]);
    int thread_job = (total_toss / total_thread);
    //cout << total_thread << " " << total_toss << " " << thread_job << endl;
    pthread_t threads[total_thread];
    //pthread_mutex_init(&mutex, 0);

    for (int i = 0; i < total_thread; i++){
        pthread_create(&threads[i], NULL, helper, (void *) &thread_job);
    }
    

    void *status;
    //cout << "end1" << endl;
    for (int i = 0; i < total_thread; i++)
    {
        pthread_join(threads[i], &status);
        //cout << i << endl;
    }
    //cout << "end join" << endl;
    //pthread_mutex_destroy(&mutex);
    double pi = 4 * num_in_circle / ((double)total_toss);
    printf("%.7lf\n", pi);
    //clock_t end = clock();
    //cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
    //cout << "pi: " << pi << endl;
    return 0;
}
