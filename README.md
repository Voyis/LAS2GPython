# LAS2GPython
Python library for reading and writing LAS files created by the 2G API (1 file per profile).

## Running with Valgrind

valgrind --tool=memcheck --suppressions=valgrind-python.supp python -E -tt ./my_python_script.py