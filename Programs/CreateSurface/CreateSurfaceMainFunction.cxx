#include "CreateSurfaceGui.h"



int main(int argc, char **argv)
{
	
	CreateSurfaceGui  *a=new CreateSurfaceGui;
	//= ICPRegistrationGUI::New();
	a->show( argc, argv);
	Fl::run();
	return 0;
}
