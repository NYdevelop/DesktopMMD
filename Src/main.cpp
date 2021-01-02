#include "MMD/ManageMMD.h"

using namespace std;

int main()
{
    // TODO: Q‚éAÀ‚é
    {
        ManageMMD manager;
        manager.Initialize();
        manager.Process();
        manager.Exit();
    }

    _CrtDumpMemoryLeaks();

    return 0;
}