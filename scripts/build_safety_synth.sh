cd "$BUILD" && rm -r safety_synthesis
cd "$BUILD" && mkdir safety_synthesis
cd "$SAFETY_SYNT" && mkdir build

## prepare the safety_synthesis executable
cd "$SAFETY_SYNT"/build && cmake ../../ && make -j 4
cp "$SAFETY_SYNT"/build/safety_synthesis/aiger_synt "$SAFETY_SYNT_BUILD"