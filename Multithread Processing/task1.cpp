#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

using namespace std;

// ЧТОБЫ УДОБНО КОМПИЛИРОВАТЬ КОД, РЕАЛИЗУЙТЕ КАЖДОЕ ЗАДАНИЕ В ВИДЕ ФУНКЦИЙ

// 1. Создайте поток, который выведет текущее время с помощью передачи в поток функции print_time()
// Для получения времени используйте функцию std::chrono::system_clock::now(); (1 балл)
void print_time()
{
    const auto now = chrono::system_clock::now();
    const time_t t_c = chrono::system_clock::to_time_t(now);
    cout << ctime(&t_c);
}

void Task1()
{
    cout << "\nTask 1\n";
    thread thread(print_time);
    thread.join();
}

// 2. Создайте объект-функтор, делающий то же самое, только время считайте с помощью std::time (1 балл)
class Functor
{
public:
    Functor() = default;
    ~Functor() = default;

    void operator()()
    {
        time_t result = time(nullptr);
        cout << asctime(localtime(&result)) << endl;
    }
};

void Task2()
{
    cout << "\nTask 2\n";
    Functor functor;
    thread thread(functor);
    thread.join();
}

// 3. Создайте поток, который вернет текущее время по ссылке, используя передачу задачи с помощью лямба - функции
// После завершения потока снова замерьте текущее время и напечатайте пройденное время(разницу в мс).(1 балл) void Task3()
void SetTime(chrono::_V2::high_resolution_clock::time_point &timePoint)
{
    timePoint = chrono::high_resolution_clock::now();
}

void Task3()
{
    cout << "\nTask 3\n";
    chrono::_V2::high_resolution_clock::time_point start;
    thread timeThread(SetTime, ref(start));
    timeThread.join();
    auto finish = chrono::high_resolution_clock::now();
    auto dur = chrono::duration_cast<chrono::microseconds>(finish - start);
    cout << dur.count() << "\n";
}

// 4. Создайте столько потоков, сколько доступно на вашем компьютере
//  (std::thread::hardware_concurrency())
//  Каждый поток должен напечатать свой id. Особое внимание обратите на вывод, чтобы он был согласованным (1 балл)
void Task4()
{
    cout << "\nTask 4\n";
    int n = thread::hardware_concurrency();
    vector<thread> threads(n);
    for (int i = 0; i < n; ++i)
    {
        threads[i] = thread([]
                            { this_thread::sleep_for(chrono::milliseconds(1000)); });
        auto id = threads[i].get_id();
        printf("%d", id);
        if (i + 1 == n)
        {
            printf("\n");
        }
        else
        {
            printf("\t");
        }
    }

    for (int i = 0; i < n; ++i)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
}

// 5. Напечатайте числа ПОСЛЕДОВАТЕЛЬНО от 0 до 10 так, чтобы четный числа выводил один поток, а нечетные - другой.
// Требуемый вывод: 0 1 2 3 4 5 6 7 8 9 10. Используйте механизм блокировки. (2 балла)
mutex mtx;

void PrintEven()
{
    for (int i = 0; i <= 10; i += 2)
    {
        mtx.lock();

        printf("%d\t", i);

        // cout << i << " ";

        this_thread::sleep_for(chrono::milliseconds(1));
        mtx.unlock();
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void PrintOdd()
{
    this_thread::sleep_for(chrono::milliseconds(1));
    for (int i = 1; i <= 10; i += 2)
    {
        mtx.lock();

        printf("%d", i);
        // cout << i;
        if (i == 10)
        {
            printf("\n");
        }
        else
        {
            printf("\t");
        }

        this_thread::sleep_for(chrono::milliseconds(1));
        mtx.unlock();
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void Task5()
{
    cout << "\nTask 5\n";
    thread even(PrintEven);
    thread odd(PrintOdd);
    even.join();
    odd.join();
}

// 6. Создайте две матрицы А и B размером 300х300. Напишите параллельную версию перемножения матриц с записью результата
// в отдельную матрицу С. (3 балла)
// Cоздайте 6 потоков (не считая родительский).  1 поток должен перемножать строки 0-49 на столбцы 0-49,
// 2 поток - строки 50-99 на столбцы 50-99 и так далее.
// Далее одна из вариаций кода переножения матрицы 3х3:
const int MATRIX_SIZE = 300;

void Multiply(int from, int to, const vector<vector<float>> &A, const vector<vector<float>> &B, vector<vector<float>> &C)
{
    for (int i = from; i < to; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void Task6()
{
    cout << "\nTask 6\n";
    vector<vector<float>> A(MATRIX_SIZE);
    ifstream fstreamA;
    fstreamA.open("A.txt");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        A[i] = vector<float>(MATRIX_SIZE);
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fstreamA >> A[i][j];
        }
    }
    fstreamA.close();

    vector<vector<float>> B(MATRIX_SIZE);
    ifstream fstreamB;
    fstreamB.open("B.txt");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        B[i] = vector<float>(MATRIX_SIZE);
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fstreamB >> B[i][j];
        }
    }
    fstreamB.close();

    vector<vector<float>> C(MATRIX_SIZE);
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        C[i] = vector<float>(MATRIX_SIZE);
    }

    auto first = chrono::high_resolution_clock::now();
    vector<thread> threads(6);
    for (int i = 0; i < 6; i++)
    {
        threads[i] = thread(Multiply, 50 * i, 50 * (i + 1), A, B, ref(C));
    }

    for (int i = 0; i < 6; i++)
    {
        threads[i].join();
    }

    ofstream fstreamC;
    fstreamC.open("C.txt");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fstreamC << C[i][j];
        }
    }
    fstreamC.close();
}

// 7. Замерьте время работы параллельного перемножения матриц и последовательного кода.
//  Сравните и объясните результаты. (2 балла)
void Task7()
{
    cout << "\nTask 7\n";
    vector<vector<float>> A(MATRIX_SIZE);
    ifstream fstreamA;
    fstreamA.open("A.txt");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        A[i] = vector<float>(MATRIX_SIZE);
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fstreamA >> A[i][j];
        }
    }
    fstreamA.close();

    vector<vector<float>> B(MATRIX_SIZE);
    ifstream fstreamB;
    fstreamB.open("B.txt");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        B[i] = vector<float>(MATRIX_SIZE);
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fstreamB >> B[i][j];
        }
    }
    fstreamB.close();

    vector<vector<float>> C(MATRIX_SIZE);
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        C[i] = vector<float>(MATRIX_SIZE);
    }

    auto first = chrono::high_resolution_clock::now();
    vector<thread> threads(6);
    for (int i = 0; i < 6; i++)
    {
        threads[i] = thread(Multiply, 50 * i, 50 * (i + 1), A, B, ref(C));
    }

    for (int i = 0; i < 6; i++)
    {
        threads[i].join();
    }
    auto second = chrono::high_resolution_clock::now();
    auto dur = chrono::duration_cast<chrono::milliseconds>(second - first);
    cout << "On 6 threads in parallel: " << dur.count() << " milliseconds\n";

    first = chrono::high_resolution_clock::now();
    Multiply(0, 300, A, B, C);
    second = chrono::high_resolution_clock::now();
    dur = chrono::duration_cast<chrono::milliseconds>(second - first);
    cout << "On a sinlge thread: " << dur.count() << " milliseconds\n";

    ofstream fstreamC;
    fstreamC.open("C.txt");
    for (int i = 0; i < MATRIX_SIZE; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            fstreamC << C[i][j];
        }
    }
    fstreamC.close();
}

int main()
{
    Task1();
    Task2();
    Task3();
    Task4();
    Task5();
    Task6();
    Task7();
}
