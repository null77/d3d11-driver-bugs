# Texture.Sample issue

This test covers an issue that On some Intel drivers (e.g. Broadwell),
when intending to pass a uniform variable as the texture coordinate
into HLSL function texture.Sample, the driver will crash after
CreatePixelShader() is called.

### Tested driver versions:

* BAD: Intel 20.19.15.4463 (5-25-2016)
