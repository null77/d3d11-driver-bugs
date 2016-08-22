# ClearRenderTargetView synching issue

This test covers a bug with ClearRenderTargetView which would sometimes
lead to rendering with undefined results. The rendering seems to be fixed
when inserting Flush calls, leading to a conclusion there is a bug with
synchronization. See the angle_code.cpp, an uncomment the Flush to fix the bug.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: Intel

![incorrect rendering on Intel](incorrect-intel.PNG?raw=true)

### Incorrect rendering: NVIDIA

![incorrect rendering on NVIDIA](incorrect-nvidia.PNG?raw=true)

### Tested driver versions:

* BAD: Intel 10.18.10.4425 (4-4-2016)
* GOOD: NVIDIA 	10.18.13.6881 (7-10-2016)

### Related tests

dEQP-GLES3.functional.fbo.color.repeated_clear.sample.tex2d.*
