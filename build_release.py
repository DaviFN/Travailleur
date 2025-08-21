import subprocess
import os
import platform

# Create output directories
os.makedirs("buildfiles/relwithdebinfo", exist_ok=True)
os.makedirs("bin/relwithdebinfo", exist_ok=True)

# Detect platform
is_windows = platform.system() == "Windows"

# Set generator and toolchain paths
if is_windows:
    generator = 'Visual Studio 17 2022'
    toolchain = r'C:\devtools\vcpkg\scripts\buildsystems\vcpkg.cmake'
else:
    generator = 'Unix Makefiles'
    vcpkg_root = os.environ.get("VCPKG_ROOT", os.path.expanduser("~/vcpkg"))
    toolchain = os.path.join(vcpkg_root, "scripts/buildsystems/vcpkg.cmake")

# Build the cmake command
cmake_cmd = [
    'cmake', '../..',
    '-G', generator,
    '-D', 'CMAKE_BUILD_TYPE=RelWithDebugInfo',
    '-D', f'CMAKE_TOOLCHAIN_FILE={toolchain}'
]

# Run cmake to generate build files
subprocess.check_call(cmake_cmd, cwd="buildfiles/relwithdebinfo")