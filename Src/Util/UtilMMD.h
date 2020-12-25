#pragma once
#include "MMD/DrawMMD.h"
#include "State/WalkStateManager.h"

#include "DxLib.h"

// yà íuÇÕå≈íË
// x,zïΩñ ì‡Ç≈à⁄ìÆ
void WalkStart(VECTOR screenPos, DrawMMD* mmd, WalkStateManager* walker);

float GetDistance(const VECTOR& v1, const VECTOR& v2);

void ViewCamera(VECTOR chara2Camera, MATRIX world2Local, MATRIX defLocalRot, int model, int boneHead);

void UpdateDirect(int modelHandle, int boneIndex, MATRIX targetLocalRot);

// è’ìÀÇµÇƒÇ¢ÇÍÇŒTrue
bool IsHitPlane(VECTOR v1, VECTOR v2, VECTOR planePoint, VECTOR planeNorm);