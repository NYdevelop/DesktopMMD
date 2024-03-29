#pragma once
#include "MMD/DrawMMD.h"
#include "State/WalkStateManager.h"

#include "DxLib.h"

// y位置は固定
// x,z平面内で移動
void WalkStart(VECTOR screenPos, DrawMMD* mmd, WalkStateManager* walker);

float GetDistance(const VECTOR& v1, const VECTOR& v2);

void ViewCamera(VECTOR chara2Camera, MATRIX world2Local, MATRIX defLocalRot, int model, int boneHead);

void UpdateDirect(int modelHandle, int boneIndex, MATRIX targetLocalRot);

// 衝突していればTrue
bool IsHitPlane(VECTOR v1, VECTOR v2, VECTOR planePoint, VECTOR planeNorm);