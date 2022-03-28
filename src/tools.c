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

/* Config parameters. */
#include "../config.h"

/* Some standard headers. */
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/* This object's header. */
#include "tools.h"

/* Local includes. */
#include "error.h"

/**
 * @brief trim leading white space from a string.
 *
 * Returns pointer to first character.
 *
 * @param s the string.
 * @result the result.
 */
char *trim_leading(char *s) {
  if (s == NULL || strlen(s) < 2) return s;
  while (isspace(*s)) s++;
  return s;
}

/**
 * @brief trim trailing white space from a string.
 *
 * Modifies the string by adding a NULL to the end.
 *
 * @param s the string.
 * @result the result.
 */
char *trim_trailing(char *s) {
  if (s == NULL || strlen(s) < 2) return s;
  char *end = s + strlen(s) - 1;
  while (isspace(*end)) end--;
  *(end + 1) = '\0';
  return s;
}

/**
 * @brief trim leading and trailing white space from a string.
 *
 * Can modify the string by adding a NULL to the end.
 *
 * @param s the string.
 * @result the result.
 */
char *trim_both(char *s) {
  if (s == NULL || strlen(s) < 2) return s;
  return trim_trailing(trim_leading(s));
}

/**
 * @brief safe check directory existence and creation.
 *
 * Doesn't try to re-create an existing directory and makes sure that it can
 * have files created in it. If the directory doesn't exist and we want to
 * create it then we try to do that with an appropriate mask that should
 * honour the users umask. If the directory is something odd like a broken
 * link then that is dereferenced so should also result in an error, since
 * it doesn't exist and cannot be created. Finally we check that an existing
 * file with this name is indeed a directory.
 *
 * @param dir the directory we need to exist.
 * @param create create if not already exists, otherwise non-existence is an
 *               error.
 */
void safe_checkdir(const char *dir, int create) {

  if (access(dir, W_OK | X_OK) != 0) {

    /* A file with this name and the minimally correct permissions does not
     * exist. */
    if (create) {
      if (mkdir(dir, 0777) != 0)
        error("Failed to create directory %s (%s)", dir, strerror(errno));
    } else {
      error("directory %s does not exist or cannot be used (%s)", dir,
            strerror(errno));
    }
  } else {

    /* Exists and has the minimally correct permissions, is this a
     * directory? */
    struct stat sb;
    if (stat(dir, &sb) != 0) {
      /* Weird error, can stat all existing files. */
      error("Failed to stat directory %s (%s)", dir, strerror(errno));
    } else if ((sb.st_mode & S_IFMT) != S_IFDIR) {
      error("%s is not a directory, cannot use or create.", dir);
    }
  }

  /* Success. */
  return;
}
