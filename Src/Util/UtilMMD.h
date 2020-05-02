#pragma once
#include "MMD/DrawMMD.h"
#include "State/WalkStateManager.h"

#include "DxLib.h"

// yˆÊ’u‚ÍŒÅ’è
// x,z•½–Ê“à‚ÅˆÚ“®
void WalkStart(float screenX, float screenY, DrawMMD* mmd, WalkStateManager* walker);

float GetDistance(const VECTOR& v1, const VECTOR& v2);