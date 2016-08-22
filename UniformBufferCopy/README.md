# Constant Buffer Init From Staging Buffer

This test exposes a bug on Intel where initializing a constant buffer (also called a
uniform buffer, as in GL) from a staging buffer will result in the first frame having
incorrect data. The workaround in this case is to initialize the constant buffer from
immediate data.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: Intel

![incorrect rendering on Intel](incorrect-intel.PNG?raw=true)

### Tested driver versions:

* GOOD: NVIDIA 10.18.13.6881 (7-10-2016)
* BAD: Intel 10.18.10.4425 (4-4-2016)

### Related tests

* ANGLE end2end_tests: UniformBufferTest.*
* dEQP-GLES3.functional.ubo.*
