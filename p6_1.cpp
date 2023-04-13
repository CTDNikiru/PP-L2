#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <chrono>

using namespace std;

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

int main()
{
    int array_size = 10000000;

    int *array = new int[array_size];

    // Выставляем начальные значения
    for (int i = 0; i < array_size; i++)
    {
        array[i] = 0;
    }

    auto begin = chrono::steady_clock::now();
    for (int i = 0; i < array_size; i++)
    {
        if (i < 2)
        {
            //cout << i << endl;
        }
        else
        {
            if (isPrime(i))
            {
                //cout << i << endl;
            }
        }
    }

    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<std::chrono::microseconds>(end - begin);

    cout << "time: " << time.count() << endl;

    cout << "func count " << func_count << endl;
}