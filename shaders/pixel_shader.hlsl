
Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);
TextureCube texCubeMap : register(t2);

SamplerState texSampler : register(s0);
SamplerState cubeMapSampler : register(s1);

cbuffer LightCamBuffer : register(b0)
{
    float4 light_position;
    float4 camera_position;
};

cbuffer MaterialBuffer : register(b1)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float shininess;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
	float2 TexCoord : TEX;
    float3 PosWorld : WORLD;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	//return float4(input.Binormal*0.5+0.5, 1);
	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
    
    float3 T = normalize(input.Tangent);
    float3 B = normalize(input.Binormal);
    float3 N = normalize(input.Normal);
     
    float3x3 TBN = transpose(float3x3(T, B, N));
    
    float4 diffuse_texture = texDiffuse.Sample(texSampler, input.TexCoord);
    float3 normal_texture = (texNormal.Sample(texSampler, input.TexCoord).xyz) * 2 - 1;
     
    N = mul(TBN, normal_texture);
    
    float3 L = normalize(light_position.xyz - input.PosWorld.xyz);
    float3 R = reflect(-L, N);
    float3 V = normalize(camera_position.xyz - input.PosWorld.xyz);
    
    //float3 refraction = refract(-L, N, 1);
    
    float4 cubemap_texture = texCubeMap.Sample(cubeMapSampler, R);
    float4 skybox_texture = texCubeMap.Sample(cubeMapSampler, V);

    
    float4 lambert_diffuse = max(dot(N, L), 0);
    float4 specular_highlight = max(pow(abs(dot(R, V)), /*shininess*/10), 0);
    
    float4 ambient_component = ambient;
    float4 diffuse_component = diffuse * lambert_diffuse * diffuse_texture;
    float4 specular_component = specular * specular_highlight;
    
    float4 phong_illumination = /* ambient_component*/+diffuse_component + specular_component;

    return float4(phong_illumination.xyz, 1.0);

}