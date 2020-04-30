#pragma once

class TransrateAnim
{
private:
    int modelHandle;
    int srcAnimIndex;
    int transrateAnimIndex;

    int transrateTime = 0;
    int progressTime = 0;

public:
    void SetAnim(int srcAnim, int transAnim, int modHandle);

    void SetTransTime(int time);

    void Transrate();
};