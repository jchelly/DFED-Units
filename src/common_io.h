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
#ifndef SWIFT_COMMON_IO_H
#define SWIFT_COMMON_IO_H

/* Config parameters. */
#include "config.h"

#define FIELD_BUFFER_SIZE 64
#define DESCRIPTION_BUFFER_SIZE 600
#define FILENAME_BUFFER_SIZE 150
#define IO_BUFFER_ALIGNMENT 1024

/* Avoid cyclic inclusion problems */
struct unit_system;

/**
 * @brief The different types of data used in the GADGET IC files.
 *
 * (This is admittedly a poor substitute to C++ templates...)
 */
enum IO_DATA_TYPE {
  INT,
  LONG,
  LONGLONG,
  UINT8,
  UINT,
  UINT64,
  ULONG,
  ULONGLONG,
  FLOAT,
  DOUBLE,
  CHAR,
  SIZE_T,
};

#if defined(HAVE_HDF5)

/* Library header */
#include <hdf5.h>

hid_t io_hdf5_type(enum IO_DATA_TYPE type);

hsize_t io_get_number_element_in_attribute(hid_t attr);
hsize_t io_get_number_element_in_dataset(hid_t dataset);
void io_read_attribute(hid_t grp, const char* name, enum IO_DATA_TYPE type,
                       void* data);
void io_read_attribute_graceful(hid_t grp, const char* name,
                                enum IO_DATA_TYPE type, void* data);
void io_assert_valid_header_cosmology(hid_t h_grp, double a);

void io_read_array_attribute(hid_t grp, const char* name,
                             enum IO_DATA_TYPE type, void* data,
                             hsize_t number_element);
void io_read_array_dataset(hid_t grp, const char* name, enum IO_DATA_TYPE type,
                           void* data, hsize_t number_element);
void io_write_attribute(hid_t grp, const char* name, enum IO_DATA_TYPE type,
                        const void* data, int num);

void io_write_attribute_d(hid_t grp, const char* name, double data);
void io_write_attribute_f(hid_t grp, const char* name, float data);
void io_write_attribute_i(hid_t grp, const char* name, int data);
void io_write_attribute_l(hid_t grp, const char* name, long data);
void io_write_attribute_ll(hid_t grp, const char* name, long long data);
void io_write_attribute_s(hid_t grp, const char* name, const char* str);


void io_read_unit_system(hid_t h_file, struct unit_system* ic_units,
                         const struct unit_system* internal_units,
                         int mpi_rank);

void io_write_unit_system(hid_t h_grp, const struct unit_system* us,
                          const char* groupName);

#endif /* HAVE_HDF5 */

size_t io_sizeof_type(enum IO_DATA_TYPE type);
int io_is_double_precision(enum IO_DATA_TYPE type);

#endif /* SWIFT_COMMON_IO_H */
