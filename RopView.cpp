#include "RopView.h"

using namespace std;

int main()
{
	omcn::OmConnect omConnect;
	cout << "****************************************************\n\t WELCOME" << endl;
	if (omConnect.omConnectDevice())
	{
		cout << "Connected & Configured" << endl;
	}
	return 0;
}
