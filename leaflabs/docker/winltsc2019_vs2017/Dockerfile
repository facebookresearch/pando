#
# Build a Windows 10 Docker image with Pando build tools installed
# Update the container image by running:
# $ docker build -t registry.gitlab.leaflabs.com/bldg8/ovrsource/winltsc2019_vs2017:[buck_release] .
#
# Update the container registry by running:
# $ docker push registry.gitlab.leaflabs.com/bldg8/ovrsource/winltsc2019_vs2017:[buck_release]
#

FROM mcr.microsoft.com/dotnet/framework/runtime:4.8-windowsservercore-ltsc2019

ARG buck_release=2021.01.12.01

SHELL ["powershell", "-ExecutionPolicy", "Bypass", "-Command"]

# Install Chocolatey
RUN iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# Install build deps using Chocolatey
RUN choco install -y python3 --version 3.6.8; choco install -y llvm --version 8.0.0; choco install -y buck --version $env:buck_release; choco install -y git visualstudio2017-workload-vctools msys2

# Windows distribution of python does not have a python3 alias for python.exe, which we need. Make it.
RUN New-Item -ItemType SymbolicLink -Path "C:\Python36\python3.exe" -Target "C:\Python36\python.exe"

# Install Basler Pylon runtume
RUN Invoke-WebRequest -Uri 'https://www.baslerweb.com/fp-1636375085/media/downloads/software/pylon_software/pylon_Runtime_6.3.0.23157.exe' -OutFile 'pylon.exe'; Start-Process 'pylon.exe' -Wait -ArgumentList '/quiet','/install=Cpp_Runtime'; Remove-Item -Force 'pylon.exe'

# Install Siicon Software Framegrabber runtime
RUN Invoke-WebRequest -Uri 'https://updates.silicon.software/download/fg/win/rt/5.7.0/RuntimeSetup_v5.7.0.76321_Win64.exe' -OutFile 'installer.exe'; Start-Process 'installer.exe' -Wait -ArgumentList '/verysilent','/norestart'; Remove-Item -Force 'installer.exe'

# Configure msys2 profile (use mingw64 subsystem, don't cd to home directory)
ENV MSYSTEM=MINGW64 CHERE_INVOKING=1

# Restore the default Windows shell for correct batch processing.
SHELL ["cmd", "/S", "/C"]

# Set up environment
ENTRYPOINT C:\Program^ Files^ ^(x86^)\Microsoft^ Visual^ Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat &&

# Launch powershell by default
CMD powershell -NoLogo -ExecutionPolicy Bypass
