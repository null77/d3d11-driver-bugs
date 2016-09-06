# Unary minor operator in dynamic loop issue

This test covers a bug that On some Intel drivers, evaluating
the unary minor operator on a signed integer may get wrong
result. One of such situation is when storing the answer to
an array and reading them in a dynamic for-loop, we will fail
to get the correct values.

### Correct rendering

![correct rendering](correct.PNG?raw=true)

### Incorrect rendering: Intel

![incorrect rendering on Intel](incorrect-intel.PNG?raw=true)

### Tested driver versions:

* BAD: Intel BDW 20.19.15.4463 (5-25-2016)
* GOOD: NVIDIA 

### Related tests

dEQP-GLES3.functional.shaderstruct.struct.local.dynamic_loop_struct_array_vertex
