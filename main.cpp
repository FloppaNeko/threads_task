#include <iostream>
#include <sstream>

#include <random>
#include <ctime>

#include <pthread.h>
#include <unistd.h> 

#include <algorithm>

#include "io.cpp"


std::ofstream fout;

std::mt19937 get_random(time(nullptr));

const int DATA_ARRAY_LENGTH = 1000;
const int MAX_VALUE = 1000000;

int data_array[DATA_ARRAY_LENGTH];

pthread_mutex_t access_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;


// This function fills data_array with random values and sorts it
void init_data_array() {
    for (int i = 0; i < DATA_ARRAY_LENGTH; ++i) {
        data_array[i] = get_random() % MAX_VALUE;
    }
    std::sort(data_array, data_array + DATA_ARRAY_LENGTH);
}

// This function makes the thread it was called from sleep a random number of seconds (from 1 to 3)
void rand_wait() {
    sleep(get_random() % 3 + 1);
}


// This functions returns the array to sorted order after one of its elements was changed
void balance_array(int i) {
    while (i+1 < DATA_ARRAY_LENGTH && data_array[i] > data_array[i+1]){
        std::swap(data_array[i], data_array[i+1]);
        ++i;
    }
    while (i-1 >= 0 && data_array[i-1] > data_array[i]) {
        std::swap(data_array[i-1], data_array[i]);
        --i;
    }
}


// Reader function (reads the value of a single random cell)
void* reader(void* ptr) {
    while (true) {
        int idx = get_random() % DATA_ARRAY_LENGTH;
        int val = data_array[idx];

        std::stringstream ss;
        ss << "Reader " << ptr << " read data_array[" << idx << "] = " << val << '\n';

        pthread_mutex_lock(&print_mutex);
        print_stringstream(ss, fout);
        pthread_mutex_unlock(&print_mutex);

        rand_wait();
    }

    return nullptr;
}


// Writer function (changes the value in a random array cell to another random value, then fixes the sorted order in the array)
void* writer(void* ptr) {
    while (true) {
        int idx = get_random() % DATA_ARRAY_LENGTH;
        int prev_val = data_array[idx];
        int new_val = get_random() % MAX_VALUE;

        pthread_mutex_lock(&access_mutex);
        data_array[idx] = new_val;
        balance_array(idx);
        pthread_mutex_unlock(&access_mutex);

        std::stringstream ss;
        ss << "Writer " << ptr << " changed data_array[" << idx << "] from " << prev_val << " to " << new_val << '\n';

        pthread_mutex_lock(&print_mutex);  
        print_stringstream(ss, fout);
        pthread_mutex_unlock(&print_mutex); 

        rand_wait();
    } 

    return nullptr;
}


int main(int argc, char* argv[]) {
    init_data_array();

    int num_readers = -1;
    int num_writers = -1;

    get_params(argc, argv, &num_readers, &num_writers, &fout);

    pthread_t readers[num_readers];
    pthread_t writers[num_writers];

    // reader threads
    for (int i = 0; i < num_readers; i++) {
        pthread_create(&readers[i], nullptr, reader, (void*)(readers + i));

        std::stringstream ss;
        ss << "Reader thread " << readers + i << " started!" << '\n';

        pthread_mutex_lock(&print_mutex);
        print_stringstream(ss, fout);
        pthread_mutex_unlock(&print_mutex);
    }

    // writer threads
    for (int i = 0; i < num_writers; i++) {
        pthread_create(&writers[i], nullptr, writer, (void*)(writers + i));

        std::stringstream ss;
        ss << "Writer thread " << writers + i << " started!" << '\n';

        pthread_mutex_lock(&print_mutex);
        print_stringstream(ss, fout);
        pthread_mutex_unlock(&print_mutex);
    }

    // Join threads (infinite loop, so this won't actually terminate)
    for (int i = 0; i < num_readers; i++) {
        pthread_join(readers[i], nullptr);
    }

    for (int i = 0; i < num_writers; i++) {
        pthread_join(writers[i], nullptr);
    }

    return 0;
}
