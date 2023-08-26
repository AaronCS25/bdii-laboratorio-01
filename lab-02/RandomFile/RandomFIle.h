//
// Created by juaquin on 24/08/23.
//

#ifndef INC_2_1_RANDOMFILE_H
#define INC_2_1_RANDOMFILE_H

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <cstring>

namespace RF {
    struct Record {
        int codigo;
        int ciclo;

        Record() {}
        Record(int codigo, int ciclo) {
            this->codigo = codigo;
            this->ciclo = ciclo;
        }

        void print() {
            std::cout << "Codigo: " << this->codigo << std::endl;
            std::cout << "Ciclo: " << this->ciclo << std::endl;
        }

        int& getKey() {
            return this->codigo;
        }
    };
}


template<typename T>
struct RandomIndex {
    std::string fileName;
    std::unordered_map<T, size_t> data;

    RandomIndex(std::string _fileName): fileName(_fileName+".index") {
        std::ifstream file(this->fileName, std::ios::binary | std::ios::app);
        if (!file.is_open()) throw std::runtime_error("Can't open file");

        file.seekg(0);
        const int size = sizeof(T)+sizeof(size_t);
        char* buffer = new char[size];

        while(file.peek() != EOF) {
            char* start = buffer;
            file.read(buffer, size);

            T key;
            memcpy(&key, buffer, sizeof(T));
            buffer += sizeof(T);

            size_t pos;
            memcpy(&pos, buffer, sizeof(size_t));
            buffer = start;
            this->data.insert({key, pos});
        }
        delete []buffer;
        file.close();
    }

    ~RandomIndex() {
        std::ofstream file(this->fileName, std::ios::binary);
        if (!file.is_open()) throw std::runtime_error("Can't open file");

        file.seekp(0);
        const int size = sizeof(T)+sizeof(size_t);
        char* buffer = new char[size];
        for(auto& it: this->data) {
            char* start = buffer;
            std::memcpy(buffer, &it.first, sizeof(T));
            buffer += sizeof(T);
            std::memcpy(buffer, &it.second, sizeof(size_t));

            file.write(start, size);
            buffer = start;
        }
        delete []buffer;
    }
};

class RandomFile {
std::string fileName;
typedef int T;
RandomIndex<T>* index;

public:
    RandomFile(std::string _fileName): fileName(_fileName) {
        index = new RandomIndex<int>(_fileName);

        std::ofstream file(this->fileName, std::ios::app | std::ios::binary);
        file.close();
    }

    RF::Record search(T key) {
        auto t = this->index->data.find(key);
        if (t == nullptr) throw std::runtime_error("key not found");

        std::ifstream file(this->fileName, std::ios::app | std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("Can't open file");

        file.seekg(t->second);
        RF::Record record;
        file.read((char*)& record, sizeof(record));
        file.close();

        return record;
    }

    void scanAll() {
        std::ifstream file(this->fileName, std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("Can't open file");

        file.seekg(0);
        while(file.peek() != EOF) {
            RF::Record record;
            file.read((char*)&record, sizeof(record));
            record.print();
        }
        file.close();
    }

    void scanAllByIndex() {
        std::ifstream file(this->fileName, std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("Can't open file");

        for(auto& it: this->index->data) {
            RF::Record record;
            file.seekg(it.second);
            file.read((char*)&record, sizeof(record));
            record.print();
        }
    }

    bool insert(RF::Record record) {
        std::ofstream file(this->fileName, std::ios::app | std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("Can't open file");

        file.seekp(0, std::ios::end);
        size_t pos = file.tellp();
        auto t = this->index->data.insert({record.getKey(), pos});
        if (!t.second) return false;

        file.write((char*)&record, sizeof(record));
        file.close();
        return true;
    }

    ~RandomFile() {
        delete index;
    }
};

void test1() {
    RandomFile rf("random1.bin");
    rf.insert(RF::Record(1, 55));
    rf.insert(RF::Record(2, 56));
    rf.insert(RF::Record(3, 1));
    std::cout << "SCAN ALL" << std::endl;
    rf.scanAll();
    std::cout << "SCAN ALL BY INDEX" << std::endl;
    rf.scanAllByIndex();
    rf.search(1).print();
}

#endif //INC_2_1_RANDOMFILE_H
