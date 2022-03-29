#include <stdlib.h>
#include <stdio.h>
#include <hdf5.h>

#include "swift_units.h"

int main(int argc, char *argv[]) {

  /* Initialize a cgs unit system */
  struct unit_system us_cgs;
  units_init_cgs(&us_cgs);

  /*
    Initialize a unit system for cosmological simulations

    Rather than specifying the time unit, here we specify
    length and velocity units then derive the time unit.
  */
  struct unit_system us_cosmo;
  double U_M = 1.98841e33;    /* Solar masses */
  double U_L = 3.08567758e24; /* Megaparsecs */
  double U_V = 1.0e5;         /* km/sec */
  double U_t = U_L / U_V;     /* Time unit, given that lengths are Mpc and velocities are km/s */
  double U_C = 1.0;           /* Current (A) */
  double U_T = 1.0;           /* Temperature (K) */
  units_init(&us_cosmo, U_M, U_L, U_t, U_C, U_T);

  /* Print out the unit systems */
  printf("CGS unit system:\n");
  units_print(&us_cgs);
  printf("\n");

  printf("Cosmological unit system:\n");
  units_print(&us_cosmo);
  printf("\n");

  /* Convert a length in Mpc to cm */
  double length_mpc = 5.0;
  double length_cm = length_mpc * units_conversion_factor(&us_cosmo, &us_cgs, UNIT_CONV_LENGTH);
  printf("Length %.2f Mpc = %12.4e cm\n\n", length_mpc, length_cm);

  /* Write the unit system to a HDF5 file */
  hid_t h_file = H5Fcreate("cosmo_units.hdf5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  units_write_hdf5(h_file, &us_cosmo, "CosmologicalUnits");
  H5Fclose(h_file);

  /* Read the unit system back in */
  struct unit_system us_read;
  h_file = H5Fopen("cosmo_units.hdf5", H5F_ACC_RDONLY, H5P_DEFAULT);
  units_init_from_hdf5(h_file, &us_read, "CosmologicalUnits");
  H5Fclose(h_file);

  printf("Cosmological unit system read back from HDF5 file:\n");
  units_print(&us_read);
  printf("\n");

  /* Generate random points in a 100Mpc box */
  const double boxsize = 100.0;
  const int nr_points = 10000;
  double *pos_mpc = malloc(3*sizeof(double)*nr_points);
  for(int i=0; i<nr_points; i+=1) {
    for(int j=0; j<3; j+=1) {
      pos_mpc[3*i+j] = ((double) rand()) / RAND_MAX * boxsize;
    }
  }

  /* Write the points to a HDF5 file */
  h_file = H5Fcreate("cosmo_points.hdf5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  hsize_t dims[2];
  dims[0] = nr_points;
  dims[1] = 3;
  hid_t h_space = H5Screate_simple(2, dims, NULL);
  hid_t h_data = H5Dcreate2(h_file, "Coordinates", H5T_NATIVE_DOUBLE, h_space, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, pos_mpc);

  /* Write the cosmological unit system to the file */
  units_write_hdf5(h_file, &us_cosmo, "CosmologicalUnits");
  
  /* Attach unit information to the dataset */
  units_add_to_hdf5_dataset(h_data, &us_cosmo, UNIT_CONV_LENGTH);

  H5Dclose(h_data);
  H5Sclose(h_space);
  H5Fclose(h_file);
  
  printf("Wrote file: cosmo_points.hdf5\n");

  return 0;
}
