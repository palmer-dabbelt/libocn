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

#include "mesh_network.h++"
#include <map>
#include <stdlib.h>
using namespace libocn;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* This is effectively the constructor, the actual constructor
 * functions are just wrappers for this. */
static std::vector<node_ptr_t>
build_mesh_network(size_t x_min, size_t x_max, size_t y_min, size_t y_max,
                   std::function<node_ptr_t(size_t, size_t)> f);

/* A helper function for build_mesh_network(), this adds a route to
 * from the source node to the destination node. */
static
void add_map(std::vector<std::shared_ptr<node>>& out,
             std::map<std::pair<size_t, size_t>, std::shared_ptr<node>>& grid,
             size_t sx, size_t sy, size_t dx, size_t dy);

mesh_network::mesh_network(size_t x, size_t X, size_t y, size_t Y,
                           std::function<node_ptr_t(size_t, size_t)> f)
    : network(build_mesh_network(x, X, y, Y, f))
{
}

mesh_network::mesh_network(size_t x, size_t y,
                           std::function<node_ptr_t(size_t, size_t)> f)
    : network(build_mesh_network(0, x - 1, 0, y - 1, f))
{
}

std::shared_ptr<node> mesh_network::create_node(size_t x, size_t y)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "(%lu,%lu)", x, y);
    return std::make_shared<node>(buffer);
}

std::vector<std::shared_ptr<node>>
build_mesh_network(size_t x_min, size_t x_max, size_t y_min, size_t y_max,
                   std::function<node_ptr_t(size_t, size_t)> f)
{
    std::vector<std::shared_ptr<node>> out;
    std::map<std::pair<size_t, size_t>, std::shared_ptr<node>> grid;

    /* First we just build a big list of every node in the system.
     * There's two accounting structures that get created: a vector of
     * all nodes, and a map that allows me to quickly look up a node
     * based on its position. */
    for (size_t x = x_min; x <= x_max; ++x) {
        for (size_t y = y_min; y <= y_max; ++y) {
            auto n = f(x, y);
            out.push_back(n);
            grid[std::make_pair(x, y)] = n;
        }
    }

    /* Now that every node has been created we want to fill out the
     * trivially-known routes. */
    for (size_t x = x_min; x <= x_max; ++x) {
        for (size_t y = y_min; y <= y_max; ++y) {
            if (x > x_min)
                add_map(out, grid, x, y, x-1, y+0);
            if (x < x_max)
                add_map(out, grid, x, y, x+1, y+0);
            if (y > y_min)
                add_map(out, grid, x, y, x+0, y-1);
            if (y < y_max)
                add_map(out, grid, x, y, x+0, y+1);
        }
    }

    return out;
}

void add_map(std::vector<std::shared_ptr<node>>& out __attribute__((unused)),
             std::map<std::pair<size_t, size_t>, std::shared_ptr<node>>& grid,
             size_t sx, size_t sy, size_t dx, size_t dy)
{
    auto source_l = grid.find(std::make_pair(sx, sy));
    if (source_l == grid.end()) {
        fprintf(stderr, "Mapping unmapped source (%lu, %lu)\n", sx, sy);
        abort();
    }
    auto source = source_l->second;

    auto dest_l = grid.find(std::make_pair(dx, dy));
    if (dest_l == grid.end()) {
        fprintf(stderr, "Mapping unmapped dest (%lu, %lu)\n", dx, dy);
        abort();
    }
    auto dest = dest_l->second;

    source->add_path(std::make_shared<path>(source, dest));
}

