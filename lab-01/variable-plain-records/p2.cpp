#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct Alumno {
  std::string Nombre;
  std::string Apellidos;
  std::string Carrera;
  float mensualidad;
};

class VariableRecord {
  std::string filename;
  char field_delim = '|';
  char record_delim = '\n';

  void _build_from_line(std::stringstream &lineStream, Alumno &record) {
    std::getline(lineStream, record.Nombre, this->field_delim);
    std::getline(lineStream, record.Apellidos, this->field_delim);
    std::getline(lineStream, record.Carrera, this->field_delim);
    std::string float_temp;
    std::getline(lineStream, float_temp, this->field_delim);
    record.mensualidad = std::stof(float_temp);
  }

  void _read_first_line(std::ifstream &file) {
    std::string tmp;
    getline(file, tmp);
  }

public:
  VariableRecord(std::string filename) : filename(filename) {}

  std::vector<Alumno> load() {
    std::ifstream infile(this->filename, std::ios::app);
    std::vector<Alumno> result;

    if (!infile.is_open()) {
      std::cerr << "Can´t open file";
      exit(EXIT_FAILURE);
    }

    this->_read_first_line(infile);
    Alumno tmp;
    std::string line;
    while (std::getline(infile, line, this->record_delim)) {
      Alumno temp;
      std::stringstream lineStream(line);
      this->_build_from_line(lineStream, temp);
      result.push_back(temp);
    }
    infile.close();

    return result;
  }

  void add(Alumno record) {
    std::ofstream outfile;
    outfile.open(this->filename, std::ios::app);
    if (!outfile.is_open()) {
      std::cerr << "Can´t open file";
      exit(EXIT_FAILURE);
    }

    if (outfile.tellp() == 0) {
      outfile << "Nombre" << this->field_delim << "Apellidos"
              << this->field_delim << "Carrera" << this->field_delim
              << "mensualidad" << this->record_delim;
    }
    outfile << record.Nombre << this->field_delim;
    outfile << record.Apellidos << this->field_delim;
    outfile << record.Carrera << this->field_delim;
    outfile << std::to_string(record.mensualidad) << this->record_delim;
    outfile.close();
  }

  Alumno readRecord(int pos) {
    if (pos < 0)
      throw std::invalid_argument(
          "La posicion enviada debe no debe ser negativa");
    std::ifstream infile(this->filename, std::ios::app);
    if (!infile.is_open()) {
      std::cerr << "Can´t open file";
      exit(EXIT_FAILURE);
    }
    this->_read_first_line(infile);
    Alumno tmp;
    std::string line;
    for (int i = 0; i <= pos; i++) {
      getline(infile, line, this->record_delim);
    }
    infile.close();
    std::stringstream lineStream(line);
    this->_build_from_line(lineStream, tmp);
    return tmp;
  }
};

int main() {
  VariableRecord vr("a.txt");
  Alumno a;
  a.Apellidos = "last name example";
  a.Nombre = "first name example";
  a.Carrera = "example";
  a.mensualidad = 1004;
  vr.add(a);
  std::cout << vr.readRecord(0).Apellidos << std::endl;
  return 0;
}