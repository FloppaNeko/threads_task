#include <iostream>
#include <sstream>
#include <fstream>


// This function tries to open config file and parse parameters from it
void get_params_from_file(std::ifstream* fin_ptr,
    int* num_readers_ptr, int* num_writers_ptr, std::ofstream* fout_ptr) {

    while(!(fin_ptr->eof())) {
        std::string line; 
        std::getline(*fin_ptr, line);

        if (line.substr(0, 8) == "readers=") {
            std::string num_str = line.substr(8);
            *num_readers_ptr = std::stoi(num_str);

            if (*num_readers_ptr < 0) {
                throw std::runtime_error("Readers number is negative!");
            }

        } else if (line.substr(0, 8) == "writers=") {
            std::string num_str = line.substr(8);
            *num_writers_ptr = std::stoi(num_str);

            if (*num_writers_ptr < 0) {
                throw std::runtime_error("Readers number is negative!");
            }

        } else if (line.substr(0, 7) == "output=") {
            std::string fout_name = line.substr(7);

            fout_ptr->open(fout_name);
            if (!(fout_ptr->is_open())) {
                throw std::runtime_error("Cannot open output file!");
            }
        }
    }
}


// This function performs the scenario of getting programm parameters from the three sourses 
// (config file, cmd parameters or console requests to user)
void get_params(int argc, char* argv[],
    int* num_readers_ptr, int* num_writers_ptr, std::ofstream* fout_ptr) {

    for (int i = 0; i < argc; ++i) {
        std::string line = argv[i];

        if (line.substr(0, 7) == "config=") {
            std::string fin_name = line.substr(7);

            std::ifstream fin(fin_name);
            if (!fin.is_open()) {
                throw std::runtime_error("Cannot open config file!");
            }

            get_params_from_file(&fin, num_readers_ptr, num_writers_ptr, fout_ptr);
            break;

        } else if (line.substr(0, 8) == "readers=") {
            std::string num_str = line.substr(8);
            *num_readers_ptr = std::stoi(num_str);

            if (*num_readers_ptr < 0) {
                throw std::runtime_error("Readers number is incorrect!");
            }

        } else if (line.substr(0, 8) == "writers=") {
            std::string num_str = line.substr(8);
            *num_writers_ptr = std::stoi(num_str);

            if (*num_writers_ptr < 0) {
                throw std::runtime_error("Readers number is incorrect!");
            }

        } else if (line.substr(0, 7) == "output=") {
            std::string fout_name = line.substr(7);

            fout_ptr->open(fout_name);
            if (!(fout_ptr->is_open())) {
                throw std::runtime_error("Cannot open output file!");
            }
        }
    }

    if (*num_readers_ptr == -1) {
        std::cout << "Input the number of reader threads: ";
        std::cin >> *num_readers_ptr;

        if (*num_readers_ptr < 0) {
            if (*num_readers_ptr < 0) {
                throw std::runtime_error("Readers number is incorrect!");
            }
        }
    }

    if (*num_writers_ptr == -1) {
        std::cout << "Input the number of writer threads: ";
        std::cin >> *num_writers_ptr;

        if (*num_writers_ptr < 0) {
            if (*num_writers_ptr < 0) {
                throw std::runtime_error("Writers number is incorrect!");
            }
        }
    }

    if (!(fout_ptr->is_open())) {
        std::string fout_name;

        std::cout << "Input the output file name (print N if file output not reqired): ";
        std::cin >> fout_name;

        if (fout_name != "N") {
            fout_ptr->open(fout_name);
            if (!(fout_ptr->is_open())) {
                throw std::runtime_error("Cannot open output file!");
            }
        }
    }
}


// This function prints the contents pf a stringstream to the console and the the output file if it was created
void print_stringstream(const std::stringstream& ss, std::ofstream& fout) {
    std::cout << ss.str();

    if (fout.is_open()) {
        fout << ss.str();
    }
}
