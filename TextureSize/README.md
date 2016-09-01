# Texture Size issue

This test exposes two issues:

1. On NVIDIA drivers, the HLSL Texture.GetDimensions instruction doesn't implicitly add the Texture's
base level to the lod parameter. This is inconsistent with the behaviour of the reference rasterizer.

2. When the HLSL shader calls GetDimensions on a Texture whose backing SRV only specifies a limited number of mips,
NVIDIA drivers can query properties of the whole texture, while the reference rasterizer returns default values for mips that are out of range of the SRV.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: NVIDIA

![incorrect rendering](incorrect-nvidia.PNG?raw=true)

### Tested driver versions:

* GOOD: Intel 20.19.15.4463 (5-25-2016)
* BAD: NVIDIA 10.18.13.6881 (7-10-2016)

### Related tests

dEQP-GLES3.functional.shaders.texture_functions.texturesize.*
