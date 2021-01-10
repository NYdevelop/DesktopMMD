#include "MMD/DrawMMD.h"

#include "Util/WinUtil.h"
#include "Util/UtilMMD.h"
#include <fstream>

using namespace std;

DrawMMD::DrawMMD(const std::string & animPath, const std::string & modelPath,
    const float charaX, const float charaY, const float charaZ,
    const float charaDirect,
    const float cameraX, const float cameraY, const float cameraZ)
{
    m_AnimPath = animPath;
    m_ModelPath = modelPath;

    //DxLib_Init�ȑO�ŌĂяo���ׂ�����
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    dispWidth = rc.right - rc.left;
    dispHeight = rc.bottom - rc.top;


    RotateY = charaDirect;
    m_Zoom = cameraZ * -1.f;

    charaPos = VGet(charaX, charaY, charaZ);
    cameraPos = VGet(cameraX, cameraY, m_Zoom * -1.f);

    SetUseDirect3DVersion(DX_DIRECT3D_9EX);
}

void DrawMMD::afterInitialize()
{
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//�w�i��ݒ�(���߂�����)

    DxLib::SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɂ���

    cameraViewOffset = VGet(0.f, 7.f, 1.f);
    cameraViewPos = VGet(0.f, 0.f, 0.f);

    SetupCamera_Perspective(30.f * DX_PI_F / 180.f);
    SetCameraNearFar(1.f, 1000.0f);
    SetCameraPositionAndTarget_UpVecY(cameraPos, cameraViewPos);

    SetFullSceneAntiAliasingMode(4, 4);
    // gHandle = MakeGraph(1920, 1040);

    //ChangeLightTypePoint(
    //    VGet(0.0f, 0.0f, 0.0f),
    //    1000.f,
    //    1.f,
    //    0.f,
    //    0.f);

    // �v���O���}�u���V�F�[�_�[���f���Q�D�O���g�p�ł��Ȃ��ꍇ�̓G���[��\�����ďI��
    if (GetValidShaderVersion() < 300)
    {
        // �G���[�\��
        std::cout << "�v���O���}�u���V�F�[�_�[�Q�D�O���g�p�ł��Ȃ����̂悤�ł�" << std::endl;

        // �L�[���͑҂�
        WaitKey();

        // �c�w���C�u�����̌�n��
        DxLib_End();

        // �\�t�g�I��
        return;
    }

    // ���_�V�F�[�_�[��ǂݍ���
    VertexShaderHandle = LoadVertexShader(L"Src/Shader/SkinMesh4_DirLight_ToonVS.vso");
    std::cout << "VShader handle: " << VertexShaderHandle << std::endl;

    // �s�N�Z���V�F�[�_�[��ǂݍ���
    PixelShaderHandle = LoadPixelShader(L"Src/Shader/SkinMesh4_DirLight_ToonPS.pso");
    std::cout << "PShader handle: " << PixelShaderHandle << std::endl;

    // �g�p���钸�_�V�F�[�_�[���Z�b�g
    SetUseVertexShader(VertexShaderHandle);

    // �g�p����s�N�Z���V�F�[�_�[���Z�b�g
    SetUsePixelShader(PixelShaderHandle);

    MV1SetUseOrigShader(TRUE);
}

//���C���ƂȂ鏈��
int DrawMMD::mainProcess()
{
    if (isDraw == false) return S_FALSE;

    //�`����e��S�폜
    DxLib::ClearDrawScreen();
    DrawBox(
        0, 0, dispWidth, dispHeight,
        GetColor(1, 1, 1), TRUE);//�w�i��ݒ�(���߂�����)

    SetCameraPositionAndTarget_UpVecY(
        cameraPos, VAdd(cameraViewPos, cameraViewOffset));

    auto rayVec = GetRayVec();

    // TODO: �ݒ�t�@�C������̗���ON/OFF
    // �������͗������Ȃ�
    //if (m_StateManager->GetCurrentStateIndex() != EState::STATE_WALK &&
    //    abs(atan2(rayVec.z, rayVec.y) + DX_PI_F / 2) < DX_PI_F / 4)
    //{
    //    charaPos.y = gravity.PosUpdate(charaPos);
    //}

    DxLib::MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));

    if (canViewCamera)
    {
        ViewCamera(
            VSub(MV1GetFramePosition(model, boneHead), rayVec) ,
            MTranspose(MV1GetFrameLocalWorldMatrix(model, boneHead)),
            defHeadLocalRot, model, boneHead);
    }
    else
    {
        MV1ResetFrameUserLocalMatrix(model, boneHead);
    }

    m_StateManager->Doing();

    MV1PhysicsCalculation(model, 1000.0f / 60.0f);

    //���f���̕`��
    DxLib::MV1DrawModel(model);

    //GetDrawScreenGraph(0,0,1920,1040, gHandle);
    //GraphFilter(gHandle, DX_GRAPH_FILTER_GAUSS, 8, 50);
    //DrawGraph(0,0,gHandle, FALSE);

    //�`����e����ʂɔ��f
    DxLib::ScreenFlip();

    return S_OK;
}

void Write(std::ofstream& ofs, const std::string& keyWord, const std::string& value)
{
    ofs << keyWord << " = " << value << std::endl;
};

void DrawMMD::Exit()
{
    isDraw = false;
    MV1DeleteModel(model);

    // �ǂݍ��񂾒��_�V�F�[�_�[�̍폜
    DeleteShader(VertexShaderHandle);

    // �ǂݍ��񂾃s�N�Z���V�F�[�_�[�̍폜
    DeleteShader(PixelShaderHandle);

    std::ofstream ofs("config_pos.txt");
    Write(ofs, "CHARA_POS_X", to_string(charaPos.x));
    Write(ofs, "CHARA_POS_Y", to_string(charaPos.y));
    Write(ofs, "CHARA_POS_Z", to_string(charaPos.z));

    Write(ofs, "CHARA_DIRECT", to_string(RotateY));

    Write(ofs, "CAMERA_POS_X", to_string(cameraPos.x));
    Write(ofs, "CAMERA_POS_Y", to_string(cameraPos.y));
    Write(ofs, "CAMERA_POS_Z", to_string(cameraPos.z));
}

void DrawMMD::LoadModel()
{
    isDraw = false;

    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
    // MV1SetLoadModelPhysicsWorldGravity(-10.f);
    MV1InitModel();

    TCHAR buf[255];
    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, m_AnimPath.c_str(), -1, buf, 255);
    MV1SetLoadModelAnimFilePath(buf);

    MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, m_ModelPath.c_str(), -1, buf, 255);
    std::cout << "load start..." << std::endl;
    model = MV1LoadModel(buf);

    std::cout << "load finish." << std::endl;
    if (model == -1)
    {
        MessageBox(NULL, TEXT("model load error"), NULL, MB_ICONERROR);
        return;
    }
    if (MV1GetAnimNum(model) == 0)
    {
        MessageBox(NULL, TEXT("motion load error"), NULL, MB_ICONERROR);
        return;
    }

    boneHead = MV1SearchFrame(model, L"��");
    defHeadLocalRot = MV1GetFrameLocalMatrix(model, boneHead);


    MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));

    MV1PhysicsResetState(model);

    isDraw = true;
}

void DrawMMD::UpdatePosRot()
{
    MV1SetPosition(model, charaPos);
    MV1SetRotationXYZ(model, VGet(0.0f, RotateY + DX_PI_F, 0.0f));
}

void DrawMMD::SetEnableLight(bool enable)
{
    auto val = enable ? TRUE : FALSE;
    SetUseLighting(val);
}
