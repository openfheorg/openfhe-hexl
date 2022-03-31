# OpenFHE HEXL Acceleration

This repository contains code that can be staged into OpenFHE to enable Intel HEXL acceleration.

# How To Use

This repository is typically used in conjunction with openfhe-configurator.

```
% git clone https://github.com/openfheorg/openfhe-configurator.git
% cd openfhe-configurator
% scripts/configure.sh

OpenFHE configurator

Would you like an OpenFHE Release build? [y/n] : n
Would you like a HEXL build?             [y/n] : y

===============================================================================

Switching to branch openfhe-development main.

Already on 'main'
Your branch is up to date with 'origin/main'.

===============================================================================

Status of branch openfhe-development main.

On branch main
Your branch is up to date with 'origin/main'.

nothing to commit, working tree clean

===============================================================================

Staging [default] build.


===============================================================================

Build [default] is staged.
You may now run scripts/build-openfhe-development.sh to perform a default build.


===============================================================================

Build [hexl-enabled] is staged.
You may now run scripts/build-openfhe-development-hexl.sh to perform a hexl-enabled build.

% scripts/build-openfhe-development-hexl.sh
[ OpenFHE HEXL-enabled build commences]
```

# For More Information

Please see [OpenFHE Development](https://github.com/openfheorg/openfhe-development)
