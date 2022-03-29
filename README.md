# SWIFT Unit Handling Library

This is a C library extracted from the [SWIFT n-body simulation code]
(www.swiftsim.com) which can be used to store, operate on and read/write
unit information in numerical simulations.

It is assumed that the simulation code uses a consistent set of units
internally such that, for example, velocity units are length units divided
by time units. The library provides facilities to describe the systems of
units used internally and in input or output files and to convert between
systems.

Unit information is NOT carried along with data arrays within the simulation
code. This avoids any performance impact and minimizes the code changes which
are necessary to make use of the library.

## Compilation

This package requires a C compiler and the HDF5 library. Building from
the git repository also requires GNU autotools. First, to generate the
configure script
```
./autogen.sh
```

Then the library can be compiled and installed with
```
./configure CC=<compiler> --prefix=<installation_dir> --with-hdf5=<hdf5_dir>
make
make install
```

Here, `<compiler>` is the name of the compiler command to use, 
`<installation_dir>` is the location in which the library and headers will
be installed, and `<hdf5_dir>` should contain the HDF5 include and lib
directories.

## Using the library

### Linking to the library

In order to use the library it's necessary to link to libswift_units and include
the header swift_units.h. This can typically be achieved by adding the following
your compiler flags 
```
-I<installation_dir>/include/
```
and adding the following to your linker flags
```
-L<installation_dir>/lib/ -lswift_units
```

The examples directory contains a Makefile which compiles a small program which
calls the library. It may be necessary to edit the line which sets
SWIFT_UNITS_DIR to specify the library installation location.

### Creating a unit system

A unit system consists of a set of base units of mass, length, time etc. The
units of other quantities are expressed as powers of these units. The library
provides a `struct unit_system` which stores the choice of base units,
specified in CGS units.

To initialize a new unit system given the base units:
```
void units_init(struct unit_system* us, double U_M_in_cgs, double U_L_in_cgs,
                double U_t_in_cgs, double U_C_in_cgs, double U_T_in_cgs);
```
Here, `us` is the struct describing the unit system and `U_*_in_cgs` are the
base units expressed in the CGS system:
  * U_M_in_cgs: size of the mass unit
  * U_L_in_cgs: size of the length unit
  * U_t_in_cgs: size of the time unit
  * U_C_in_cgs: size of the current unit
  * U_T_in_cgs: size of the temperature init

There are also two built in, standard unit systems. To create a CGS unit system:
```
struct unit_system us_cgs;
units_init_cgs(&us_cgs);
```
and to create an SI unit system:
```
struct unit_system us_si;
units_init_si(&us_si);
```

### Specifying units and defining new units

Once the base unit system has been defined, derived units can be specified
as powers of the base units.

Units are specified using the type `enum unit_conversion_factor`, defined
in swift_units.h. For each unit we need to define the exponents of mass,
length, time, current and temperature. This is done in the function
`units_get_base_unit_exponents_array` in swift_units.c.

To add a new unit:
  * Add a new entry to `enum unit_conversion_factor` in swift_units.h
  * Modify units_get_base_unit_exponents_array() in swift_units.c to return
    the base units exponents for the new unit

A possible enhancement to the library would be to allow definition of new
units on the fly and storage of unit definitions in output files.

### Converting between unit systems

A physical quantity can be converted between different units as follows:
  * Define the unit system describing the units in which the quantity is
    currently stored
  * Define the unit system which the quantity is to be converted to
  * Call `units_conversion_factor()` to get the conversion factor between the
    two systems

The function is defined as
```
double units_conversion_factor(const struct unit_system* from,
                               const struct unit_system* to,
                               enum unit_conversion_factor unit);
```
Here, `from` and `to` are the unit systems to convert between. `unit` is an
instance of `enum unit_conversion_factor` which specifies the unit - e.g. 
`UNIT_CONV_VELOCITY` if the quantity is a velocity. 

The function returns the factor the quantity must be multiplied by to convert
it to unit system `to`.

The typical use of this function is to convert between the unit system used
internally in a simulation and the unit system used in the input or output
files.

### Reading and writing unit systems

A unit system can be written to a HDF5 file as a HDF5 group with attributes
specifying the base units. The function to do this is:
```
void units_write_hdf5(hid_t h_file, const struct unit_system* us,
                      const char* groupName);
```
where `h_file` is the HDF5 identifier of the file or group in which
to write the unit system, `us` is the unit system struct, and groupName is the
name of the HDF5 group to create.

A unit system can be initialised and read in from a HDF5 file with
```
void units_init_from_hdf5(hid_t h_file, struct unit_system* us,
                          const char *groupName);
```
where `h_file` is the HDF5 file or group to read the unit system from, `us` is
the unit system struct, and groupName is the name of the HDF5 group to read.

### Adding unit information to HDF5 datasets

Given a unit system and the unit of a particular quantity the library can
attach HDF5 attributes to a dataset describing its units. This is done with

```
void units_add_to_hdf5_dataset(hid_t h_data, const struct unit_system *us,
                               enum unit_conversion_factor unit);
```
Here `us` is the unit system, `unit` is the enum specifying the units of this
particular quantity and `h_data` is the HDF5 identifier for the dataset the
metadata will be added to.

## Examples

The program in examples/cosmology.c shows how to call the library to do the
following:

  * define a new system of units
  * convert between unit systems
  * write a unit system to a HDF5 file
  * read a unit system back from a HDF5 file
  * write a HDF5 dataset with units metadata attached

The program can be compiled by running make in the examples directory. It may
be necessary to edit the Makefile to specify the location of the units library
and the necessary flags to link to HDF5.

## Using the units metadata in python

