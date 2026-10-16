#include <atomic>
#include <thread>
int main(){std::atomic<int> x{0};std::thread t([&]{++x;});++x;t.join();return x==2?0:1;}
