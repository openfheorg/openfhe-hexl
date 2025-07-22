# OpenFHE HEXL Acceleration

This repository contains code that can be staged into OpenFHE v1.3.1 to enable Intel HEXL v1.2.6 acceleration.

# How To Use

This repository is typically used in conjunction with [openfhe-configurator](https://github.com/openfheorg/openfhe-configurator).

Before following the steps below, we recommend following the guidelines outlined in the `Notes specific to the Intel HEXL backend` section of
[Building OpenFHE for Best Performance](https://github.com/openfheorg/openfhe-development/blob/main/docs/static_docs/Best_Performance.md).

```
% git clone https://github.com/openfheorg/openfhe-configurator.git
% cd openfhe-configurator
% scripts/configure.sh

OpenFHE configurator

Would you like to stage an openfhe-development build?     [y/n] : n
Would you like to stage an openfhe-hexl build?            [y/n] : y

===============================================================================

Cloning openfhe-development repository.

===============================================================================

Switching to openfhe-development branch v1.3.1.

===============================================================================

Status of openfhe-development brach v1.3.1.

===============================================================================

Staging [default] build.

===============================================================================

Build [default] is staged.
You may now run scripts/build-openfhe-development.sh to perform a default build.

===============================================================================

Cloning openfhe-hexl repository.

===============================================================================

Switching to openfhe-hexl branch v1.3.1.0.

===============================================================================

Status of openfhe-hexl branch v1.3.1.0.

===============================================================================

Staging [hexl-enabled] build.

===============================================================================

Build [hexl-enabled] is staged.
You may now run scripts/build-openfhe-development.sh to perform a hexl-enabled build.

% scripts/build-openfhe-development.sh
[ OpenFHE HEXL-enabled build commences ]
```

By default, the `OpenFHE` and `HEXL` libraries are built and installed at `openfhe-configurator/openfhe-staging/install`.
If you want to change the install location, pass an `OPENFHE_INSTALL_DIR` value to the build script e.g.,

```
% OPENFHE_INSTALL_DIR=~/local scripts/build-openfhe-development.sh
```

If you want to change OpenFHE build paramaters, pass those changes as `CMAKE_FLAGS` e.g.,

```
% CMAKE_FLAGS="-DWITH_OPENMP=OFF -DCMAKE_BUILD_TYPE=Debug" scripts/build-openfhe-development.sh
```

If you want to use a preinstalled build of the Intel HEXL library with your openfhe-hexl build, include the following:

```
% CMAKE_FLAGS="-DINTEL_HEXL_PREBUILT=ON -DINTEL_HEXL_HINT_DIR=[PATH TO INTEL HEXL]" scripts/build-openfhe-development.sh
```

# For More Information

Please see [OpenFHE Development](https://github.com/openfheorg/openfhe-development)
