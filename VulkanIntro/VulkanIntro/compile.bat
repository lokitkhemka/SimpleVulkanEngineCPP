%VULKAN_SDK%\Bin\glslc.exe .\shaders\shader.vert -o .\shaders\shader.vert.spv
%VULKAN_SDK%\Bin\glslc.exe .\shaders\shader.frag -o .\shaders\shader.frag.spv
XCOPY .\shaders\*.* ..\x64\Debug\shaders /C /S /D /Y /I
XCOPY .\models\*.* ..\x64\Debug\models /C /S /D /Y /I
pause