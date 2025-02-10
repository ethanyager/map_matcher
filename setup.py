import sys
import pybind11
from setuptools import setup, Extension

# Only use the macOS-specific link flag if we run locally on mac
extra_link_args = []
if sys.platform == "darwin":
    extra_link_args = ["-undefined", "dynamic_lookup"]

ext_modules = [
    Extension(
        'map_matcher',
        ['map_matcher.cpp'],
        include_dirs=[
            pybind11.get_include(),
            pybind11.get_include(True),
        ],
        language='c++',
        extra_compile_args=["-std=c++14"],
        extra_link_args=extra_link_args
    )
]

setup(
    name='map_matcher',
    version='0.1',
    ext_modules=ext_modules,
    install_requires=['pybind11'],
    setup_requires=['pybind11'],
)

