# vcf-validator [![Build Status](https://travis-ci.org/EBIvariation/vcf-validator.svg?branch=develop)](https://travis-ci.org/EBIvariation/vcf-validator)

Validator for the Variant Call Format (VCF) implemented using C++11.

It includes all the checks from the vcftools suite, and some more that involve lexical, syntactic and semantic analysis of the VCF input. If any inconsistencies are found, they are classified in one of the following categories:

* Errors: Violations of the VCF specification
* Warnings: An indication that something weird happened (commas were used instead of colons to split ids) or a recommendation is not followed (missing meta-data)

Please read the wiki for more details about checks already implemented.


## Download

We recommend using the [latest release](https://github.com/EBIvariation/vcf-validator/releases) for the most stable experience using vcf-validator. Along with the release notes, you will find the executables `vcf_validator` and `vcf_debugulator`, which will allow you to validate and fix VCF files. Currently we provide executables for Linux and macOS.


## Run

### Validator

vcf-validator accepts both compressed and non-compressed input VCF files. Supported compression formats are .bz2, .gz and .z. For other formats such as .zip, the `zcat` command and a pipe can be used (see below).

Reading uncompressed files:
  * File path as argument: `vcf_validator -i /path/to/file.vcf`
  * Standard input: `vcf_validator < /path/to/file.vcf`
  * Standard input from pipe: `cat /path/to/file.vcf | vcf_validator`

Reading compressed files:
  * File path as argument: `vcf_validator -i /path/to/compressed_file.vcf.gz`
  * Standard input: `vcf_validator < /path/to/compressed_file.vcf.z`
  * Standard input from pipe: `zcat /path/to/compressed_file.vcf.zip | vcf_validator`

The validation level can be configured using `-l` / `--level`. This parameter is optional and accepts 3 values:

* error: Display only syntax errors
* warning: Display both syntax and semantic, both errors and warnings (default)
* stop: Stop after the first syntax error is found

Different types of validation reports can be written with the `-r` / `--report` option. Several ones may be specified in the same execution, using commas to separate each type (without spaces, e.g.: `-r summary,database,text`).

* summary: Write a human-readable summary report to a file. This includes one line for each type of error and the number of occurrences, along with the first line that shows that type of error (default)
* text: Write a human-readable report to a file, with one description line for each VCF line that has an error.
* database: Write structured report to a database file. The database engine used is SQLite3, so the results can be inspected manually, but they are intended to be consumed by other applications.

Each report is written into its own file and it is named after the input file, followed by a timestamp. The default output directory is the same as the input file's if provided using `-i`, or the current directory if using the standard input; it can be changed with the `-o` / `--outdir` option.

### Debugulator

There are some simple errors that can be automatically fixed. The most common error is the presence of duplicate variants. The needed parameters are the original VCF and the report generated by a previous run of the vcf_validator with the option `-r database`.

The fixed VCF will be written into the standard output, which you can redirect to a file, or use the `-o` / `--output` option and specify the desired file name.

The logs about what the debugulator is doing will be written into the error output. The logs may be redirected to a log file using `2>debugulator_log.txt` or completely discarded with `2>/dev/null`.

### Examples

Simple example: `vcf_validator -i /path/to/file.vcf`

Full example: `vcf_validator -i /path/to/file.vcf -l stop -r database,stdout -o /path/to/output/folder/`

Debugulator example:
```
vcf_validator -i /path/to/file.vcf -r database -o /path/to/write/report/
vcf_debugulator -i /path/to/file.vcf -e /path/to/write/report/vcf.errors.timestamp.db -o /path/to/fixed.vcf 2>debugulator_log.txt
```


## Build

If you would like to use an unreleased version of vcf-validator, you can build it under 3 platforms: Docker (generates Linux binary without installing dependencies), Linux and macOS. A statically linked executable will be generated, which means you won't need to install any dependencies to run it.

### Docker

The easiest way to build vcf-validator is using the Docker image provided with the source code. This will create an executable that can be run on any Linux machine.

1. Install and configure Docker following [their tutorial](https://docs.docker.com/engine/getstarted/).
2. Create the Docker image:
    1. Clone this Git repository: `git clone https://github.com/EBIvariation/vcf-validator.git`
    2. Move to the folder the code was downloaded to: `cd vcf-validator`
    3. Build the image: `docker build -t ebivariation/vcf-validator docker/`. Please replace `ebivariation` with your user account if you plan to push this image to [Docker Hub](https://hub.docker.com).
3. Build the executable running `docker run -v ${PWD}:/tmp ebivariation/vcf-validator`. Again, replace `ebivariation` with your user name if necessary.

Executables will be created in the `build/bin` subfolder.

### Linux

The build has been tested on the following compilers:
* Clang 3.9 to 5.0
* GCC 4.8 to 6.0

#### Dependencies

You can easily install some of the required dependencies running `./install_dependencies.sh linux`, and you may run `./install_dependencies.sh --help` for help. Otherwise please follow the steps below.

##### Compression libraries

You will need to install the bzip2 and zlib development packages. For Ubuntu users, these are `libbz2-dev` and `zlib1g-dev`.

##### Boost

The dependencies are the Boost library core, and its submodules: Boost.filesystem, Boost.iostreams, Boost.program_options, Boost.regex, Boost.log and Boost.system. If you are using Ubuntu, the required packages' names will be `libboost-dev`, `libboost-filesystem-dev`, `libboost-iostreams`, `libboost-program-options-dev`, `libboost-regex-dev` and `libboost-log-dev`.

##### ODB

You will need to download the ODB compiler, the ODB common runtime library, and the SQLite database runtime library from [this page](http://codesynthesis.com/products/odb/download.xhtml).

ODB requires SQLite3 to be installed. If you are using Ubuntu, the required packages' names will be `libsqlite3-0` and `libsqlite3-dev`.

To install the ODB compiler, the easiest way is to download the `.deb` or `.rpm` packages and install them automatically with `dpkg`. Both the ODB runtime and SQLite database runtime libraries can be installed manually running `./configure && make && sudo make install`. This will install the libraries in `/usr/local/lib`.

If you don't have root permissions, please run `./configure --prefix=/path/to/odb/libraries/folder` to specify which folder to install ODB in, then `make && make install`, without `sudo`. Also you will have to provide the path to ODB while configuring libodb-sqlite using `./configure --with-libodb=/path/to/odb/libraries`.

#### Compile

In order to create the build scripts, please run `cmake` with your preferred generator. For instance, `cmake -G "Unix Makefiles"` will create Makefiles, and to build the binaries, you will need to run `make`.

If the ODB libraries were not found during the build, please run `sudo updatedb && sudo ldconfig`. Also, if ODB has been installed in a non-default location, the option `-DEXT_LIB_PATH=/path/to/external/libraries/folder` must be also provided to the `cmake` command.

Binaries will be created in the `bin` subfolder.


### macOS

On macOS the binaries obtained will only have system libraries dynamically linked. This means you will need to install dependencies to build vcf-validator but not to run it.

#### Dependencies

In order to compile this project, first you need to run `brew install cmake ninja boost sqlite3`.

Now you can easily install ODB ORM and compression libraries just by running `./install_dependencies.sh osx`. You may run `./install_dependencies.sh --help` for usage instructions.

Finally, add the `osx_dependencies/odb-2.4.0-i686-macosx/bin` subfolder to your PATH to be able to run the ODB compiler.

#### Compile

In order to create the build scripts, please run `cmake` with your preferred generator. For instance, `cmake -G "Unix Makefiles"` will create Makefiles, and to build the binaries, you will need to run `make`.

Binaries will be created in the `bin` subfolder.


### Windows

On Windows the binaries obtained will only have odb libraries dynamically linked, boost and system libraries are statically linked. We have provided the pre-compiled dependencies odb dlls and libs within the repository. You will need to download and build boost and download required headers for odb.

#### Dependencies

##### Compression libraries

You will need to download the bzip2 and zlib source code, from [here](http://www.bzip.org/downloads.html) and [here](https://zlib.net/zlib1211.zip) respectively.

##### Boost

The dependencies are the Boost library core, and its submodules: Boost.filesystem, Boost.iostreams, Boost.program_options, Boost.regex, Boost.log and Boost.system. You will need to compile them with zlib and bzip2 support and statically linking the runtime libraries.

- Download Boost from [here](https://www.boost.org/users/download/) and uncompress it
- From the directory where Boost was uncompressed, run these commands:

```
bootstrap
.\b2 --with-atomic --with-chrono --with-date_time --with-filesystem --with-log --with-program_options --with-regex --with-system --with-thread --with-iostreams -sBZIP2_SOURCE=path\to\bzip2-1.x.x -sZLIB_SOURCE=path\to\zlib-1.x.x runtime-link=static --build-type=complete
```
- Add boost_1_xx_x/stage/lib folder to the environment variable `LIB`
- Add boost_1_xx_x folder to the environment variable `INCLUDE`

##### ODB

Precompiled libraries of odb and odb-sqlite are provided. In order to download headers, simply run the comand `install_dependencies.bat`, which will create a `windows_dependencies` folder in the root directory of project.

#### Compile

In order to create the build scripts and compile vcf-validator, please run the following commands from the project root folder:

```
cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" /path/to/CMakeLists.txt
nmake
```

Binaries will be created in the `bin` subfolder.

In order to run those binaries, you will need to add the `lib/windows_specific` directory to the `PATH`. This will allow the dll files inside that directory to be found.



## Deliverables

The following binaries are be created after successful build:

* `vcf_validator`: validation tool
* `vcf_debugulator`: automatic fixing tool
* `test_validator` and derivatives: testing correct behaviour of the tools listed above


## Tests

Unit tests can be run using the binary `bin/test_validator` or, if the generator supports it, a command like `make test`. The first option may provide a more detailed output in case of test failure.

**Note**: Tests that require input files will only work when executed with `make test` or running the binary from the project root folder (not the `bin` subfolder).

## Generate code from descriptors

Code generated from descriptors shall be always up-to-date in the GitHub repository. If changes to the source descriptors were necessary, please generate the Ragel machines C code from `.ragel` files using:

```
ragel -G2 src/vcf/vcf_v41.ragel -o inc/vcf/validator_detail_v41.hpp
ragel -G2 src/vcf/vcf_v42.ragel -o inc/vcf/validator_detail_v42.hpp
ragel -G2 src/vcf/vcf_v43.ragel -o inc/vcf/validator_detail_v43.hpp
```

And the full ODB-based code from the classes definitions using:

```
odb --include-prefix vcf --std c++11 -d sqlite --generate-query --generate-schema --hxx-suffix .hpp --ixx-suffix .ipp --cxx-suffix .cpp --output-dir inc/vcf/ inc/vcf/error.hpp
mv inc/vcf/error-odb.cpp src/vcf/error-odb.cpp
```


### Build ODB Libraries for windows

This section is for building odb libraries for windows. You may ignore it if you want to use pre-compiled libraries given inside the repository. To build those libraries first download the source code using install_dependencies.bat it will create following directories in windows_dependencies folder.
    - libodb-2.4.0
    - libodb-sqlite-2.4.0
    - sqlite
    - odb (header files only)
you will have to compile libodb-2.4.0, sqlite and then libodb-sqlite-2.4.0. To do so you will need Visual Studio IDE (tested on VS-Studio-2017).

#### Build Odb runtime
    1. Open libodb-2.4.0/libodb-vc12.sln in VS-Studio. File->Open->Project/Solution.
    2. Retarget the solution file to latest version. Project->Retarget Solution.
    3. Select Build type Release and configuration win32.
    4. Select /MT in Project->Properties->Configuration Properties->C/C++->Code Generation->Runtime Library options.
    5. Build the solution using Build->Build Solution.

#### Build Sqlite for odb-sqlite
    1. Open sqlite/sqlite3-vc12.sln in VS-Studio.
    2. Retarget the solution file.
    3. Select Build type Release and configuration win32.
    4. Select /MT in Project->Properties->Configuration Properties->C/C++->Code Generation->Runtime Library options.
    5. Build the solution using Build->Build Solution.

#### Build Odb-sqlite runtime
    1. Open libodb-sqlite-2.4.0/libodb-sqlite-vc12.sln in VS-Studio.
    2. Retarget the solution file.
    3. Select Build type Release and configuration win32.
    4. Select /MT in Project->Properties->Configuration Properties->C/C++->Code Generation->Runtime Library options.
    5. Go to Project->Properties->Configuration Properties->VC++ Directories and append these paths to following variables.
        - Executable Directories => path/to/libodb-2.4.0/bin and path/to/sqlite/bin
        - Include Directories => path/to/libodb-2.4.0 and path/to/sqlite
        - Library Directories => path/to/libodb-2.4.0/lib and path/to/sqlite/lib
    Note the paths should be absolute and the directories will be present within windows_dependencies folder.
    6. Build the solution using Build->Build Solution.

Now you will obitain compiled libs and dlls in lib and bin folders respectively.
