#include <iostream>
#include <thread>
//#include "ostreamlock.h" - C++17
#include "stdio.h"

using namespace std;

static void greeting(size_t &i) {
//     cout  << "Hello, world! I am thread " << i << endl;  // не потокобезопасно
//    cout << oslock << "Hello, world!" << endl << osunlock; // потокобезопасно, С++17
    printf("Hello, world! I am thread %d \n", i); // тоже потокобезопасно

}

static const size_t kNumFriends = 6;
int main(int argc, char *argv[]) {
     cout << "Let's hear from " << kNumFriends << " threads." << endl;

     thread friends[kNumFriends]; // declare array of empty thread handles

     // Spawn threads
     for (size_t i = 0; i < kNumFriends; i++) {
         friends[i] = thread(greeting, ref(i));
     }

     // Wait for threads
     for (size_t i = 0; i < kNumFriends; i++) {
         friends[i].join();
         }

     cout << "Everyone's said hello!" << endl;
     return 0;
     }
