struct VS_IN //頂点シェーダ入力 INPUT LAYOUT
{
	float3 pos : POSITION0;
	float3 nor : NORMAL0;
	float4 color : COLOR0; //Diffuse（表面拡散光色）
	float2 uv : TEXTURE0;

	//SkinMesh Unityちゃんは５点以上の影響ボーンがあるクラスタがあるので二つ（最大８）必要
	uint4 indices0 : BLENDINDICES0;
	uint4 indices1 : BLENDINDICES1;
	float4 weights0 : BLENDWEIGHT0;
	float4 weights1 : BLENDWEIGHT1;
};
struct VS_OUT //ピクセルシェーダへの出力
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXTURE0;
};

cbuffer ViewBuffer : register(b0) //これはカメラの位置で決まる
{
	float4x4 View; // ビュー変換行列
}
cbuffer ProjectionBuffer : register(b1) //これはカメラの位置で決まる
{
	float4x4 Projection; // 射影変換行列
}
cbuffer WorldBuffer : register(b2) // メッシュが持つ自身のマトリクス
{
	float4x4 World; // ワールド変換行列
}
cbuffer MeshBone : register(b3)
{
	float4x4 bones[2]; //ボーン用配列。データがオーバーしても可変長になるので大丈夫。
	//長さが１だと固定長になってしまうので２以上にする事。
}

VS_OUT main(VS_IN input)
{
	VS_OUT output;

	output.pos = float4(input.pos, 1.0f);

	float4x4 boneMtx;										// アニメマトリクス作成 UnityちゃんのBoneは最大６つだけどね。
	boneMtx = bones[input.indices0[0]] * input.weights0[0]; //indicesに入っている値はbonesのインデックス番号
	boneMtx += bones[input.indices0[1]] * input.weights0[1];
	boneMtx += bones[input.indices0[2]] * input.weights0[2];
	boneMtx += bones[input.indices0[3]] * input.weights0[3];
	boneMtx += bones[input.indices1[0]] * input.weights1[0];
	boneMtx += bones[input.indices1[1]] * input.weights1[1];
	boneMtx += bones[input.indices1[2]] * input.weights1[2];
	boneMtx += bones[input.indices1[3]] * input.weights1[3];

	output.pos = mul(boneMtx, output.pos); // ローカル座標 * アニメマトリクス メッシュ自体の変形なので最初
	output.pos = mul(output.pos, World); // ローカル座標 * ワールド座標変換行列
	output.pos = mul(output.pos, View); // ワールド座標 * ビュー座標変換行列
	output.pos = mul(output.pos, Projection); // ビュー座標 * プロジェクション座標変換行列
	output.color = input.color; // 頂点カラー
	output.uv = input.uv; // Texture座標指定
	return output;
}