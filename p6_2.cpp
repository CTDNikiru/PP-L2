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

void primitiveSito(void *arg)
{
    SitoParams *params = (SitoParams *)arg;
    int start = params->start_value;
    int temp = 2 * start;
    while (temp <= global_param->count_elements)
    {
        //cout << "set index of " << temp << endl;
        global_param->array_ptr[temp] = 1;
        temp += start;
    }
}

int main()
{
    int array_size = 10000000;

    int *array = new int[array_size];

    // Выставляем начальные значения
    for (int i = 0; i < array_size; i++)
    {
        array[i] = 0;
    }
    global_param = new Param;

    global_param->array_ptr = array;
    global_param->count_elements = array_size;

    auto begin = chrono::steady_clock::now();

    // Если число - простое, отправляем его на вычисление
    for (int i = 0; i < array_size; i++)
    {
        if (i < 2)
        {
            //cout << i << endl;
        }
        else
        {
            //Проверяем выставлено ли уже значение
            if (global_param->array_ptr[i] != 1)
            {
                //Если значение нет, возможно это простое, проверяем
                if (isPrime(i))
                {
                    //cout << i << endl;
                    SitoParams *sito = new SitoParams;
                    sito->start_value = i;
                    primitiveSito(sito);
                }
            }
        }
    }

    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<std::chrono::microseconds>(end - begin);

    cout << "time: " << time.count() << endl;
    cout << "func count " << func_count << endl;
}