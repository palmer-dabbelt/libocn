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

#ifndef LIBOCN__CMESH_NETWORK_HXX
#define LIBOCN__CMESH_NETWORK_HXX

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
     * of a cmesh network.  The idea is that the user doesn't need to
     * specify a network configuration file but can instead simply
     * create one of these. */
    template<class node_t>
    class cmesh_network : public network<node_t> {
    protected:
        typedef std::shared_ptr<node_t> node_ptr;
        typedef path<node_t> path_t;

    public:
        /* A concentrated mesh network consists of a bunch crossbars
         * that are connected via a mesh.  Note that this also takes a
         * function that creates a node.  A sane default value for
         * this function is cmesh_network::create_node() from
         * below. */
        cmesh_network(size_t x, size_t y, size_t count,
                      std::function<node_ptr(size_t, size_t, size_t)> f)
            : network<node_t>(build_cmesh_network(x-1, y-1, count, f))
            {
            }

    public:
        /* This is the default node creation function for a cmesh
         * network.  The general idea is that this allows users of
         * this class to create nodes in their own way -- for example,
         * they could create a subclass of node that's got some
         * extra-special information in there if they need it. */
        static node_ptr create_node(size_t x, size_t y, size_t b)
            {
                char buffer[BUFFER_SIZE];
                snprintf(buffer, BUFFER_SIZE, "(" SIZET_FORMAT "," SIZET_FORMAT ")." SIZET_FORMAT, x, y, b);
                return std::make_shared<node_t>(buffer);
            }

    private:
        /* This is effectively the constructor, the actual constructor
         * functions are just wrappers for this. */
        static std::vector<node_ptr>
        build_cmesh_network(size_t x_max, size_t y_max, size_t count,
                            std::function<node_ptr(size_t, size_t, size_t)> f)
            {
                size_t x_min = 0, y_min = 0;

                std::vector<node_ptr> out;
                std::map<std::pair<size_t, size_t>, node_ptr> grid;

                /* First we build up all the crossbar groups.  We keep
                 * track of a single node in each crossbar that acts
                 * as the routing node. */
                for (size_t x = x_min; x <= x_max; ++x) {
                    for (size_t y = y_min; y <= y_max; ++y) {
                        std::vector<node_ptr> crossbar;

                        for (size_t i = 0; i < count; ++i) {
                            crossbar.push_back(f(x, y, i));
                            out.push_back(crossbar[i]);
                        }

                        for (size_t i = 0; i < count; ++i) {
                            for (size_t j = 0; j < count; ++j) {
                                if (i == j)
                                    continue;

                                auto s = crossbar[i];
                                auto d = crossbar[j];
                                s->add_path(std::make_shared<path_t>(s, d));
                            }
                        }

                        grid[std::make_pair(x, y)] = crossbar[0];
                    }
                }

                /* Now we just need to connect the crossbar switches
                 * together. */
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
