#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <chrono>

using namespace std;

struct Param
{
    int *array_ptr;
    int count_elements;
    int start_value;
} *global_param;

struct SitoParams
{
    int start_value;
    int end_value;
};

int func_count = 0;

bool isPrime(int n)
{
    func_count++;
    if (n <= 1)
        return false;
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

void primitiveSito(int start, int max)
{
    int temp = 2 * start;
    while (temp <= max)
    {
        // cout << "set index of " << temp << endl;
        global_param->array_ptr[temp] = 1;
        temp += start;
    }

    // cout << "endthread" << endl;
}

void *segmentedSito(void *arg)
{
    SitoParams *params = (SitoParams *)arg;
    int start = params->start_value;
    int end = params->end_value;

    for (int i = start; i < end; i++)
    {
        // Проверяем выставлено ли уже значение
        if (global_param->array_ptr[i] != 1)
        {
            // Если значение нет, возможно это простое, проверяем
            if (isPrime(i))
            {
                //cout << i << endl;
                primitiveSito(i, end);
            }
        }
    }
}

int main()
{
    int array_size = 10000000;

    int *array = new int[array_size];

    int err;

    // Выставляем начальные значения
    for (int i = 0; i < array_size; i++)
    {
        array[i] = 0;
    }
    global_param = new Param;

    global_param->array_ptr = array;
    global_param->count_elements = array_size;

    int nthreads = 10;

    // Массив аргументов, которые будут переданы в функции потоков
    SitoParams *params = new SitoParams[nthreads];
    // Массив потоков
    pthread_t *threads = new pthread_t[nthreads];

    int temp_count = array_size / nthreads;

    for (int i = 0; i < nthreads-1; i++)
    {
        params[i].start_value = i*temp_count;
        params[i].end_value = params[i].start_value + temp_count - 1;
    }

    params[nthreads-1].start_value = params[nthreads-2].end_value;
    params[nthreads-1].end_value = array_size;

    auto begin = chrono::steady_clock::now();
    
    for (int i = 0; i < nthreads; i++)
    {

        // Создание потока
        err = pthread_create(&threads[i], NULL, segmentedSito, (void *)&params[i]);
        if (err != 0)
        {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            exit(-1);
        }
    }

    // Ждем завершения всех созданных потоков
    for (int i = 0; i < nthreads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<std::chrono::microseconds>(end - begin);

    cout << "time: " << time.count() << endl;
    cout << "func count " << func_count << endl;
}