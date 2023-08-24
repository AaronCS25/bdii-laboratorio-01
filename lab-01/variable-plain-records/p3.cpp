#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

struct Matricula{
    std::string codigo;
    int ciclo;
    float mensualidad;
    std::string observaciones;

    int size_bytes() {
        int sz_fields = this->codigo.size() + sizeof(this->ciclo) + sizeof(this->mensualidad) + this->observaciones.size();
        int sz_prefix_fields =  2 * sizeof(int); 
        return sz_fields + sz_prefix_fields;
    }

    void desempaquetar(char* buffer, int sz_reg){

        // Codigo
        int sz_codigo;
        std::memcpy(&sz_codigo, buffer, sizeof(sz_codigo));

        buffer += sizeof(sz_codigo);

        char *buffer_codigo = new char[sz_codigo];
        std::memcpy(buffer_codigo, buffer, sz_codigo);
        buffer += sz_codigo;

        this->codigo = std::string(buffer_codigo, sz_codigo);
        delete[] buffer_codigo;

        // Ciclo
        memcpy(&this->ciclo, buffer, sizeof(this->ciclo));
        buffer += sizeof(this->ciclo);

        // Mensualidad
        memcpy(&this->mensualidad, buffer, sizeof(this->mensualidad));
        buffer += sizeof(this->mensualidad);

        // Observaciones
        int sz_observaciones;
        std::memcpy(&sz_observaciones, buffer, sizeof(sz_observaciones));

        buffer += sizeof(sz_observaciones);

        char *buffer_observaciones = new char[sz_observaciones];
        std::memcpy(buffer_observaciones, buffer, sz_observaciones);
        buffer += sz_observaciones;

        this->observaciones = std::string(buffer_observaciones, sz_observaciones);
        delete[] buffer_observaciones;
    }

    char *empaquetar() {

        char *buffer = new char[this->size_bytes()];
        char *start_buffer = buffer;

        int sz_codigo = this->codigo.size();

        std::memcpy(buffer, &sz_codigo, sizeof(int));
        buffer += sizeof(int);
        std::memcpy(buffer, this->codigo.c_str(), sz_codigo);
        buffer += sz_codigo;

        std::memcpy(buffer, &this->ciclo, sizeof(int));
        buffer += sizeof(int);

        std::memcpy(buffer, &this->mensualidad, sizeof(float));
        buffer += sizeof(float);

        int sz_observaciones = this->observaciones.size();

        std::memcpy(buffer, &sz_observaciones, sizeof(int));
        buffer += sizeof(int);
        std::memcpy(buffer, this->observaciones.c_str(), sz_observaciones);
        buffer += sz_observaciones;

        return start_buffer;
    }
};

std::ostream& operator<<(std::ostream& stream, const Matricula& matricula){
    stream<<matricula.codigo<<" - "<<matricula.ciclo<<" - "<<matricula.mensualidad<<" - "<<matricula.observaciones;
    return stream;
}

struct MetadataRecord{
    int pos;
    int size;
};


class VariableRecord{
    std::string filename;
    std::string metadata_filename;

    void createFile() {
        std::ofstream file(this->filename, std::ios::binary | std::ios::app);
        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");
        file.close();

        std::ofstream metadata(this->metadata_filename, std::ios::binary | std::ios::app);
        if(!metadata.is_open()) throw std::runtime_error("No se pudo crear el archivo");
        metadata.close();
    }

    bool fileExists() {
        std::ifstream file(this->filename);
        bool exists = file.good();
        file.close();
        return exists;
    }

public:
    VariableRecord(const std::string& filename, const std::string& metadata_filename): filename(filename), metadata_filename(metadata_filename){
        if ( !(this->fileExists()) ) {
            this->createFile();
        }
    }

    std::vector<Matricula> load() {
        std::vector<Matricula> records;

        std::fstream metadata(this->metadata_filename, std::ios::in | std::ios::out | std::ios::binary);
        if(!metadata.is_open()) throw std::runtime_error("No se pudo abrir el archivo de metadatos");

        std::fstream file(this->filename, std::ios::in | std::ios::out | std::ios::binary);
        if(!file.is_open()) {metadata.close(); throw std::runtime_error("No se pudo abrir el archivo de datos");}

        MetadataRecord metadata_record;
        Matricula record;

        while(metadata.peek() != EOF) {
            metadata.read((char *) &metadata_record, sizeof(MetadataRecord));
            char* buffer = new char[metadata_record.size];

            file.seekg(metadata_record.pos, std::ios::beg);
            file.read((char *) buffer, metadata_record.size);

            record.desempaquetar(buffer, metadata_record.size);
            records.push_back(record);

            delete[] buffer;
        }
        metadata.close();
        file.close();

        return records;
    };

    void add(Matricula& record){

        std::fstream metadata(this->metadata_filename, std::ios::app | std::ios::binary);
        if(!metadata.is_open()) throw std::runtime_error("No se pudo abrir el archivo de metadatos");
        
        std::fstream file(this->filename, std::ios::app | std::ios::binary);
        if(!file.is_open()) {metadata.close(); throw std::runtime_error("No se pudo abrir el archivo de datos");}

        MetadataRecord metadata_record;
        metadata_record.pos = file.tellp();
        metadata_record.size = record.size_bytes();
        
        metadata.write((char*) &metadata_record, sizeof(MetadataRecord));

        char* buffer = record.empaquetar();

        file.write((char*)buffer, metadata_record.size);

        delete[] buffer;

        metadata.close();
        file.close();
        
    };

    Matricula readRecord(int pos){
        std::fstream metadata(this->metadata_filename, std::ios::in | std::ios::out | std::ios::binary);
        if(!metadata.is_open()) throw std::runtime_error("No se pudo abrir el archivo de metadatos");

        std::fstream file(this->filename, std::ios::in | std::ios::out | std::ios::binary);
        if(!file.is_open()) {metadata.close(); throw std::runtime_error("No se pudo abrir el archivo de datos");}

        MetadataRecord metadata_record;
        Matricula record;

        if (pos < 0 || pos >= this->size()) throw std::runtime_error("Posicion invalida");

        metadata.seekg(pos * sizeof(MetadataRecord), std::ios::beg);
        metadata.read((char *) &metadata_record, sizeof(MetadataRecord));

        char* buffer = new char[metadata_record.size];

        file.seekg(metadata_record.pos, std::ios::beg);
        file.read((char *) buffer, metadata_record.size);

        record.desempaquetar(buffer, metadata_record.size);

        delete[] buffer;
        
        file.close();
        metadata.close();
        
        return record;
    };

    int size(){
        std::fstream metadata(this->metadata_filename, std::ios::in | std::ios::out | std::ios::binary);
        if(!metadata.is_open()) throw std::runtime_error("No se pudo abrir el archivo");
        
        metadata.seekg(0, std::ios::end);
        long total_bytes = metadata.tellg();
        metadata.close();
        return ( total_bytes ) / sizeof(MetadataRecord);
    }
};

void testWrite(VariableRecord vr){
    std::cout<<"----WRITE----"<<std::endl;
    int sz;
    std::cout<<"Ingrese el numero de elementos a agregar: ";
    std::cin>>sz;
    std::cin.ignore();
    while(sz-- > 0){
        Matricula matricula;

        std::cout<<"Ingrese codigo:"; std::getline(std::cin, matricula.codigo);

        std::cout<<"Ingrese ciclo:"; std::cin>>matricula.ciclo;

        std::cout<<"Ingrese mensualidad:"; std::cin>>matricula.mensualidad;
        std::cin.ignore();
        std::cout<<"Ingrese observaciones:"; std::getline(std::cin, matricula.observaciones);

        vr.add(matricula);

        std::cout<<std::endl;
    }
}

void testRead(VariableRecord vr)
{
    std::cout<<"----READ----"<<std::endl;
    auto records = vr.load();
    auto sz_records = records.size();

    std::cout<<"Cantidad de registros: "<<sz_records<<std::endl;
    if (sz_records == 0) {return;}
    std::cout<<"---------------"<<std::endl;
    for (auto &record : records)
    {
        std::cout << record << std::endl;
    }
    std::cout<<"---------------"<<std::endl;
    int pos_record;
    while (true) {
        std::cout<<"Seleccione indice de registro: "; std::cin>>pos_record;
        if (pos_record >= 0 && pos_record < records.size()){break;}
    }
    Matricula record = vr.readRecord(pos_record);
    std::cout << record << std::endl;
}

int main()
{
    VariableRecord vr("datos_matricula.bin", "cabecera.bin");
    testWrite(vr);
    testRead(vr);
    return 0;
}