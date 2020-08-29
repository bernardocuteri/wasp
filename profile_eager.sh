./wasp.bin --compile-eager constraint
make -j8 BUILD=prof clean
make -j8 BUILD=prof
gdb --args wasp.bin --interpreter=cpp_eager --script-directory=. --plugins-file=constraint
#then run < input
