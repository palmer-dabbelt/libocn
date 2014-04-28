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

#ifndef LIBOCN__MESH_NETWORK_HXX
#define LIBOCN__MESH_NETWORK_HXX

#include "network.h++"
#include "node.h++"
#include "sizet_printf.h++"
#include <stdlib.h>
#include <map>
#include <functional>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

namespace libocn {
    /* This is a special sort of network that allows for the creation
     * of a mesh network.  The idea is that the user doesn't need to
     * specify a network configuration file but can instead simply
     * create one of these. */
    template<class node_t>
    class mesh_network : public network<node_t> {
    protected:
        typedef std::shared_ptr<node_t> node_ptr;
        typedef path<node_t> path_t;

    public:
        /* Mesh networks are entirely paramaterized by a rectangle
         * that bounds them.  Note that this also takes a function
         * that creates a node.  A sane default value for this
         * function is mesh_network::create_node() from below. */
        mesh_network(size_t x_min, size_t x_max,
                     size_t y_min, size_t y_max,
                     std::function<node_ptr(size_t, size_t)> f
            )
            : network<node_t>(build_mesh_network(x_min,
                                                 x_max,
                                                 y_min,
                                                 y_max,
                                                 f)
                )
            {
            }

        /* This is exactly the same as calling "mesh_network(0, xc, 0,
         * yc, f)" from above. */
        mesh_network(size_t x_count, size_t y_count,
                     std::function<node_ptr(size_t, size_t)> f
            )
            : network<node_t>(build_mesh_network(0,
                                                 x_count - 1,
                                                 0,
                                                 y_count - 1,
                                                 f)
                )
            {
            }

    public:
        /* This is the default node creation function for a mesh
         * network.  The general idea is that this allows users of
         * this class to create nodes in their own way -- for example,
         * they could create a subclass of node that's got some
         * extra-special information in there if they need it. */
        static node_ptr create_node(size_t x, size_t y)
            {
                char buffer[BUFFER_SIZE];
                snprintf(buffer, BUFFER_SIZE, "(" SIZET_FORMAT ", " SIZET_FORMAT ")", x, y);
                return std::make_shared<node_t>(buffer);
            }

    private:
        /* This is effectively the constructor, the actual constructor
         * functions are just wrappers for this. */
        static std::vector<node_ptr>
        build_mesh_network(size_t x_min, size_t x_max,
                           size_t y_min, size_t y_max,
                           std::function<node_ptr(size_t, size_t)> f)
            {
                std::vector<node_ptr> out;
                std::map<std::pair<size_t, size_t>, node_ptr> grid;

                /* First we just build a big list of every node in the
                 * system.  There's two accounting structures that get
                 * created: a vector of all nodes, and a map that
                 * allows me to quickly look up a node based on its
                 * position. */
                for (size_t y = y_min; y <= y_max; ++y) {
                    for (size_t x = x_min; x <= x_max; ++x) {
                        auto n = f(x, y);
                        out.push_back(n);
                        grid[std::make_pair(x, y)] = n;
                    }
                }

                /* Now that every node has been created we want to
                 * fill out the trivially-known routes. */
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

        static
        void add_map(std::vector<node_ptr>& out __attribute__((unused)),
                     std::map<std::pair<size_t, size_t>,
                              node_ptr>& grid,
                     size_t sx, size_t sy, size_t dx, size_t dy)
            {
                auto source_l = grid.find(std::make_pair(sx, sy));
                if (source_l == grid.end()) {
                    fprintf(stderr, "Unmapped source (" SIZET_FORMAT "," SIZET_FORMAT ")\n", sx, sy);
                    abort();
                }
                auto source = source_l->second;

                auto dest_l = grid.find(std::make_pair(dx, dy));
                if (dest_l == grid.end()) {
                    fprintf(stderr, "Unmapped dest (" SIZET_FORMAT "," SIZET_FORMAT ")\n", dx, dy);
                    abort();
                }
                auto dest = dest_l->second;

                source->add_path(std::make_shared<path_t>(source, dest));
            }
    };
}

#endif
