struct VS_OUTPUT {
  float4 p : POSITION;
  float2 u : TEXCOORD;
};

VS_OUTPUT vs_main(float4 p: POSITION, float2 u: TEXCOORD) {
  VS_OUTPUT o;
  o.p = p;
  o.u = u;
  return o;
};
