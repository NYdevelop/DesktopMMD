#include "EstimateBPM.h"

float EstimateBPM::Estimate(std::vector<int> val)
{
    if (previousValue == 0)
    {
        previousValue = val[0];

        if (val[1] != -1)
            previousValue = val[0] < val[1] ? val[0] : val[1];

        return previousValue;
    }

    auto add = val[0];
    if (val[1] != -1)
    {
        double dist1 = abs(previousValue - val[0]);
        double dist2 = abs(previousValue - val[1]);

        if (dist1 > dist2)
        {
            add = val[1];
        }
    }
    previousValue = (previousValue + add) / 2.0f;
    return previousValue;
}
