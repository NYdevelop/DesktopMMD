#include "MMD/ManageMMD.h"

using namespace std;

int main()
{
    // TODO: �Q��A����
    {
        ManageMMD manager;
        manager.Initialize();
        manager.Process();
        manager.Exit();
    }

    _CrtDumpMemoryLeaks();

    return 0;
}