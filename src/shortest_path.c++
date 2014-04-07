/*
 * Copyright (C) 2014 Palmer Dabbelt
 *   <palmer.dabbelt@eecs.berkeley.edu>
 *
 * This file is part of libocn.
 *
 * libocn is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libocn is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libocn.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "version.h"
#include <libocn/mesh_network.h++>
#include <stdio.h>
#include <string.h>

int main(int argc, const char **argv)
{
    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        exit(1);
    }

    if ((argc < 2) || (strcmp(argv[1], "--verbose") == 0)) {
        exit(1);
    }

    /* This can be constructed in a number of different ways, but
     * needs to persist below. */
    std::shared_ptr<libocn::network> network = NULL;

    if ((argc == 4) && (strcmp(argv[1], "--mesh") == 0)) {
        network = std::make_shared<libocn::mesh_network>(atoi(argv[2]),
                                                         atoi(argv[3]));
    }

    if (network == NULL) {
        exit(1);
    }

    for (const auto& node : network->nodes()) {
        for (const auto& path : node->paths()) {
            printf("%s -> %s: %f\n",
                   path->s()->name().c_str(),
                   path->d()->name().c_str(),
                   path->cost()
                );
        }
    }

    return 0;
}
