#define _USE_MATH_DEFINES
#include <cmath>

#include <Eigen/Geometry>
typedef Eigen::Vector3f Vector3;
typedef Eigen::Matrix3f Matrix3;
typedef Eigen::Quaternionf Quat;
typedef Eigen::AngleAxisf AngleAxisd;

#include "vmd_reader.h"


inline Vector3 rad2deg(Vector3 rad) { return 180.0 / M_PI * rad; }
inline double rad2deg(double rad) { return 180.0 / M_PI * rad; }
inline Vector3 deg2rad(Vector3 deg) { return M_PI / 180.0 * deg; }
// inline Matrix3 q2rpy(Quat q) { return q.matrix().eulerAngles(0, 1, 2); }
inline Quat rpy2q(Vector3 rpy) {
    Quat q = AngleAxisd(rpy[0], Vector3::UnitX())
        * AngleAxisd(rpy[1], Vector3::UnitY())
        * AngleAxisd(rpy[2], Vector3::UnitZ());
    return q;
}
// inline Vector3 q2rpy2deg(Quat q) { return rad2deg(q2rpy(q)); }

int main()
{
    //// 直接指定
    //Quat q1(1.0, 0.0, 0.25, 0.5);

    //// 角度（ラジアン）とベクトルを渡す = Euler角
    //Quat q2(AngleAxisd(1, Vector3::UnitX())     // roll
    //    * AngleAxisd(0, Vector3::UnitY())    // pitch
    //    * AngleAxisd(0, Vector3::UnitZ()));  // yaw

    //// 2つのベクトルの間の角度を求める
    //Quat q3 = Quat::FromTwoVectors(Vector3::UnitX(), Vector3::UnitZ());

    //// 単位クオータニオン(実数が1で虚数が0)
    //Quat q4 = Quat::Identity();

    //// 回転行列から変換
    //Quat q5(Matrix3::Identity());

    //Quat q6 = rpy2q((Vector3(1.0, 0.0, 0.0)));
    //auto q7 = q5 * q6;

    mmd::VMDReader reader;
    auto v = reader.LoadFromFile("input.vmd");

    auto num = v->motions_.size();
    Eigen::Quaternionf r = rpy2q(rad2deg(Vector3(0.0, 0.0, -1.0)));
    Eigen::Quaternionf l = rpy2q(rad2deg(Vector3(0.0, 0.0, 1.0)));
    for (size_t i = 0; i < num; i++)
    {
        auto m = v->motions_[i];
        std::string boneName(m.bone_name);
        if (boneName.find("腕") == std::string::npos || m.bone_name[4] != '\0')
        {
            continue;
        }

        auto rot = l;
        if (boneName.find("右") != std::string::npos)
        {
            rot = r;
        }
        Eigen::Quaternionf baseQ(m.rotation[3], m.rotation[0], m.rotation[1], m.rotation[2]);
        auto ret = rot * baseQ;
        v->motions_[i].rotation[0] = ret.x();
        v->motions_[i].rotation[1] = ret.y();
        v->motions_[i].rotation[2] = ret.z();
        v->motions_[i].rotation[3] = ret.w();
    }

    reader.WriteFile(*v, "output.vmd");
    return 0;
}