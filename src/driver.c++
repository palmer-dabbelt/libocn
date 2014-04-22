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
#include <libocn/dmesh_network.h++>
#include <libocn/crossbar_network.h++>
#include <libocn/plain_node.h++>
#include <libocn/sizet_printf.h++>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char **argv)
{
    if ((argc < 2) || (strcmp(argv[1], "--help") == 0)) {
        printf("%s: Compute all shortest paths for a network\n", argv[0]);
        printf("\t--mesh <width> <height>: A mesh network\n");
        printf("\t--dmesh <width> <height>: A DREAMER-style mesh, 1 offset\n");
        return (argc == 2) ? 0 : 1;
    }

    if ((argc < 2) || (strcmp(argv[1], "--version") == 0)) {
        printf("%s\n", PCONFIGURE_VERSION);
        return 0;
    }

    /* This can be constructed in a number of different ways, but
     * needs to persist below. */
    std::shared_ptr<libocn::network<libocn::plain_node>> network = NULL;

    if ((argc == 4) && (strcmp(argv[1], "--mesh") == 0)) {
        network = std::make_shared<libocn::mesh_network<libocn::plain_node>>(
            atoi(argv[2]),
            atoi(argv[3]),
            libocn::mesh_network<libocn::plain_node>::create_node
            );
    }

    if ((argc == 4) && (strcmp(argv[1], "--dmesh") == 0)) {
        network = std::make_shared<libocn::dmesh_network<libocn::plain_node>>(
            atoi(argv[2]),
            atoi(argv[3]),
            libocn::mesh_network<libocn::plain_node>::create_node
            );
    }

    if ((argc == 3) && (strcmp(argv[1], "--crossbar") == 0)) {
        network = std::make_shared<libocn::crossbar_network<libocn::plain_node>>(
            atoi(argv[2]),
            libocn::crossbar_network<libocn::plain_node>::create_node
            );
    }

    if ((argc == 3) && (strcmp(argv[1], "--file") == 0)) {
        network = std::make_shared<libocn::network<libocn::plain_node>>(
            argv[2]
            );
    }

    if (network == NULL) {
        exit(1);
    }

#if defined(SHORTEST_PATHS)
    for (const auto& node : network->nodes()) {
        for (const auto& path : node->paths()) {
            printf("\"%s\" -> \"%s\": " SIZET_FORMAT "\n",
                   path->s()->name().c_str(),
                   path->d()->name().c_str(),
                   path->cost()
                );
        }
    }
#elif defined(NEIGHBORS)
    for (const auto& node : network->nodes()) {
        for (const auto& path : node->outgoing_neighbors()) {
            printf("\"%s\" " SIZET_FORMAT " -> \"%s\" " SIZET_FORMAT ": " SIZET_FORMAT "\n",
                   path->s()->name().c_str(),
                   path->s()->port_number_out(path->d()),
                   path->d()->name().c_str(),
                   path->d()->port_number_in(path->s()),
                   path->cost()
                );
        }
    }
#elif defined(NODE_LIST)
    for (const auto& node : network->nodes()) {
        printf("%s\n", node->name().c_str());
    }
#else
#error "Define some sort of driver mode"
#endif

    return 0;
}
