#include <iostream>
//Try to understand why the program is returnig a seg. fault
//How would you correct it? Something is missing
struct person{
	double height;
	int age;
	std::string name;
};

int main(){
	//Var. decl.
	person Albert;
	//Access to members
	Albert.height=1.85;
	Albert.age=22;
	Albert.name="Albert Smith";


	std::cout << "Albert struct:\n";
	std::cout << "Height: " << Albert.height <<std::endl;
	std::cout << "Age: " << Albert.height <<std::endl;
	std::cout << "Mame: " << Albert.name <<std::endl;

	//
	person *Lucia;
	Lucia->height=1.72;
	Lucia->age=45;
	Lucia->name="Lucia Rossi";

	std::cout << "Lucia struct:\n";
	std::cout << "Height: " << Lucia->height <<std::endl;
	std::cout << "Age: " << Lucia->height <<std::endl;
	std::cout << "Mame: " << Lucia->name <<std::endl;
	return 0;
}

