#ifndef _CONFIG_LOADER_H_
#define _CONFIG_LOADER_H_

// 2018/06/29

#include <string>

class CConfigLoader                             // �ݒ�t�@�C���ǂݍ��݃N���X
{
public:
    CConfigLoader(const std::string& fileName);   // �R���X�g���N�^
    ~CConfigLoader();                             // �f�X�g���N�^

    std::string Load(const std::string& keyWord); // �L�[���[�h������ǂݍ���

    void SetRemoveDoubleQuatation(bool isRemove);

private:
    std::string RemoveSpace(const std::string& str);

    std::string m_FileName;                       // �ݒ�t�@�C����

    bool m_IsRemoveDoubleQuatation = false;
};

#endif
