#include <iostream>
#include <thread>

class background_task {
public:
    void operator()() const
    {
        std::cout << "Hello, Multithreaded World!\n"; }
};

int main()
{
    background_task bgtask;
    std::thread mythread(bgtask); // передача функтора в поток
    // продолжение работы в главном потоке
    mythread.join();
    return 0;
}

