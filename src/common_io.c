/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2012 Pedro Gonnet (pedro.gonnet@durham.ac.uk),
 *                    Matthieu Schaller (matthieu.schaller@durham.ac.uk).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

/* Config parameters. */
#include "../config.h"

/* This object's header. */
#include "common_io.h"

/* Local includes. */
#include "swift_units.h"
#include "error.h"
/* #include "version.h" */

/* Some standard headers. */
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_HDF5)

#include <hdf5.h>

/**
 * @brief Converts a C data type to the HDF5 equivalent.
 *
 * This function is a trivial wrapper around the HDF5 types but allows
 * to change the exact storage types matching the code types in a transparent
 *way.
 */
hid_t io_hdf5_type(enum IO_DATA_TYPE type) {

  switch (type) {
    case INT:
      return H5T_NATIVE_INT;
    case UINT8:
      return H5T_NATIVE_UINT8;
    case UINT:
      return H5T_NATIVE_UINT;
    case UINT64:
      return H5T_NATIVE_UINT64;
    case LONG:
      return H5T_NATIVE_LONG;
    case ULONG:
      return H5T_NATIVE_ULONG;
    case LONGLONG:
      return H5T_NATIVE_LLONG;
    case ULONGLONG:
      return H5T_NATIVE_ULLONG;
    case FLOAT:
      return H5T_NATIVE_FLOAT;
    case DOUBLE:
      return H5T_NATIVE_DOUBLE;
    case CHAR:
      return H5T_NATIVE_CHAR;
    default:
      error("Unknown type");
      return 0;
  }
}

/**
 * @brief Return 1 if the type has double precision
 *
 * Returns an error if the type is not FLOAT or DOUBLE
 */
int io_is_double_precision(enum IO_DATA_TYPE type) {

  switch (type) {
    case FLOAT:
      return 0;
    case DOUBLE:
      return 1;
    default:
      error("Invalid type");
      return 0;
  }
}

/**
 * @brief Reads an attribute (scalar) from a given HDF5 group.
 *
 * @param grp The group from which to read.
 * @param name The name of the attribute to read.
 * @param type The #IO_DATA_TYPE of the attribute.
 * @param data (output) The attribute read from the HDF5 group.
 *
 * Calls #error() if an error occurs.
 */
void io_read_attribute(hid_t grp, const char* name, enum IO_DATA_TYPE type,
                       void* data) {

  const hid_t h_attr = H5Aopen(grp, name, H5P_DEFAULT);
  if (h_attr < 0) error("Error while opening attribute '%s'", name);

  const hid_t h_err = H5Aread(h_attr, io_hdf5_type(type), data);
  if (h_err < 0) error("Error while reading attribute '%s'", name);

  H5Aclose(h_attr);
}

/**
 * @brief Reads an attribute from a given HDF5 group.
 *
 * @param grp The group from which to read.
 * @param name The name of the attribute to read.
 * @param type The #IO_DATA_TYPE of the attribute.
 * @param data (output) The attribute read from the HDF5 group.
 *
 * Exits gracefully (i.e. does not read the attribute at all) if
 * it is not present, unless debugging checks are activated. If they are,
 * and the read fails, we print a warning.
 */
void io_read_attribute_graceful(hid_t grp, const char* name,
                                enum IO_DATA_TYPE type, void* data) {

  /* First, we need to check if this attribute exists to avoid raising errors
   * within the HDF5 library if we attempt to access an attribute that does
   * not exist. */
  const htri_t h_exists = H5Aexists(grp, name);

  if (h_exists <= 0) {
    /* Attribute either does not exist (0) or function failed (-ve) */
#ifdef SWIFT_DEBUG_CHECKS
    message("WARNING: attribute '%s' does not exist.", name);
#endif
  } else {
    /* Ok, now we know that it exists we can read it. */
    const hid_t h_attr = H5Aopen(grp, name, H5P_DEFAULT);

    if (h_attr >= 0) {
      const hid_t h_err = H5Aread(h_attr, io_hdf5_type(type), data);
      if (h_err < 0) {
        /* Explicitly do nothing unless debugging checks are activated */
#ifdef SWIFT_DEBUG_CHECKS
        message("WARNING: unable to read attribute '%s'", name);
#endif
      }
    } else {
#ifdef SWIFT_DEBUG_CHECKS
      if (h_attr < 0) {
        message("WARNING: was unable to open attribute '%s'", name);
      }
#endif
    }

    H5Aclose(h_attr);
  }
}

/**
 * @brief Reads the number of elements in a HDF5 attribute.
 *
 * @param attr The attribute from which to read.
 *
 * @return The number of elements.
 *
 * Calls #error() if an error occurs.
 */
hsize_t io_get_number_element_in_attribute(hid_t attr) {
  /* Get the dataspace */
  hid_t space = H5Aget_space(attr);
  if (space < 0) error("Failed to get data space");

  /* Read the number of dimensions */
  const int ndims = H5Sget_simple_extent_ndims(space);

  /* Read the dimensions */
  hsize_t* dims = (hsize_t*)malloc(sizeof(hsize_t) * ndims);
  H5Sget_simple_extent_dims(space, dims, NULL);

  /* Compute number of elements */
  hsize_t count = 1;
  for (int i = 0; i < ndims; i++) {
    count *= dims[i];
  }

  /* Cleanup */
  free(dims);
  H5Sclose(space);
  return count;
};

/**
 * @brief Reads an attribute (array) from a given HDF5 group.
 *
 * @param grp The group from which to read.
 * @param name The name of the dataset to read.
 * @param type The #IO_DATA_TYPE of the attribute.
 * @param data (output) The attribute read from the HDF5 group (need to be
 * already allocated).
 * @param number_element Number of elements in the attribute.
 *
 * Calls #error() if an error occurs.
 */
void io_read_array_attribute(hid_t grp, const char* name,
                             enum IO_DATA_TYPE type, void* data,
                             hsize_t number_element) {

  /* Open attribute */
  const hid_t h_attr = H5Aopen(grp, name, H5P_DEFAULT);
  if (h_attr < 0) error("Error while opening attribute '%s'", name);

  /* Get the number of elements */
  hsize_t count = io_get_number_element_in_attribute(h_attr);

  /* Check if correct number of element */
  if (count != number_element) {
    error(
        "Error found a different number of elements than expected (%lli != "
        "%lli) in attribute %s",
        count, number_element, name);
  }

  /* Read attribute */
  const hid_t h_err = H5Aread(h_attr, io_hdf5_type(type), data);
  if (h_err < 0) error("Error while reading attribute '%s'", name);

  /* Cleanup */
  H5Aclose(h_attr);
}

/**
 * @brief Reads the number of elements in a HDF5 dataset.
 *
 * @param dataset The dataset from which to read.
 *
 * @return The number of elements.
 *
 * Calls #error() if an error occurs.
 */
hsize_t io_get_number_element_in_dataset(hid_t dataset) {
  /* Get the dataspace */
  hid_t space = H5Dget_space(dataset);
  if (space < 0) error("Failed to get data space");

  /* Read the number of dimensions */
  const int ndims = H5Sget_simple_extent_ndims(space);

  /* Read the dimensions */
  hsize_t* dims = (hsize_t*)malloc(sizeof(hsize_t) * ndims);
  H5Sget_simple_extent_dims(space, dims, NULL);

  /* Compute number of elements */
  hsize_t count = 1;
  for (int i = 0; i < ndims; i++) {
    count *= dims[i];
  }

  /* Cleanup */
  free(dims);
  H5Sclose(space);
  return count;
};

/**
 * @brief Reads a dataset (array) from a given HDF5 group.
 *
 * @param grp The group from which to read.
 * @param name The name of the dataset to read.
 * @param type The #IO_DATA_TYPE of the attribute.
 * @param data (output) The attribute read from the HDF5 group (need to be
 * already allocated).
 * @param number_element Number of elements in the attribute.
 *
 * Calls #error() if an error occurs.
 */
void io_read_array_dataset(hid_t grp, const char* name, enum IO_DATA_TYPE type,
                           void* data, hsize_t number_element) {

  /* Open dataset */
  const hid_t h_dataset = H5Dopen(grp, name, H5P_DEFAULT);
  if (h_dataset < 0) error("Error while opening attribute '%s'", name);

  /* Get the number of elements */
  hsize_t count = io_get_number_element_in_dataset(h_dataset);

  /* Check if correct number of element */
  if (count != number_element) {
    error(
        "Error found a different number of elements than expected (%lli != "
        "%lli) in dataset %s",
        count, number_element, name);
  }

  /* Read dataset */
  const hid_t h_err = H5Dread(h_dataset, io_hdf5_type(type), H5S_ALL, H5S_ALL,
                              H5P_DEFAULT, data);
  if (h_err < 0) error("Error while reading dataset '%s'", name);

  /* Cleanup */
  H5Dclose(h_dataset);
}

/**
 * @brief Write an attribute to a given HDF5 group.
 *
 * @param grp The group in which to write.
 * @param name The name of the attribute to write.
 * @param type The #IO_DATA_TYPE of the attribute.
 * @param data The attribute to write.
 * @param num The number of elements to write
 *
 * Calls #error() if an error occurs.
 */
void io_write_attribute(hid_t grp, const char* name, enum IO_DATA_TYPE type,
                        const void* data, int num) {

  const hid_t h_space = H5Screate(H5S_SIMPLE);
  if (h_space < 0)
    error("Error while creating dataspace for attribute '%s'.", name);

  hsize_t dim[1] = {(hsize_t)num};
  const hid_t h_err = H5Sset_extent_simple(h_space, 1, dim, NULL);
  if (h_err < 0)
    error("Error while changing dataspace shape for attribute '%s'.", name);

  const hid_t h_attr =
      H5Acreate1(grp, name, io_hdf5_type(type), h_space, H5P_DEFAULT);
  if (h_attr < 0) error("Error while creating attribute '%s'.", name);

  const hid_t h_err2 = H5Awrite(h_attr, io_hdf5_type(type), data);
  if (h_err2 < 0) error("Error while reading attribute '%s'.", name);

  H5Sclose(h_space);
  H5Aclose(h_attr);
}

/**
 * @brief Write a string as an attribute to a given HDF5 group.
 *
 * @param grp The group in which to write.
 * @param name The name of the attribute to write.
 * @param str The string to write.
 * @param length The length of the string
 *
 * Calls #error() if an error occurs.
 */
void io_writeStringAttribute(hid_t grp, const char* name, const char* str,
                             int length) {

  const hid_t h_space = H5Screate(H5S_SCALAR);
  if (h_space < 0)
    error("Error while creating dataspace for attribute '%s'.", name);

  const hid_t h_type = H5Tcopy(H5T_C_S1);
  if (h_type < 0) error("Error while copying datatype 'H5T_C_S1'.");

  const hid_t h_err = H5Tset_size(h_type, length);
  if (h_err < 0) error("Error while resizing attribute type to '%i'.", length);

  const hid_t h_attr = H5Acreate1(grp, name, h_type, h_space, H5P_DEFAULT);
  if (h_attr < 0) error("Error while creating attribute '%s'.", name);

  const hid_t h_err2 = H5Awrite(h_attr, h_type, str);
  if (h_err2 < 0) error("Error while reading attribute '%s'.", name);

  H5Tclose(h_type);
  H5Sclose(h_space);
  H5Aclose(h_attr);
}

/**
 * @brief Writes a double value as an attribute
 * @param grp The group in which to write
 * @param name The name of the attribute
 * @param data The value to write
 */
void io_write_attribute_d(hid_t grp, const char* name, double data) {
  io_write_attribute(grp, name, DOUBLE, &data, 1);
}

/**
 * @brief Writes a float value as an attribute
 * @param grp The group in which to write
 * @param name The name of the attribute
 * @param data The value to write
 */
void io_write_attribute_f(hid_t grp, const char* name, float data) {
  io_write_attribute(grp, name, FLOAT, &data, 1);
}

/**
 * @brief Writes an int value as an attribute
 * @param grp The group in which to write
 * @param name The name of the attribute
 * @param data The value to write
 */
void io_write_attribute_i(hid_t grp, const char* name, int data) {
  io_write_attribute(grp, name, INT, &data, 1);
}

/**
 * @brief Writes a long value as an attribute
 * @param grp The group in which to write
 * @param name The name of the attribute
 * @param data The value to write
 */
void io_write_attribute_l(hid_t grp, const char* name, long data) {
  io_write_attribute(grp, name, LONG, &data, 1);
}

/**
 * @brief Writes a long long value as an attribute
 * @param grp The group in which to write
 * @param name The name of the attribute
 * @param data The value to write
 */
void io_write_attribute_ll(hid_t grp, const char* name, long long data) {
  io_write_attribute(grp, name, LONGLONG, &data, 1);
}

/**
 * @brief Writes a string value as an attribute
 * @param grp The group in which to write
 * @param name The name of the attribute
 * @param str The string to write
 */
void io_write_attribute_s(hid_t grp, const char* name, const char* str) {
  io_writeStringAttribute(grp, name, str, strlen(str));
}

/**
 * @brief Reads the Unit System from an IC file.
 *
 * If the 'Units' group does not exist in the ICs, we will use the internal
 * system of units.
 *
 * @param h_file The (opened) HDF5 file from which to read.
 * @param ic_units The unit_system to fill.
 * @param internal_units The internal system of units to copy if needed.
 * @param mpi_rank The MPI rank we are on.
 */
void io_read_unit_system(hid_t h_file, struct unit_system* ic_units,
                         const struct unit_system* internal_units,
                         int mpi_rank) {

  /* First check if it exists as this is *not* required. */
  const htri_t exists = H5Lexists(h_file, "/Units", H5P_DEFAULT);

  if (exists == 0) {

    if (mpi_rank == 0)
      message("'Units' group not found in ICs. Assuming internal unit system.");

    units_copy(ic_units, internal_units);

    return;

  } else if (exists < 0) {
    error("Serious problem with 'Units' group in ICs. H5Lexists gives %d",
          exists);
  }

  if (mpi_rank == 0) message("Reading IC units from ICs.");
  hid_t h_grp = H5Gopen(h_file, "/Units", H5P_DEFAULT);

  /* Ok, Read the damn thing */
  io_read_attribute(h_grp, "Unit length in cgs (U_L)", DOUBLE,
                    &ic_units->UnitLength_in_cgs);
  io_read_attribute(h_grp, "Unit mass in cgs (U_M)", DOUBLE,
                    &ic_units->UnitMass_in_cgs);
  io_read_attribute(h_grp, "Unit time in cgs (U_t)", DOUBLE,
                    &ic_units->UnitTime_in_cgs);
  io_read_attribute(h_grp, "Unit current in cgs (U_I)", DOUBLE,
                    &ic_units->UnitCurrent_in_cgs);
  io_read_attribute(h_grp, "Unit temperature in cgs (U_T)", DOUBLE,
                    &ic_units->UnitTemperature_in_cgs);

  /* Clean up */
  H5Gclose(h_grp);
}

/**
 * @brief Writes the current Unit System
 * @param h_file The (opened) HDF5 file in which to write
 * @param us The unit_system to dump
 * @param groupName The name of the HDF5 group to write to
 */
void io_write_unit_system(hid_t h_file, const struct unit_system* us,
                          const char* groupName) {

  const hid_t h_grpunit = H5Gcreate1(h_file, groupName, 0);
  if (h_grpunit < 0) error("Error while creating Unit System group");

  io_write_attribute_d(h_grpunit, "Unit mass in cgs (U_M)",
                       units_get_base_unit(us, UNIT_MASS));
  io_write_attribute_d(h_grpunit, "Unit length in cgs (U_L)",
                       units_get_base_unit(us, UNIT_LENGTH));
  io_write_attribute_d(h_grpunit, "Unit time in cgs (U_t)",
                       units_get_base_unit(us, UNIT_TIME));
  io_write_attribute_d(h_grpunit, "Unit current in cgs (U_I)",
                       units_get_base_unit(us, UNIT_CURRENT));
  io_write_attribute_d(h_grpunit, "Unit temperature in cgs (U_T)",
                       units_get_base_unit(us, UNIT_TEMPERATURE));

  H5Gclose(h_grpunit);
}

#endif /* HAVE_HDF5 */

/**
 * @brief Returns the memory size of the data type
 */
size_t io_sizeof_type(enum IO_DATA_TYPE type) {

  switch (type) {
    case INT:
      return sizeof(int);
    case UINT8:
      return sizeof(uint8_t);
    case UINT:
      return sizeof(unsigned int);
    case UINT64:
      return sizeof(uint64_t);
    case LONG:
      return sizeof(long);
    case ULONG:
      return sizeof(unsigned long);
    case LONGLONG:
      return sizeof(long long);
    case ULONGLONG:
      return sizeof(unsigned long long);
    case FLOAT:
      return sizeof(float);
    case DOUBLE:
      return sizeof(double);
    case CHAR:
      return sizeof(char);
    case SIZE_T:
      return sizeof(size_t);
    default:
      error("Unknown type");
      return 0;
  }
}

