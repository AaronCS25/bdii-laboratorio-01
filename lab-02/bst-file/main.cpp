#include "avl.h"

int main(){
    AVLFile avl("hola.bin");

    Record record1(1, 1);
    Record record2(2, 2);
    Record record3(3, 3);
    Record record4(4, 4);
    Record record5(5, 5);
    Record record6(6, 6);
    Record record7(7, 7);

    avl.insert(record4);
    avl.insert(record2);
    avl.insert(record6);
    avl.insert(record1);
    
    avl.insert(record3);
    avl.insert(record5);
    avl.insert(record7); 
    
 
    std::vector<Record> records = avl.linearRead();
    std::cout<<"Numero: "<<records.size()<<std::endl;

    for (int i = 0; i < records.size(); i++) {
        std::cout << records[i].codigo << std::endl;
    } 
    std::cout<<std::endl;


    std::cout<<avl.size()<<std::endl;

    avl.find<int>(4).print();std::cout<<std::endl;
    avl.find<int>(2).print();std::cout<<std::endl;
    avl.find<int>(6).print();std::cout<<std::endl;
    avl.find<int>(1).print();std::cout<<std::endl;
    avl.find<int>(3).print();std::cout<<std::endl;
    avl.find<int>(5).print();std::cout<<std::endl;
    avl.find<int>(7).print();std::cout<<std::endl;
     
    
    records = avl.inorder();

    for (int i = 0; i < records.size(); i++) {
        std::cout << records[i].codigo << std::endl;
    } 
    
    std::cout<<std::endl;
    records = avl.range<int>(2, 5);

    for (int i = 0; i < records.size(); i++) {
        std::cout << records[i].codigo << std::endl;
    }

    return 0;
}