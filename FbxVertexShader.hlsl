struct VS_IN//頂点シェーダ入力（頂点レイアウト）
{
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float4 color : COLOR0;//Diffuse
	float2 uv: TEXTURE0;
};

struct VS_OUT//頂点シェーダ出力（ピクセルシェーダ入力）
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv: TEXTURE0;
};

cbuffer ViewBuffer: register(b0)//これはカメラの位置で決まる
{
	float4x4 View;//ビュー変換行列
}

cbuffer ProjectionBuffer: register(b1)//これはカメラの位置で決まる
{
	float4x4 Projection;//透視射影変換行列
}

cbuffer WorldBuffer: register(b2)// メッシュが持つ自身のマトリクス
{
	float4x4 World;// ワールド変換行列
}

VS_OUT main(VS_IN input)
{
	VS_OUT output;

	// 行列変換用に４要素にふやして末尾（w）に1.0fを追加
	output.pos = float4(input.pos, 1.0f);

	// ローカル座標 * ワールド座標変換行列
	output.pos = mul(output.pos, World);

	// ワールド座標 * ビュー座標変換行列
	output.pos = mul(output.pos, View);

	// ビュー座標 * プロジェクション座標変換行列
	output.pos = mul(output.pos, Projection);

	// 頂点カラー
	output.color = input.color;

	// Texture座標指定
	output.uv = input.uv;

	return output;
}