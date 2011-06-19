void ColouredCubicVoxelVP(
    float4 inPosition    : POSITION,
    float4 inNormal        : NORMAL,

    out float4 outClipPosition        : POSITION,
    out float4 outWorldPosition    : TEXCOORD0,
    out float4 outWorldNormal    : TEXCOORD1,

    uniform float4x4 world,
    uniform float4x4 viewProj
    )
{   
    //Compute the world space position
    outWorldPosition = mul(world, inPosition);   
   
    //Just pass though the normals without transforming them in any way. No rotation occurs.
    outWorldNormal = inNormal;

    //Compute the clip space position
    outClipPosition = mul(viewProj, outWorldPosition);
}

void ColouredCubicVoxelFP(
    float4 inPosition        : POSITION,
    float4 inWorldPosition    : TEXCOORD0,
    float4 inWorldNormal    : TEXCOORD1,

    uniform sampler2D heightMap : TEXUNIT0,
   
    out float4 result        : COLOR)
{   
    inWorldNormal = normalize(inWorldNormal);
   
    float3 col;
   
    //World position is used as texture coordinates. Choose which
    //two components of world position to use based on normal. Could
    //optionally use a different texture for each face here as well.
    if(inWorldNormal. x > 0.5)
    {
        col = tex2D(heightMap, inWorldPosition.yz);
    }
   
    if(inWorldNormal. x < -0.5)
    {
        col = tex2D(heightMap, inWorldPosition.yz);
    }
   
    if(inWorldNormal. y > 0.5)
    {
        col = tex2D(heightMap, inWorldPosition.xz);
    }
   
    if(inWorldNormal. y < -0.5)
    {
        col = tex2D(heightMap, inWorldPosition.xz);
    }
   
    if(inWorldNormal. z > 0.5)
    {
        col = tex2D(heightMap, inWorldPosition.xy);
    }
   
    if(inWorldNormal. z < -0.5)
    {
        col = tex2D(heightMap, inWorldPosition.xy);
    }
   
    result = float4(col, 1.0);
}