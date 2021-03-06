import os
import shutil
import subprocess
import argparse

PROJECT_ROOT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BUILD_PARAMETERS = {}

def get_parser():
    parser = argparse.ArgumentParser(description='Arguments parser')
    parser.add_argument("-conf", "--conf", type=str, help="build configuration (Release/Debug)")
    parser.add_argument("-build", "--build", type=str, help="server, client or all ('all' by default)")
    parser.add_argument("-c", "--c_compiler", type=str, help="C compiler (for example: gcc)")
    parser.add_argument("-cxx", "--cxx_compiler", type=str, help="CXX compiler (for example: g++)")
    return parser

def check_args_values(args):
    """The fucntion checks that all arguments are correct and set them into global variables."""

    if not isinstance(args.conf, type(None)):
        if args.conf.lower() in ["release", "debug"]:
            BUILD_PARAMETERS["conf"] = args.conf.lower().title()
        else:
            raise ValueError("Incorrect configuration type (see help for more details)")
    if not isinstance(args.build, type(None)):
        if args.build.lower() in ["server", "client", "all"]:
            BUILD_PARAMETERS["build"] = args.build.lower()
        else:
            raise ValueError("Incorrect build type (see help for more details)")
    if not (isinstance(args.c_compiler, type(None)) and isinstance(args.cxx_compiler, type(None))):
        if not isinstance(args.c_compiler, type(None)) and not isinstance(args.cxx_compiler, type(None)):
            BUILD_PARAMETERS["c_compiler"] = args.c_compiler
            BUILD_PARAMETERS["cxx_compuler"] = args.cxx_compiler
        else:
            raise ValueError("Please specify C and CXX compilers")

def set_default_parameters():
    """Set default values into global variables"""

    global BUILD_PARAMETERS

    BUILD_PARAMETERS = {
        "conf": "Release",
        "build": "all",
        "c_compiler": "gcc",
        "cxx_compiler": "g++",
    }

def build():
    """Build application executable files"""

    build_dir = os.path.join(PROJECT_ROOT_DIR, "build")
    if os.path.exists(build_dir):
        while True:
            is_remove = input(f"[WARNING]: '{build_dir}' already exists! Do you want to overwrite it? [y/n]: ").lower()
            if is_remove in ["y", "n"]:
                break
        if is_remove == "y":
            shutil.rmtree(build_dir)
            os.mkdir(os.path.join(PROJECT_ROOT_DIR, "build"))

    cmake_cmd = ["cmake",  "-B", f"{build_dir}", f"-DCMAKE_C_COMPILER={BUILD_PARAMETERS['c_compiler']}",
                f"-DCMAKE_CXX_COMPILER={BUILD_PARAMETERS['cxx_compiler']}", f"-DCMAKE_BUILD_TYPE={BUILD_PARAMETERS['conf']}"]
    if BUILD_PARAMETERS["build"] == "client":
        cmake_cmd.extend(["-DBUILD_SERVER=OFF", "-DBUILD_CLIENT=ON"])
    elif BUILD_PARAMETERS["build"] == "server":
        cmake_cmd.extend(["-DBUILD_SERVER=ON", "-DBUILD_CLIENT=OFF"])
    print(f"[DEBUG]: cmake command line: {' '.join(cmake_cmd)}")
    subprocess.call(cmake_cmd)

    make_cmd = ["make", "-C", f"{build_dir}", "VERBOSE=1", "install"]
    print(f"[DEBUG]: make command line: {' '.join(make_cmd)}")
    subprocess.call(make_cmd)

def main():
    set_default_parameters()

    args = get_parser().parse_args()
    check_args_values(args)

    build()

if __name__ == '__main__':
    main()
