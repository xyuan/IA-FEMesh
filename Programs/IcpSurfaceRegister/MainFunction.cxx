#include "ICPRegistrationGUI.h"



int main(int argc, char **argv)
{
	
	ICPRegistrationGUI  *a=new ICPRegistrationGUI;
	//= ICPRegistrationGUI::New();
	a->show( argc, argv);
	Fl::run();
	return 0;
}
