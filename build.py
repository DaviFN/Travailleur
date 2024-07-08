import subprocess
import os

os.makedirs("buildfiles/debug", exist_ok=True)
os.makedirs("bin/debug", exist_ok=True)
subprocess.call('cmake ../.. -G "Visual Studio 17 2022" -D CMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:\\devtools\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake', cwd="buildfiles/debug")
#subprocess.call('devenv.com travailleur.sln /build', cwd='buildfiles/debug')
#os.system("msbuild project.sln /p:Configuration=Debug")