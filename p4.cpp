#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
using namespace std;
#define err_exit(code, str)                   \
    {                                         \
        cerr << str << ": " << strerror(code) \
             << endl;                         \
        exit(EXIT_FAILURE);                   \
    }
enum signal_state
{
    OFF,
    ON
} state = signal_state::OFF;
pthread_mutex_t mutex;
pthread_cond_t cond;
void *switch_on(void *arg)
{
    int err;
    //
    while (true)
    {
        // Захватываем мьютекс и ожидаем сигнал на отключение
        err = pthread_mutex_lock(&mutex);
        if (err != 0)
            err_exit(err, "Cannot lock mutex");
        while (state == ON)
        {
            err = pthread_cond_wait(&cond, &mutex);
            if (err != 0)
                err_exit(err, "Cannot wait on condition variable");
        }
        // Получен сигнал на отключение
        // Включаем вновь
        state = ON;
        cout << "Chanel on" << endl;
        // Посылаем сигнал, что на складе появился товар.
        err = pthread_cond_signal(&cond);
        if (err != 0)
            err_exit(err, "Cannot send signal");
        err = pthread_mutex_unlock(&mutex);
        if (err != 0)
            err_exit(err, "Cannot unlock mutex");
    }
}

void *switch_off(void *arg)
{
    int err;
    //
    while (true)
    {
        // Захватываем мьютекс и ожидаем сигнала на включение
        err = pthread_mutex_lock(&mutex);
        if (err != 0)
            err_exit(err, "Cannot lock mutex");
        while (state == OFF)
        {
            err = pthread_cond_wait(&cond, &mutex);
            if (err != 0)
                err_exit(err, "Cannot wait on condition variable");
        }
        // Получен сигнал
        cout << "Chanel stopping" << endl;
        sleep(1);
        state = OFF;
        cout << "Chanel off" << endl;
        cout << "done" << endl;
        
        // Посылаем сигнал, что на складе не осталось товаров.
        err = pthread_cond_signal(&cond);
        if (err != 0)
            err_exit(err, "Cannot send signal");
        err = pthread_mutex_unlock(&mutex);
        if (err != 0)
            err_exit(err, "Cannot unlock mutex");
    }
}
int main()
{
    int err;
    //
    pthread_t thread1, thread2;
    // Идентификаторы потоков
    // Инициализируем мьютекс и условную переменную
    err = pthread_cond_init(&cond, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize condition variable");
    err = pthread_mutex_init(&mutex, NULL);
    if (err != 0)
        err_exit(err, "Cannot initialize mutex");
    // Создаём потоки
    err = pthread_create(&thread1, NULL, switch_on, NULL);
    if (err != 0)
        err_exit(err, "Cannot create thread 1");
    err = pthread_create(&thread2, NULL, switch_off, NULL);
    if (err != 0)
        err_exit(err, "Cannot create thread 2");
    // Дожидаемся завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    // Освобождаем ресурсы, связанные с мьютексом
    // и условной переменной
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}