# ClearRenderTargetView with RGB565 issue

This test covers a bug with DXGI_FORMAT_B5G6R5_UNORM.
Refer to https://msdn.microsoft.com/en-us/library/windows/desktop/ff471324(v=vs.85).aspx.
DXGI_FORMAT_B5G6R5_UNORM in D3D11 should be equivalent to D3DFMT_R5G6B5 in D3D9.
However, in Intel driver, it will be treated as B5G6R5 not R5G6B5.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: Intel

![incorrect rendering on Intel](incorrect-intel.PNG?raw=true)

### Tested driver versions:

* BAD: Intel 20.19.15.4463(5/25/2016)

### Related tests

dEQP-GLES3.functional.fbo.blit.default_framebuffer.rgb565_nearest_scale_blit_from_default
dEQP-GLES3.functional.fbo.blit.default_framebuffer.rgb565_linear_scale_blit_from_default
dEQP-GLES3.functional.fbo.blit.default_framebuffer.rgb565_linear_out_of_bounds_blit_from_default
