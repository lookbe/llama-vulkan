🦙 Llama.cpp Vulkan Build Setup

This project depends on a custom Llama.cpp build with Vulkan support enabled.
Follow the steps below to correctly clone, build, and link the required libraries.

⚙️ 1. Clone Llama.cpp

First, clone the Llama.cpp
 repository:

git clone https://github.com/ggerganov/llama.cpp.git
cd llama.cpp

🏗️ 2. Build Llama-Vulkan

To build the Vulkan version of Llama.cpp:

mkdir build
cd build
cmake .. -A x64


Then open the generated Visual Studio solution (llama.sln) inside the build folder and build it using Visual Studio 2022.

💡 Ensure that the Vulkan SDK is installed and available in your system path.

🔧 3. Build with Vulkan Support (CMake Command)

Alternatively, you can configure and build directly using CMake with Vulkan enabled:

mkdir build
cd build
cmake .. -DGGML_VULKAN=on -DLLAMA_CURL=off -DBUILD_SHARED_LIBS=off -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release


This will generate .lib files with Vulkan acceleration enabled.

📦 4. Copy Built Libraries

After building, locate the .lib files in the Llama.cpp build output folders (e.g., ggml.lib, llama.lib, etc.)
Then copy them into your project’s corresponding directories:

project_root/
├── lib/
│   ├── debug/
│   │   └── *.lib   ← copy debug .lib files here
│   └── release/
│       └── *.lib   ← copy release .lib files here


From:

llama.cpp/build/Debug/*.lib    →  lib/debug/
llama.cpp/build/Release/*.lib  →  lib/release/

✅ 5. Verify Integration

In your project settings (CMake or Visual Studio):

Include directories: llama.cpp/include

Library directories: lib/debug or lib/release (depending on build configuration)

Once linked correctly, you’ll have a Vulkan-accelerated version of Llama.cpp integrated with your project.
