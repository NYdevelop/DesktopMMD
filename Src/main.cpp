#include "MMD/ManageMMD.h"
#include "Util\ConfigLoader.h"
using namespace std;

int main()
{
    // TODO: êQÇÈÅAç¿ÇÈ
    {
        CConfigLoader config("config.txt");
        config.SetRemoveDoubleQuatation(true);

        ManageMMD manager;
        manager.Initialize(config.Load("ANIM_PATH"), config.Load("MODEL_PATH"));
        manager.Process();
        manager.Exit();
    }

    _CrtDumpMemoryLeaks();

    return 0;
}