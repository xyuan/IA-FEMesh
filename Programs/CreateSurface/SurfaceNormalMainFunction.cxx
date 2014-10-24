#include "GenerateSurfaceNormalsGui.h"



int main(int argc, char **argv)
{
	
	SurfaceNormalGui  *a=new SurfaceNormalGui;
	//= ICPRegistrationGUI::New();
	a->show( argc, argv);
	Fl::run();
	return 0;
}
