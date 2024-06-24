#include <iostream>
#include <thread>
//#include "ostreamlock.h" - C++17
#include "stdio.h"

using namespace std;

static void greeting(string str, int i, int len) {
//     cout  << "Hello, world! I am thread " << i << endl;  // не потокобезопасно
//    cout << oslock << "Hello, world!" << endl << osunlock; // потокобезопасно, С++17
    string newstr = str.substr(i, len);
    printf("%s \n", newstr.c_str()); // тоже потокобезопасно

}


int main(int argc, char *argv[]) {

     static const size_t kNumFriends = thread::hardware_concurrency();
     cout << "Let's hear from " << kNumFriends << " threads." << endl;

     thread friends[kNumFriends]; // declare array of empty thread handles
     string largestring = "one large string to share between threads";
     // Spawn threads
     int len = largestring.length() / kNumFriends;

     for (size_t i = 0; i < kNumFriends; i++) {
         friends[i] = thread(greeting, largestring, i*len, len);
     }
     
     // Wait for threads
     for (size_t i = 0; i < kNumFriends; i++) {
         friends[i].join();
         }

     cout << "Everyone's said!" << endl;
     return 0;
     }
