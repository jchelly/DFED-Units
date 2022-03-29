#!/bin/env python3

import h5py
import swift_units
import unyt
import matplotlib.pyplot as plt

# Read in the random points from the cosmology.c example
filename="cosmo_points.hdf5"
with h5py.File(filename, "r") as infile:
    pos_mpc = swift_units.read_quantity(infile["Coordinates"])

# Convert positions to millions of lightyears
pos_mlyr = pos_mpc.to(1e6*unyt.light_year)

# Plot the points
plt.plot(pos_mlyr[:,0], pos_mlyr[:,1], "k,")
plt.xlabel("x [Mlyr]")
plt.ylabel("y [Mlyr]")
plt.show()
