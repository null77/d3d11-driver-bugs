# Copy issue with depth/stencil textures

This test covers a bug when copying stencil data from a depth/stencil texture.
Using the depth and stencil after a copy seems to not store the updated stencil
data under some circumstances, or use the incorrect depth data, resulting in
incorrect rendering.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: AMD

![incorrect rendering on AMD](incorrect-amd.PNG?raw=true)

### Tested driver versions:

* BAD: AMD 16.300.2311.0 (7-18-2016)
* GOOD: NVIDIA 	10.18.13.6881 (7-10-2016)

### Related tests

* dEQP-GLES3.functional.fbo.blit.depth_stencil.depth24_stencil8_stencil_only

