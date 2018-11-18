## import subprocess
# import platform
#from __future__ import division
#from __future__ import print_function

#import zlib
import datetime
#import subprocess
import platform
import os
import os.path
#from warnings import warn

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as _build_ext

#from distutils.core import setup
#from distutils.extension import Extension


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


now = datetime.datetime.now()
ZLIB_PREFIX = os.getenv("ZLIB_PREFIX", None)
IS_WINDOWS = platform.system() == "Windows"

class PathConfigurator(object):
    """
    A class to attempt configuration of the compile search paths
    on various platforms.
    """
    def __init__(self):
        self.include_dirs = []
        self.library_dirs = []
        #try:
            #self._configure_gsl()
        #except OSError as e:
            #warn("Error occured getting GSL path config: {}".format(e))
        ## If the conda prefix is defined, then we are compiling in a conda
        ## context. All include and lib paths should come from within this prefix.
        if ZLIB_PREFIX is not None:
            prefix = ZLIB_PREFIX
            if IS_WINDOWS:
                prefix = prefix
                #prefix = os.path.join(prefix, "Library")
            self.library_dirs.append(os.path.join(prefix, "lib"))
            self.include_dirs.append(os.path.join(prefix, "include"))


libdir = "lib/src/"
includes = [libdir]
sumstat_dir = "summary_statistics/"
random_dir = "random/"

configurator = PathConfigurator()

source_files = ["forest.cc", "node.cc", "forest-debug.cc",
    "node_container.cc", "time_interval.cc", "model.cc", "param.cc",
    "tree_point.cc",
    os.path.join(random_dir, "fastfunc.cc"),
    os.path.join(random_dir, "mersenne_twister.cc"),
    os.path.join(random_dir, "random_generator.cc"),
    os.path.join(sumstat_dir, "tmrca.cc"),
    os.path.join(sumstat_dir, "seg_sites.cc"),
    os.path.join(sumstat_dir, "frequency_spectrum.cc"),
    os.path.join(sumstat_dir, "newick_tree.cc"),
    os.path.join(sumstat_dir, "oriented_forest.cc"),
]


cpp11option = ['-std=c++11']

_scrm_module = Extension(
    '_scrm',
    sources=["scrm-py.cpp"] + [
        os.path.join(libdir, f) for f in source_files],
    extra_compile_args=cpp11option,
    extra_link_args=['-lz'],
    undef_macros=[],
    define_macros=[("VERSION", "\"python\"")],
    include_dirs=[get_pybind_include(),
            get_pybind_include(user=True)] + ["lib/"] + includes + [
        os.path.join(libdir, sumstat_dir)] + [
        os.path.join(libdir, random_dir)] + configurator.include_dirs,
    library_dirs=configurator.library_dirs,
)

with open("README.rst") as f:
    long_description = f.read()

setup(
    name="scrm",
    packages=['scrm'],
    description="scrm CPP module",
    long_description=long_description,
    author=["Paul Staab", "Joe Zhu"],
    version="1.7a0",
    author_email="joe.zhu@bdi.ox.ac.uk",
    url="https://github.com/scrm/scrm-py",
    ext_modules=[_scrm_module],
    keywords=["deconvolution", "mixed infection"],
    license="GNU GPLv3+",
    platforms=["POSIX"],
    classifiers=[
        "Programming Language :: C++",
        "Programming Language :: C",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.3",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Development Status :: 2 - Pre-Alpha",
        "Environment :: Other Environment",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)",
        "Operating System :: POSIX",
        "Operating System :: MacOS :: MacOS X",
        "Topic :: Scientific/Engineering",
        "Topic :: Scientific/Engineering :: Bio-Informatics",
    ],
)
