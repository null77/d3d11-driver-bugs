# 24-bit Depth Textures with Small Mips

This test covers a bug with rendering with small mips of 24-bit depth texture.
The 2x2 and 1x1 mips of a 8x8 or larger depth texture seem to always come out
as zero on AMD hardware. This was giving incorrect rendering in some WebGL
and dEQP tests. A possible workaround is to use 32-bit depth formats instead,
but this has some downside from the extra bandwidth.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: AMD

![incorrect rendering on AMD](incorrect-amd.PNG?raw=true)

### Tested driver versions:

* BAD: AMD 16.300.2311.0 (7-18-2016)
* GOOD: NVIDIA 10.18.13.6881 (7-10-2016)
* GOOD: Intel 10.18.10.4425 (4-4-2016)

### Related tests

* dEQP-GLES3.functional.texture.specification.texstorage2d.format.depth_component24_2d
* Possibly many more.
