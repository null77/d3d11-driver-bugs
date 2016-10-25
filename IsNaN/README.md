# isnan issue

This test covers a bug that on some Intel drivers, evaluating
HLSL function isnan() fails to get right answer, such as Intel
D3D drivers on Windows Skylake and Intel OpenGL drivers on Mac
Skylake.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: Intel

![incorrect rendering on Intel](incorrect-intel.PNG?raw=true)

### Tested driver versions:

* BAD: Intel SKL 20.19.15.4483 (7-1-2016)
* GOOD: NVIDIA 

### Related tests

dEQP-GLES3.functional.shadercommonfunction.common.isnan.float_highp_vertex
dEQP-GLES3.functional.shadercommonfunction.common.isnan.float_highp_fragment
dEQP-GLES3.functional.shadercommonfunction.common.isnan.vec2_highp_vertex
dEQP-GLES3.functional.shadercommonfunction.common.isnan.vec2_highp_fragment
dEQP-GLES3.functional.shadercommonfunction.common.isnan.vec3_highp_vertex
dEQP-GLES3.functional.shadercommonfunction.common.isnan.vec3_highp_fragment
dEQP-GLES3.functional.shadercommonfunction.common.isnan.vec4_highp_vertex
dEQP-GLES3.functional.shadercommonfunction.common.isnan.vec4_highp_fragment