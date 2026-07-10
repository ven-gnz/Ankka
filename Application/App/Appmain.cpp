#include <App/Appmain.h>

int main(int argc, char *argv[])
{	
	std::unique_ptr<Window> w = std::make_unique<Window>();
	if (!w->init(640, 480, "Test Window", false))
	{
		Logger::log(1, "$s error: Window init error\n",
			__FUNCTION__);
		system("pause");
		return -1;
	}

	w->mainLoop(); // hazaah its working!
	w->cleanup();
	return 0;
}
