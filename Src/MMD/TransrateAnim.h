#pragma once

class TransrateAnim
{
private:
    int modelHandle = 0;
    int srcAnimIndex = 0;
    int transrateAnimIndex = 0;

    int transrateTime = 0;
    int progressTime = 0;

public:
    void SetAnim(int srcAnim, int transAnim, int modHandle);

    void SetTransTime(int time);

    void Transrate();
};