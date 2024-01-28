#include "app.hpp"

//std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	App app{};
	
	try
	{
		app.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << std::endl << "  ERROR :" << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}