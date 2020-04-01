import setuptools
from sys import platform

with open("README.md", "r") as fh:
    long_description = fh.read()

if "linux" in platform:
    extension_mod = setuptools.Extension(
        "las_2g.las_2g_python",
        ["src/las_2g_python_module.c", "src/las_2g_python.c"],
        extra_compile_args=["-D_CRT_SECURE_NO_WARNINGS"]
    )

    debug_extension_mod = setuptools.Extension(
        "las_2g.las_2g_python",
        ["src/las_2g_python_module.c", "src/las_2g_python.c"],
        extra_compile_args=["-D_CRT_SECURE_NO_WARNINGS", "-O0", "-g", "-DDEBUG", "-fno-inline"],
        extra_link_args=['-DEBUG']
    )
elif "win" in platform:
    extension_mod = setuptools.Extension(
        "las_2g.las_2g_python",
        ["src/las_2g_python_module.c", "src/las_2g_python.c"],
        extra_compile_args=["-D_CRT_SECURE_NO_WARNINGS"]
    )

    debug_extension_mod = setuptools.Extension(
        "las_2g.las_2g_python",
        ["src/las_2g_python_module.c", "src/las_2g_python.c"],
        extra_compile_args=["-D_CRT_SECURE_NO_WARNINGS /Zi /0d"],
        extra_link_args=['/DEBUG']
    )

setuptools.setup(
    name="las_2g",
    version="0.0.1",
    author="Ryan Wicks",
    author_email="rwicks@2grobotics.com",
    description="A library for reading and writing LAS files generated by the 2G API.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/tgrobotics/LAS2GPython",
    packages=setuptools.find_packages(),
    python_requires='>=3.6',
    ext_modules=[debug_extension_mod],

)
