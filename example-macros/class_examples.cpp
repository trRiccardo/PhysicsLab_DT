#include <iostream>
#include <string>

//Parent class
class Polygon{
	public:
		Polygon(){};
		~Polygon(){};
		
		void SetNVertex(int n){n_vertex=n;};
		int GetNVertex(){return n_vertex;};
		void SetName(std::string str){name=str;}
		std::string GetName(){return name;}
	private:
		int n_vertex;
		double area;
		std::string name;
};

//Derived class n1
class Triangle: public Polygon{
	public:
		//Two types of constructor (overloading of a function)
		Triangle(){};
		Triangle(double b, double h){base=b; height=h;};

		double ComputeArea(){return base*height/2.;};
		void SetBaseHeight(double b, double h){base=b; height=h;};

	private:
		double base, height;
};

//Derived class n2
class Square: public Polygon{
	public:
		//Two types of constructor (overloading of a function)
		Square(){};
		Square(double h){height=h;};

		double ComputeArea(){return height*height;};

	private:
		double height;
};


int main(){

	//Defining two squares
	Square my_square();
	Square my_other_square(5.4);

	//Defining two triangles (with pointers)
	Triangle *my_triangle=new Triangle();
	Triangle *my_second_triangle=new Triangle(1.5, 2.3);

	my_second_triangle->SetName("the_second_triangle");
	my_second_triangle->SetNVertex(3);

	std::cout<< "The object: "<< my_second_triangle->GetName() << " has area : " << my_second_triangle->ComputeArea() 
		<< " and " << my_second_triangle->GetNVertex() <<" vertices"<<std::endl;

	return 0;
}
