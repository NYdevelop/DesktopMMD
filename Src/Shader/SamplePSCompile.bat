@setlocal
@SET PATH=%PATH%;G:\モノオキ\プログラミング\DxLib_VC\Tool\ShaderCompiler

ShaderCompiler.exe /Tvs_3_0 SkinMesh4_DirLight_ToonVS.fx
ShaderCompiler.exe /Tps_3_0 SkinMesh4_DirLight_ToonPS.fx
@endlocal
@rem pause