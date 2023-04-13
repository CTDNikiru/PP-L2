#include <cstdlib>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <pthread.h>
#include <thread>
#include <chrono>
#include <random>
using namespace std;
#define err_exit(code, str)                   \
    {                                         \
        cerr << str << ": " << strerror(code) \
             << endl;                         \
        exit(EXIT_FAILURE);                   \
    }
const int TASKS_COUNT = 20;
int task_list[TASKS_COUNT]; // Массив заданий
int current_task = 0;       // Указатель на текущее задание
pthread_spinlock_t spin;      // Спинлок
void do_task(int task_no)
{
    cout << "i do task " << task_no << endl;

    //random_device rd;   // non-deterministic generator
    //mt19937 gen(rd());  // to seed mersenne twister.
    //uniform_int_distribution<> dist(1500,2000);

    
    //std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
}
void *thread_job(void *arg)
{
    int task_no;
    int err;
    // Перебираем в цикле доступные задания
    while (true)
    {
        // Захватываем мьютекс для исключительного доступа
        // к указателю текущего задания (переменная
        // current_task)
        err = pthread_spin_lock(&spin);
        if (err != 0)
            err_exit(err, "Cannot lock spin");
        // Запоминаем номер текущего задания, которое будем исполнять
        task_no = current_task;
        // Сдвигаем указатель текущего задания на следующее
        current_task++;
        // Освобождаем спин
        err = pthread_spin_unlock(&spin);
        if (err != 0)
            err_exit(err, "Cannot unlock spin");
        // Если запомненный номер задания не превышает
        // количества заданий, вызываем функцию, которая
        // выполнит задание.
        // В противном случае завершаем работу потока
        if (task_no < TASKS_COUNT)
            do_task(task_no);
        else
            return NULL;
    }
}
int main()
{
    const auto start = std::chrono::high_resolution_clock::now();
    int threads_count = 4;
    auto threads = new pthread_t[threads_count];
    // Идентификаторы потоков
    int err;
    // Код ошибки
    // Инициализируем массив заданий случайными числами
    for (int i = 0; i < TASKS_COUNT; ++i)
        task_list[i] = rand() % TASKS_COUNT;
    // Инициализируем спин
    err = pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);
    if(err != 0)
        err_exit(err, "Cannot initialize spinlock");
    for(int i = 0; i<threads_count; i++){
        // Создаём потоки
        err = pthread_create(&threads[i], NULL, thread_job, NULL);
        if (err != 0)
            err_exit(err, "Cannot create thread");
    }

    for(int i = 0; i<threads_count; i++){
        pthread_join(threads[i], NULL);
    }

    // Освобождаем ресурсы, связанные с мьютексом
    pthread_spin_destroy(&spin);

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> elapsed = end - start;
    cout << "Elapsed miliseconds: " << elapsed.count() << endl;
}