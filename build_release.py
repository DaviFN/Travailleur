import subprocess
import os

os.makedirs("buildfiles/relwithdebinfo", exist_ok=True)
os.makedirs("bin/relwithdebinfo", exist_ok=True)
subprocess.call('cmake ../.. -G "Visual Studio 17 2022" -D CMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_TOOLCHAIN_FILE=C:\\devtools\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake', cwd="buildfiles/relwithdebinfo")
#subprocess.call('cmake --build . --config RelWithDebInfo', cwd="buildfiles/relwithdebinfo")
#subprocess.call('devenv.com travailleur.sln /build', cwd='buildfiles/debug')
#os.system("msbuild project.sln /p:Configuration=Debug")