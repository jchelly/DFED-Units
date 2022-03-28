/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2012 Pedro Gonnet (pedro.gonnet@durham.ac.uk),
 *                    Matthieu Schaller (matthieu.schaller@durham.ac.uk).
 *               2015 Peter W. Draper (p.w.draper@durham.ac.uk)
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
#ifndef SWIFT_ERROR_H
#define SWIFT_ERROR_H

/* Config parameters. */
#include "../config.h"

/* Some standard headers. */
#include <stdio.h>
#include <stdlib.h>

/* Abort with core dump */
#define swift_abort(errcode) abort()

/**
 * @brief Error macro. Prints the message given in argument and aborts.
 *
 */
#define error(s, ...)                                                      \
  ({                                                                       \
    fflush(stdout);                                                        \
    fprintf(stderr, "%s:%s():%i: " s "\n",                                 \
            __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);              \
    memdump(engine_rank);                                                  \
    swift_abort(1);                                                        \
  })

/**
 * @brief Macro to print a localized message with variable arguments.
 *
 */
#define message(s, ...)                                                 \
  ({                                                                    \
    printf("%s: " s "\n", __FUNCTION__,                                 \
           ##__VA_ARGS__);                                              \
  })

/**
 * @brief Macro to print a localized warning message with variable arguments.
 *
 * Same as message(), but this version prints to the standard error and is
 * flushed immediately.
 *
 */
#define warning(s, ...)                                                     \
  ({                                                                        \
    fprintf(stderr, "%s: WARNING: " s "\n",                                 \
            __FUNCTION__, ##__VA_ARGS__);                                   \
  })

#endif /* SWIFT_ERROR_H */
