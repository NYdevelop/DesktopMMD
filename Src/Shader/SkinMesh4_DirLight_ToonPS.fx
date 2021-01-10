// ピクセルシェーダーの入力
struct PS_INPUT
{
    float2 TexCoords0      : TEXCOORD0;    // テクスチャ座標
    float3 VPosition       : TEXCOORD1;    // 座標( ビュー空間 )
    float3 VNormal         : TEXCOORD2;    // 法線( ビュー空間 )
};

// ピクセルシェーダーの出力
struct PS_OUTPUT
{
    float4 Color0          : COLOR0;
};

// マテリアルパラメータ
struct MATERIAL
{
    float4 Diffuse;      // ディフューズカラー
    float4 Specular;     // スペキュラカラー
    float4 Power;        // スペキュラの強さ
};

// ライトパラメータ
struct LIGHT
{
    float4 Position;               // 座標( ビュー空間 )
    float3 Direction;              // 方向( ビュー空間 )
    float4 Diffuse;                // ディフューズカラー
    float4 Specular;               // スペキュラカラー
    float4 Ambient;                // アンビエントカラーとマテリアルのアンビエントカラーを乗算したもの
    float4 Range_FallOff_AT0_AT1;  // x:有効距離  y:スポットライト用FallOff  z:距離による減衰処理用パラメータ０  w:距離による減衰処理用パラメータ１
    float4 AT2_SpotP0_SpotP1;      // x:距離による減衰処理用パラメータ２  y:スポットライト用パラメータ０( cos( Phi / 2.0f ) )  z:スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )
};


// C++ 側で設定するテクスチャや定数の定義
sampler  DiffuseMapTexture             : register(s0);		// ディフューズマップテクスチャ
// sampler  ToonTexture                   : register(s1);		// トゥーンテクスチャ
sampler  ToonSphereMapTexture          : register(s5);		// トゥーンレンダリング用スフィアマップテクスチャ
float4   cfAmbient_Emissive            : register(c1);		// エミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー
MATERIAL cfMaterial : register(c2);		// マテリアルパラメータ
float4   cfFactorColor                 : register(c5);		// 不透明度等
LIGHT    cfLight[1]                  : register(c32);		// ライトパラメータ


                                                            // main関数
PS_OUTPUT main(PS_INPUT PSInput)
{
    PS_OUTPUT PSOutput;
    float4 lTextureDiffuseColor;
    float4 lToonSphereMapColor;
    float4 lSpecularColor;
    float4 lDiffuseColor;
    float3 Normal;
    float lDiffuseAngleGen;
    float4 lTotalDiffuse;
    float4 lTotalSpecular;
    float4 lTotalAmbient;
    // float4 lToonColor;
    float lTotalLightGen;
    float3 V_to_Eye;
    float3 halfF3;
    float Temp;
    float3 lLightTemp;
    float3 lLightDir;


    // 法線の準備
    Normal = normalize(PSInput.VNormal);

    // 頂点座標から視点へのベクトルを正規化
    V_to_Eye = normalize(-PSInput.VPosition);

    // ディフューズカラーとスペキュラカラーとアンビエントカラーの合計値を初期化
    lTotalDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    lTotalSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    lTotalAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // ライトの減衰率合計値の初期化
    lTotalLightGen = 0.0f;



    // ディレクショナルライトの処理 +++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

    // ライト方向ベクトルのセット
    lLightDir = cfLight[0].Direction;


    // ディフューズ角度減衰率計算
    lDiffuseAngleGen = saturate(dot(Normal, -lLightDir));

    // ディフューズ減衰率を合計値に加算
    lTotalLightGen += lDiffuseAngleGen;


    // スペキュラカラー計算

    // ハーフベクトルの計算
    halfF3 = normalize(V_to_Eye - lLightDir);

    // Temp = pow( max( 0.0f, N * H ), cfMaterial.Power.x )
    Temp = pow(max(0.0f, dot(Normal, halfF3)), cfMaterial.Power.x);

    // スペキュラライト合計値 += スペキュラ角度減衰計算結果 * ライトのスペキュラカラー
    lTotalSpecular += Temp * cfLight[0].Specular;


    // ライトのディフューズカラーを合計値に加算
    lTotalDiffuse += cfLight[0].Diffuse;

    // ライトのアンビエントカラーを合計値に加算
    lTotalAmbient += cfLight[0].Ambient;

    // ディレクショナルライトの処理 +++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

    // アンビエントカラーの蓄積値 += マテリアルのアンビエントカラーとグローバルアンビエントカラーを乗算したものとマテリアルエミッシブカラーを加算したもの
    lTotalAmbient += cfAmbient_Emissive;






    // 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

    // テクスチャカラーの取得
    lTextureDiffuseColor = tex2D(DiffuseMapTexture, PSInput.TexCoords0);

    // トゥーンテクスチャカラーをライトのディフューズ減衰率から取得
    //lToonColor = tex2D(ToonTexture, lTotalLightGen);

    // ディフューズカラー = ライトのディフューズカラー蓄積値 * マテリアルのディフューズカラー
    lDiffuseColor = lTotalDiffuse  * cfMaterial.Diffuse;

    // スペキュラカラー = ライトのスペキュラカラー蓄積値 * マテリアルのスペキュラカラー
    lSpecularColor = lTotalSpecular;// *cfMaterial.Specular;

    // 出力 = saturate( saturate( ディフューズカラー * アンビエントカラーの蓄積値 ) * トゥーンテクスチャカラー + スペキュラカラー ) * テクスチャカラー
    //PSOutput.Color0.rgb = saturate(saturate(lDiffuseColor.rgb + lTotalAmbient.rgb) * lToonColor.rgb + lSpecularColor.rgb) * lTextureDiffuseColor.rgb;
    PSOutput.Color0.rgb = saturate(saturate(lDiffuseColor.rgb + lTotalAmbient.rgb) + lSpecularColor.rgb) * lTextureDiffuseColor.rgb;

    // スフィアマップの色を取得
    halfF3 = Normal * float3(0.5f, -0.5f, 0.0f) + float3(0.5f, 0.5f, 0.5f);
    lToonSphereMapColor = tex2D(ToonSphereMapTexture, halfF3.xy);

    /* スフィアマップの効果が乗算の場合はこちらを加算の２行の代わりに使います
    // スフィアマップの色を出力に乗算
    PSOutput.Color0.rgb *= lToonSphereMapColor.rgb ;
    */

    // スフィアマップの色を出力に加算
    PSOutput.Color0.rgb += lToonSphereMapColor.rgb;

    // アルファ値 = ディフューズアルファ * マテリアルのディフューズアルファ * 不透明度
    PSOutput.Color0.a = lTextureDiffuseColor.a * cfMaterial.Diffuse.a * cfFactorColor.a;

    // 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )


    // 出力パラメータを返す
    return PSOutput;
}