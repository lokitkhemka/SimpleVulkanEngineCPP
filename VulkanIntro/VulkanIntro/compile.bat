C:\VulkanSDK\1.3.216.0\Bin\glslc.exe .\shaders\shader.vert -o .\shaders\shader.vert.spv
C:\VulkanSDK\1.3.216.0\Bin\glslc.exe .\shaders\shader.frag -o .\shaders\shader.frag.spv
XCOPY .\shaders\*.* ..\x64\Debug\shaders /C /S /D /Y /I
pause