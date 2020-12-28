#include "MMD/ManageMMD.h"
#include "Util\ConfigLoader.h"
using namespace std;

int main()
{
    // TODO: êQÇÈÅAç¿ÇÈ
    {
        CConfigLoader config("config.txt");
        config.SetRemoveDoubleQuatation(true);

        CConfigLoader configPos("config_pos.txt");

        ManageMMD manager;

        if (configPos.IsOpen())
        {
            manager.Initialize(config.Load("ANIM_PATH"), config.Load("MODEL_PATH"),
                std::stof(configPos.Load("CHARA_POS_X")), std::stof(configPos.Load("CHARA_POS_Y")), std::stof(configPos.Load("CHARA_POS_Z")),
                std::stof(configPos.Load("CHARA_DIRECT")),
                std::stof(configPos.Load("CAMERA_POS_X")), std::stof(configPos.Load("CAMERA_POS_Y")), std::stof(configPos.Load("CAMERA_POS_Z")));
        }
        else
        {
            manager.Initialize(config.Load("ANIM_PATH"), config.Load("MODEL_PATH"));
        }
        manager.Process();
        manager.Exit();
    }

    _CrtDumpMemoryLeaks();

    return 0;
}