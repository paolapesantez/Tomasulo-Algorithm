// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "TomasuloComponents.h"
#include "TomasuloFunctions.h"


int main()
{
	TomasuloFunctions tom_ROB;
	clock_t t1,t2;    
    
	try
	{
		t1=clock();
		tom_ROB.simulateCycle();
		t2=clock();
	}
	catch (std::runtime_error &e)
	{
		std::cout<<e.what()<<std::endl;
	}
	catch(...)
	{
		std::cout<<"Unknown error Main"<<std::endl;
	}
    
    float diff ((float)t2-(float)t1);
	float seconds = diff / CLOCKS_PER_SEC;
    cout<<seconds<<endl;
	cout << "Tomasulo Simulation has finished."<< endl;
	system("pause");
	return 0;
}