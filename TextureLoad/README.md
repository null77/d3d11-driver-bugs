# Texture.Load issue

This test covers an issue that on some Intel drivers, HLSL's function
texture.Load returns 0 when the parameter Location is negative, even
if the sum of Offset and Location is in range.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: Intel

![incorrect rendering on Intel](incorrect-intel.PNG?raw=true)

### Tested driver versions:

* BAD: Intel 20.19.15.4463 (5-25-2016)

### Related tests

dEQP-GLES3.functional.shadertexturefunction.texelfetchoffset.*
