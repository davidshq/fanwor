# Contributing

## Visual Studio Code
You may need to create a `c_cpp_properties.json` file in your `.vscode` folder. The contents of mine currently look like:
```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/src",
                "${workspaceFolder}/src/arch/atari",
                "${workspaceFolder}/src/arch/sdl",
                "/usr/include/SDL",
                "/usr/include/SDL2"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c11",
            "cppStandard": "c++14",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
} 
```