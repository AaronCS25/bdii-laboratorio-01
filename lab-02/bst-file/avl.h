#include <iostream>
#include <string>
#include <fstream>
#include <vector>

struct Record {
    int codigo;
    int ciclo;

    int left = -1;
    int right = -1;
    int parent = -1;

    Record() {}
    Record(int codigo, int ciclo) {
        this->codigo = codigo;
        this->ciclo = ciclo;
    }

    void print() {
        std::cout << "Codigo: " << this->codigo << std::endl;
        std::cout << "Ciclo: " << this->ciclo << std::endl;
        std::cout << "Left: " << this->left << std::endl;
        std::cout << "Right: " << this->right << std::endl;
        std::cout << "Parent: " << this->parent << std::endl;
    }
};

class AVLFile{
    std::string filename;

    void createFile(){
        std::ofstream file(this->filename, std::ios::app | std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");
        file.close();
    }

    bool fileExists(){
        std::ifstream file(this->filename);
        bool exists = file.good();
        file.close();
        return exists;
    }

public:
    AVLFile(std::string filename){
        this->filename = filename;
        if (!(this->fileExists())) createFile();
    }

    void insert(Record record) {
        std::fstream file(this->filename, std::ios::binary | std::ios::in | std::ios::out);

        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");

        if (this->size() == 0) {
            file.write((char*) &record, sizeof(record));
        } else {
            Record current;
            int current_position;

            file.seekg(0, std::ios::end);
            int last_position = file.tellg() / sizeof(Record);
            file.seekg(0, std::ios::beg);

            while (true) {
                
                current_position = file.tellg() / sizeof(Record);
                file.read((char*) &current, sizeof(Record));

                if (current.codigo > record.codigo) {
                    if (current.left == -1) {
                        current.left = last_position;
                        break;
                    } else {
                        file.seekg(current.left * sizeof(Record), std::ios::beg);
                    }
                } else {
                    if (current.right == -1) {
                        current.right = last_position;
                        break;
                    } else {
                        file.seekg(current.right * sizeof(Record), std::ios::beg);
                    }
                }
            }

            file.seekg(current_position * sizeof(Record));
            file.write((char*) &current, sizeof(Record));

            record.parent = current_position;
            file.seekg(0, std::ios::end);
            file.write((char*) &record, sizeof(Record));
        }
        file.close();
    }

    template<typename KEY_TYPE = int>
    Record find(KEY_TYPE codigo){
        std::fstream file(this->filename, std::ios::binary | std::ios::in | std::ios::out);

        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");

        Record record;
        if (this->size() != 0) {
            while (true) {
                file.read((char*) &record, sizeof(record));
                if (record.codigo == codigo) {
                    break;
                }
                else if (record.codigo > codigo) {
                    if (record.left == -1) {
                        break;
                    } else {
                        file.seekg(record.left * sizeof(Record), std::ios::beg);
                    }
                } else {
                    if (record.right == -1) {
                        break;
                    } else {
                        file.seekg(record.right * sizeof(Record), std::ios::beg);
                    }
                }
            }
        }
        file.close();
        return record;
    }

    template<typename KEY_TYPE = int>
    void rangeSortedRecursiveSearch(std::fstream& file, std::vector<Record>& records, KEY_TYPE inicio, KEY_TYPE final, int position) {
        Record record;

        if (position != -1) {
            file.seekg(position * sizeof(Record), std::ios::beg);
            file.read((char*) &record, sizeof(record));
            if (record.codigo >= inicio && record.codigo <= final) {
                rangeSortedRecursiveSearch<KEY_TYPE>(file, records, inicio, final, record.left);
                records.push_back(record);
                rangeSortedRecursiveSearch<KEY_TYPE>(file, records, inicio, final, record.right);
            } else if (record.codigo < inicio) {
                rangeSortedRecursiveSearch<KEY_TYPE>(file, records, inicio, final, record.right);
            } else {
                rangeSortedRecursiveSearch<KEY_TYPE>(file, records, inicio, final, record.left);
            }
        }  

    }

    template<typename KEY_TYPE = int>
    std::vector<Record> range(KEY_TYPE inicio, KEY_TYPE final){
        std::fstream file(this->filename, std::ios::binary | std::ios::in | std::ios::out);

        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");

        std::vector<Record> records;
        Record record;

        if (this->size() != 0) {
            rangeSortedRecursiveSearch<KEY_TYPE>(file, records, inicio, final, 0);
        }
        return records;
    }

    void recursive_inorder(std::fstream& file, std::vector<Record>& records, int position) {
        Record record;

        if (position != -1) {
            file.seekg(position * sizeof(Record), std::ios::beg);
            file.read((char*) &record, sizeof(record));
            recursive_inorder(file, records, record.left);
            records.push_back(record);
            recursive_inorder(file, records, record.right);
        }

    }

    std::vector<Record> linearRead() {
        std::fstream file(this->filename, std::ios::binary | std::ios::in | std::ios::out);
        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");

        std::vector<Record> records;

        if (this->size() != 0) {
            Record record;
            while (file.peek() != EOF) {
                file.read((char*) &record, sizeof(Record));
                records.push_back(record);
            }
        }
        file.close();
        return records;
    }

    std::vector<Record> inorder() {
        std::fstream file(this->filename, std::ios::binary | std::ios::in | std::ios::out);
        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");

        std::vector<Record> records;

        if (this->size() != 0) {
            recursive_inorder(file, records, 0);
        }
        file.close();
        return records;
    }


    int size(){
        std::ifstream file(this->filename, std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("No se pudo abrir el archivo");

        file.seekg(0, std::ios::end);
        long total_bytes = file.tellg();
        file.close();
        return total_bytes / sizeof(Record);
    }
};