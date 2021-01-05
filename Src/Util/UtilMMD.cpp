#include "UtilMMD.h"

void WalkStart(VECTOR screenPos, DrawMMD * mmd, WalkStateManager * walker)
{
    auto tmpScreenPos = screenPos;
    tmpScreenPos.z = 0.0f;
    auto Start3DPos = ConvScreenPosToWorldPos(tmpScreenPos);

    tmpScreenPos.z = 1.0f;
    auto End3DPos = ConvScreenPosToWorldPos(tmpScreenPos);

    auto rayVec = VNorm(VSub(End3DPos, Start3DPos));
    auto pos = mmd->GetCharactorPos();

    // �����x�N�g���ƃL����y���ʂƂ������ɋ߂����ǂ���
    if (abs(rayVec.y) < 0.5)
    {
        VECTOR newPos;
        if (screenPos.z == -1)
        {
            // �w���ʍ��W��ray���܂ޖʁA�ƁA
            // �L�������W�����ʉ������ɐL�΂��������̌�_�Ɉړ�
            // ��ʉ����������W���Z�o
            tmpScreenPos.x += 640.0f;
            tmpScreenPos.z = 0.0f;
            auto tmp3DPos = ConvScreenPosToWorldPos(tmpScreenPos);
            auto ScreenSlideVec = VNorm(VSub(tmp3DPos, Start3DPos));

            // �w���ʍ��W���܂ޖʌW���Z�o
            auto norm = VCross(rayVec, VGet(0.f, 1.f, 0.f));

            auto t = VDot(norm, VSub(pos, Start3DPos));
            newPos = VAdd(pos, VScale(ScreenSlideVec, t));
        }
        else
        {
            newPos = ConvScreenPosToWorldPos(screenPos);
        }
        walker->Start(newPos);
        return;
    }

    // ray�����ƃL����y���ʂƂ̌�_�Ɉړ�
    auto t = (pos.y - Start3DPos.y) / rayVec.y;
    auto newPos = VGet(Start3DPos.x + t * rayVec.x, pos.y, Start3DPos.z + t * rayVec.z);
    walker->Start(newPos);
}

float GetDistance(const VECTOR& v1, const VECTOR& v2)
{
    auto sub = VSub(v1, v2);
    return sqrt(VDot(sub, sub));
}

void ViewCamera(VECTOR chara2Camera, MATRIX world2Local, MATRIX defLocalRot, int model, int boneHead)
{
    auto cameraLocal = VTransformSR(chara2Camera, world2Local);
    if (abs(atan2(cameraLocal.z, cameraLocal.y) - DX_PI_F / 2) < DX_PI_F / 4 &&
        abs(atan2(cameraLocal.x, cameraLocal.z)) < DX_PI_F / 4)
    {
        auto rot = MGetRotVec2(VGet(0, 0, 1), cameraLocal);
        rot.m[3][0] += defLocalRot.m[3][0];
        rot.m[3][1] += defLocalRot.m[3][1];
        rot.m[3][2] += defLocalRot.m[3][2];
        // DxLib::MV1SetFrameUserLocalMatrix(model, boneHead, rot);
        UpdateDirect(model, boneHead, rot);
        return;
    }

    UpdateDirect(model, boneHead, defLocalRot);
}

const VECTOR Ident = VGet(0.f, 0.f, 1.f);
void UpdateDirect(int modelHandle, int boneIndex, MATRIX targetLocalRot)
{
    /// ��]���Z�o
    auto currentRot = MV1GetFrameLocalMatrix(modelHandle, boneIndex);
    auto norm = VCross(VTransformSR(Ident, currentRot), VTransformSR(Ident, targetLocalRot));
    if (VDot(norm, norm) < 0.01f)
    {
        return;
    }

    /// �����̍X�V
    auto updateRot = MMult(MGetRotAxis(norm, 1.f * DX_PI_F / 180.f), currentRot);
    updateRot = MGetRotVec2(VGet(0, 0, 1), VTransformSR(Ident, updateRot));
    updateRot.m[3][0] = targetLocalRot.m[3][0];
    updateRot.m[3][1] = targetLocalRot.m[3][1];
    updateRot.m[3][2] = targetLocalRot.m[3][2];
    DxLib::MV1SetFrameUserLocalMatrix(modelHandle, boneIndex, updateRot);
}


// p0�𕽖ʏ�̓_�Ƃ��āA
// ((v1 - p0)�En)* ((v2 - p0)�En) <= 0�Ȃ�ΐ����͕��ʂƏՓ�
// http://marupeke296.com/COL_3D_No3_LineToPlane.html
bool IsHitPlane(VECTOR v1, VECTOR v2, VECTOR planePoint, VECTOR planeNorm)
{
    if (VDot(VSub(v1, planePoint), planeNorm) * VDot(VSub(v2, planePoint), planeNorm) > 0)
    {
        return false;
    }
    return true;
}
