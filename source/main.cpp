//!< Common includes
#include <iostream>

//!< User include
#include "VehicleTracking.h"

//!< Program main
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		readme(argv[0]);
		return -1;
	} 
	else
	{
		return project_main(argc, argv);
	}
	return 0;
}

