#!/bin/env python3
#
# Functions to convert between SWIFT unit metadata and astropy units
#

import astropy
import astropy.units as u
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

def write_unit_attributes(dset, unit):
    """
    Write unit attributes corresponding to an astropy unit object
    """
    cgs_factor = unit.cgs.scale
    powers = collections.defaultdict(lambda: 0)
    for base, power in zip(unit.cgs.bases, unit.cgs.powers):
        if base == u.cm:
            powers["U_L"] = power
        elif base == u.g:
            powers["U_M"] = power
        elif base == u.A:
            powers["U_I"] = power
        elif base == u.K:
            powers["U_T"] = power
        elif base == u.s:
            powers["U_t"] = power
    dset.attrs["Conversion factor to CGS (not including cosmological corrections)"] = [cgs_factor,]
    dset.attrs["U_I exponent"] = [powers["U_I"],]
    dset.attrs["U_L exponent"] = [powers["U_L"],]
    dset.attrs["U_M exponent"] = [powers["U_M"],]
    dset.attrs["U_T exponent"] = [powers["U_T"],]
    dset.attrs["U_t exponent"] = [powers["U_t"],]
    dset.attrs["Astropy unit string representation"] = unit.to_string()

def read_quantity(dset):
    """
    Read a dataset and return an astropy Quantity with unit information
    """
    data  = dset[...]
    units = units_from_attributes(dset)
    return astropy.units.Quantity(data, unit=units, dtype=data.dtype)
