#include <iostream>
#include <fstream>
//test output file with: od -a  
//test output file with: od -tx1 
int main(){
	std::string ch2="somebytes\n";
	std::ofstream file;

	file.open("test", std::ios::binary);
	file.write(ch2.c_str() ,ch2.length());

	ch2="someotherbytes\n";
	file.write(ch2.c_str() ,ch2.length());

	file.close();
	return 0;
}

