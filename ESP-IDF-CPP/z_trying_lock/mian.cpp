#include <mutex>
#include <iostream>

static std::mutex my_mutex;

void fun_1(void){
    std::lock_guard<std::mutex> lock(my_mutex);
    std::lock_guard<std::mutex> lock_2(my_mutex);
    std::cout << "fun_1" << std::endl;
}

void fun_3(void){
    std::lock_guard<std::mutex> lock_3(my_mutex);
    std::cout << "fun_3" << std::endl;
}

void fun_2(void){
    std::lock_guard<std::mutex> lock(my_mutex);
    fun_3();
    std::cout << "fun_2" << std::endl;
}




int main(void){
    while(1){
        fun_1();
        fun_2();
        fun_3();
    }
}
