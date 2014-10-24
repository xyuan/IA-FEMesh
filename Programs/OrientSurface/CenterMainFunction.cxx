#include "CenterMeshGUI.h"



int main(int argc, char **argv)
{
	
	CenterMeshGUI  *a=new CenterMeshGUI;
	//= ICPRegistrationGUI::New();
	a->show( argc, argv);
	Fl::run();
	return 0;
}
