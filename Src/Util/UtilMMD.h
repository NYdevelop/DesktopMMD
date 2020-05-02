#pragma once
#include "MMD/DrawMMD.h"
#include "State/WalkStateManager.h"

#include "DxLib.h"

// y�ʒu�͌Œ�
// x,z���ʓ��ňړ�
void WalkStart(float screenX, float screenY, DrawMMD* mmd, WalkStateManager* walker);

float GetDistance(const VECTOR& v1, const VECTOR& v2);