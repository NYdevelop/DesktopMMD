#include "MMD/ManageMMD.h"

using namespace std;


bool IsLarge(RECT target, RECT src)
{
    auto x = (target.left <= src.left) && (target.right >= src.right);
    auto y = (target.top <= src.top) && (target.bottom >= src.bottom);
    return x & y;
}

int main()
{
    // TODO: êQÇÈÅAç¿ÇÈ
    {
        ManageMMD manager;
        manager.Initialize();
        manager.Process();
        manager.Exit();
    }

    _CrtDumpMemoryLeaks();

    return 0;
}