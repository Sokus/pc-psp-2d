#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xy          0     NONE   float   xy  
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    0   xyz         2     NONE   float   xyz 
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    0   xyzw        2     NONE   float   xyzw
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_input v0.xy
dcl_input v1.xy
dcl_input v2.xyz
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o2.xyzw
mov o0.xy, v0.xyxx
mov o0.zw, l(0,0,0,1.000000)
mov o1.xy, v1.xyxx
mov o2.xyz, v2.xyzx
mov o2.w, l(1.000000)
ret 
// Approximately 0 instruction slots used
#endif

const BYTE d3d11_vshader[] =
{
     68,  88,  66,  67, 244,  65, 
     59,  57,   1,  73,  85, 203, 
     75,   8, 185, 213,  49, 254, 
    246, 104,   1,   0,   0,   0, 
    228,   1,   0,   0,   3,   0, 
      0,   0,  44,   0,   0,   0, 
    156,   0,   0,   0,  16,   1, 
      0,   0,  73,  83,  71,  78, 
    104,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
     80,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   3,   0,   0, 
     89,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   3,   3,   0,   0, 
     98,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   7,   7,   0,   0, 
     80,  79,  83,  73,  84,  73, 
     79,  78,   0,  84,  69,  88, 
     67,  79,  79,  82,  68,   0, 
     67,  79,  76,  79,  82,   0, 
     79,  83,  71,  78, 108,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  92,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,  12,   0,   0, 101,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  67,  79,  76,  79,  82, 
      0, 171,  83,  72,  69,  88, 
    204,   0,   0,   0,  80,   0, 
      1,   0,  51,   0,   0,   0, 
    106,   8,   0,   1,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      2,   0,   0,   0, 103,   0, 
      0,   4, 242,  32,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
     50,  32,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8, 194,  32,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
     54,   0,   0,   5,  50,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  16,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
    114,  32,  16,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   5, 130,  32,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     62,   0,   0,   1
};