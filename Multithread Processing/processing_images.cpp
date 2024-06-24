#include <iostream>
#include <thread>

#include <chrono>

using namespace std;

static void process (size_t id, size_t& remainingImages) {
    while (remainingImages > 0) {
        this_thread::sleep_for(500ms);
        remainingImages--;
    }
}

int main(int argc, const char *argv[]) {
    thread processors[10];
    size_t remainingImages = 250;
    for (size_t i = 0; i < 10; i++)
        processors[i] = thread(process, 101 + i, ref(remainingImages));
    for (thread& proc: processors) proc.join();
    cout << "Images done!" << endl;
    return 0;
}