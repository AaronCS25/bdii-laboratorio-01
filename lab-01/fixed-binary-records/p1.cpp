#include "fstream"
#include "iostream"
#include "vector"

struct Alumno {
    char codigo [5];
    char nombre [11];
    char apellidos [20];
    char carrera [15];
    int ciclo;
    float mensualidad;

    int nextDel = -2;

    void setData(){
        std::cout<<"Codigo:";std::cin>>codigo;
        std::cout<<"Nombre:";std::cin>>nombre;
        std::cout<<"Apellidos:";std::cin>>apellidos;
        std::cout<<"Carrera:";std::cin>>carrera;
        std::cout<<"Ciclo:";std::cin>>ciclo;
        std::cout<<"Mensualidad:";std::cin>>mensualidad;
    }    
    void showData(){
        std::cout<<codigo<<" - "<<nombre<<" - "<<apellidos<<" - "<<carrera<<" - "<<ciclo<<" - "<<mensualidad<<std::endl;
    }
};

int header = -1;

class FixedRecordFile
{
private:
    std::string file_name;

    void createFile() {
        std::ofstream file(this->file_name, std::ios::binary | std::ios::app);
        if(!file.is_open()) throw std::runtime_error("No se pudo crear el archivo");
        file.write((char*) &header, sizeof(header));
        file.close();
    }

    bool fileExists() {
        std::ifstream file(this->file_name);
        bool exists = file.good();
        file.close();
        return exists;
    }
public:

    FixedRecordFile(std::string file_name){
        this->file_name = file_name;      
        if ( !(this->fileExists()) ) {
            this->createFile();
        }  
    } 

    void add(Alumno record){
        std::fstream file(this->file_name, std::ios::binary | std::ios::in | std::ios::out);

        if(!file.is_open()) throw std::runtime_error("No se pudo abrir el archivo");
        file.read((char*) &header, sizeof(header));
        if (header == -1) {
            file.seekp(0, std::ios::end);
            file.write((char*) &record, sizeof(record));//guardar en formato binario
        } else {
            file.seekp(sizeof(header) + header * sizeof(record), std::ios::beg);
            Alumno next;
            file.read((char*) &next, sizeof(record));
            file.seekp(sizeof(header) + header * sizeof(record), std::ios::beg);
            file.write((char*) &record, sizeof(record));
            header = next.nextDel;
            file.seekp(0, std::ios::beg);
            file.write((char*) &header, sizeof(header));
        }
        file.close();
    }  

    bool deleteRecord(int pos){
        std::fstream file(this->file_name, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open()) throw std::runtime_error("No se pudo abrir el archivo");

        if (pos < 0 || pos >= this->size()) return false;

        file.read((char*) &header, sizeof(header));
        Alumno record;

        file.seekp(sizeof(header) + pos * sizeof(Alumno), std::ios::beg);
        file.read((char*) &record, sizeof(Alumno));
        record.nextDel = header;
        file.seekp(sizeof(header) + pos * sizeof(Alumno), std::ios::beg);
        file.write((char*) &record, sizeof(Alumno));
        header = pos;
        file.seekp(0, std::ios::beg);
        file.write((char*) &header, sizeof(header));

        file.close();
        return true;
    }

    std::vector<Alumno> load(){
        std::ifstream file(this->file_name, std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("No se pudo abrir el archivo");
        
        std::vector<Alumno> alumnos;
        Alumno record; 

        file.read((char*) &header, sizeof(header));

        while(file.peek() != EOF){
            record = Alumno();               
            file.read((char*) &record, sizeof(Alumno));
            if (record.nextDel == -2) alumnos.push_back(record);
        }
        file.close();
        return alumnos;
    } 

    Alumno readRecord(int pos){
        std::ifstream file(this->file_name, std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("No se pudo abrir el archivo");

        if (pos < 0 || pos >= this->size()) throw std::runtime_error("Posicion invalida");

        Alumno record;
        file.seekg(sizeof(header) + pos * sizeof(Alumno), std::ios::beg);//fixed length record
        file.read((char*) &record, sizeof(Alumno));
        file.close();

        if (record.nextDel != -2) throw std::runtime_error("Registro eliminado");
        return record;
    }

    int size(){
        std::ifstream file(this->file_name, std::ios::binary);
        if(!file.is_open()) throw std::runtime_error("No se pudo abrir el archivo");
        
        file.seekg(0, std::ios::end);//ubicar cursos al final del archivo
        long total_bytes = file.tellg();//cantidad de bytes del archivo        
        file.close();
        return ( total_bytes - sizeof(header) )/ sizeof(Alumno);
    }
};

int main() {
    //Escritura
    FixedRecordFile file1("data.bin");
    Alumno record;
    Alumno record2;
    std::cout<<"Alumno 1"<<std::endl;
    record.setData();
    std::cout<<std::endl;
    std::cout<<"Alumno 2"<<std::endl;
    record2.setData();
    file1.add(record);
    file1.add(record2);

    //Lectura
    std::cout<<std::endl<<"Lista alumnos"<<std::endl;
    FixedRecordFile file2("data.bin");
    std::vector<Alumno> alumnos = file2.load();
    for(Alumno r : alumnos){
        r.showData();
    }
    std::cout<<std::endl;
    
    std::cout<<"Alumno indice 0:"<<std::endl;
    file2.readRecord(0).showData();
    file2.deleteRecord(0);
    std::cout<<"Alumno de indice 0 eliminado"<<std::endl<<std::endl;

    std::cout<<"Lista alumnos"<<std::endl;
    alumnos = file2.load();
    for(Alumno r : alumnos){
        r.showData();
    }
    std::cout<<std::endl;

    std::cout<<"Alumno indice 1:"<<std::endl;
    file2.readRecord(1).showData();
    file2.deleteRecord(1);
    std::cout<<"Alumno de indice 1 eliminado"<<std::endl<<std::endl;

    std::cout<<"Lista alumnos"<<std::endl;
    alumnos = file2.load();
    for(Alumno r : alumnos){
        r.showData();
    }

    std::cout<<std::endl;
}