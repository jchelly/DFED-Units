.PHONY: all
all:	cgs_units cosmology

clean:
	\rm -f cgs_units cosmology

# Location where the library was installed
SWIFT_UNITS_DIR=$(HOME)/swift_units

# Flags to find the header file
SWIFT_UNITS_INC=-I${SWIFT_UNITS_DIR}/include/

# Flags to find the library
SWIFT_UNITS_LIB=-L${SWIFT_UNITS_DIR}/lib/ -Wl,-rpath=${SWIFT_UNITS_DIR}/lib/ -lswift_units ${LDFLAGS} -lhdf5

cgs_units:	cgs_units.c
	$(CC) cgs_units.c ${SWIFT_UNITS_INC} ${SWIFT_UNITS_LIB} -o cgs_units

cosmology:	cosmology.c
	$(CC) cosmology.c ${SWIFT_UNITS_INC} ${SWIFT_UNITS_LIB} -o cosmology
