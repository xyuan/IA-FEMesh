
//#include "config.h"
#include <FL/Fl.H>
#include "IaFeMesh.h"

int main() 
{
	//Initial global objects.
	IaFeMesh *MainMenuObj = new IaFeMesh;
    Fl::visual(FL_DOUBLE|FL_INDEX);
	MainMenuObj->Show();
    return Fl::run();
	// delete all initial allocations
	delete MainMenuObj;
}


