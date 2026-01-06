#include "Ankka/Ankka.h"

using namespace std;

int main(int argc, char *argv[])
{	
	std::unique_ptr<Window> w = std::make_unique<Window>();

	if (!w->init(640, 480, "Test Window"))
	{
		Logger::log(1, "$s error: Window init error\n",
			__FUNCTION__);
		return -1;
	}

	w->mainLoop(); // hazaah its working!

	return 0;
}
