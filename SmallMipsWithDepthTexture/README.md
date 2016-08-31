# Depth-Stencil Textures with Small Mips

This test covers a bug with rendering with small mips with depth textures that
also have stencil channels.
The 2x2 and 1x1 mips of a 8x8 or larger depth texture seem to always come out
as zero on AMD hardware. This was giving incorrect rendering in some WebGL
and dEQP tests. Both D24S8 and D32S8 were tested as problematic.

A possible workaround is to use other formats specifically when rendering depth
textures using multiple levels, but this has some downside from the extra bandwidth
and copying.

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
