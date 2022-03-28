/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2012 Pedro Gonnet (pedro.gonnet@durham.ac.uk),
 *                    Matthieu Schaller (matthieu.schaller@durham.ac.uk)
 * Copyright (c) 2015 Peter W. Draper (p.w.draper@durham.ac.uk)
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

#ifndef SWIFT_TOOL_H
#define SWIFT_TOOL_H

#include "error.h"

char *trim_leading(char *s);
char *trim_trailing(char *s);
char *trim_both(char *s);

void safe_checkdir(const char *dir, int create);

#define check_snprintf(s, n, format, ...)                   \
  do {                                                      \
    int _len = snprintf(s, n, format, __VA_ARGS__);         \
    if ((_len < 0) || (_len >= n))                          \
      error("truncation of string with format %s", format); \
  } while (0)

#endif /* SWIFT_TOOL_H */
