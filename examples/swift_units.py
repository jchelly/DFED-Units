#!/bin/env python3
#
# Functions to convert between SWIFT unit metadata and unyt units
#

import unyt as u
import collections

def units_from_attributes(dset):
    """
    Create an astropy unit object from the attributes attached to a HDF5 dataset
    """
    cgs_factor = dset.attrs["Conversion factor to CGS (not including cosmological corrections)"][0]
    U_I = dset.attrs["U_I exponent"][0]
    U_L = dset.attrs["U_L exponent"][0]
    U_M = dset.attrs["U_M exponent"][0]
    U_T = dset.attrs["U_T exponent"][0]
    U_t = dset.attrs["U_t exponent"][0]
    return cgs_factor * (u.A**U_I) * (u.cm**U_L) * (u.g**U_M) * (u.K**U_T) * (u.s**U_t) 

def read_quantity(dset):
    """
    Read a dataset and return an astropy Quantity with unit information
    """
    data  = dset[...]
    units = units_from_attributes(dset)
    return u.unyt_array(data, units=units, dtype=data.dtype)
