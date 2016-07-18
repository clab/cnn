from setuptools import setup
from setuptools.extension import Extension
from Cython.Distutils import build_ext


# Remove the "-Wstrict-prototypes" compiler option, which isn't valid for C++.
import distutils.sysconfig
cfg_vars = distutils.sysconfig.get_config_vars()
if "CFLAGS" in cfg_vars:
       cfg_vars["CFLAGS"] = cfg_vars["CFLAGS"].replace("-Wstrict-prototypes", "")

COMPILER_ARGS=["-std=c++11"]
import platform

# in some OSX systems, the following extra flags are needed:
if platform.system() == 'Darwin':
    COMPILER_ARGS.extend(["-stdlib=libc++","-mmacosx-version-min=10.7"])

ext = Extension(
        "_pycnn",                 # name of extension
        ["pycnn.pyx"],           # filename of our Pyrex/Cython source
        language="c++",              # this causes Pyrex/Cython to create C++ source
        include_dirs=["../../cnn/", # this is the location of the main cnn directory.
                      "../../eigen/"], # this is the directory where eigen is saved.
        libraries=['cnn_shared'],             # ditto
        library_dirs=["."],
        #extra_link_args=["-L/home/yogo/Vork/Research/cnn/cnn/build/cnn"],       # if needed
        extra_compile_args=COMPILER_ARGS,
        runtime_library_dirs=["$ORIGIN/./"],
        )

setup(ext_modules = [ext],
        cmdclass = {'build_ext': build_ext},
        name="pyCNN",
        py_modules = ['pycnn','pycnn_viz'],
)
