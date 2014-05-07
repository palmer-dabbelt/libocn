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

#ifndef LIBOCN__CROSSMESH_NETWORK_HXX
#define LIBOCN__CROSSMESH_NETWORK_HXX

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
     * of a crossmesh network.  The idea is that the user doesn't need to
     * specify a network configuration file but can instead simply
     * create one of these. */
    template<class node_t>
    class crossmesh_network : public network<node_t> {
    protected:
        typedef std::shared_ptr<node_t> node_ptr;
        typedef path<node_t> path_t;

    public:
        /* A concentrated mesh network consists of a bunch crossbars
         * that are connected via a mesh.  Note that this also takes a
         * function that creates a node.  A sane default value for
         * this function is crossmesh_network::create_node() from
         * below. */
        crossmesh_network(size_t x, size_t y, size_t count,
                      std::function<node_ptr(size_t, size_t, size_t)> f)
            : network<node_t>(build_crossmesh_network(x, y, count, f))
            {
            }

    public:
        /* This is the default node creation function for a crossmesh
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
        build_crossmesh_network(size_t X, size_t Y, size_t count,
                            std::function<node_ptr(size_t, size_t, size_t)> f)
            {
                size_t side = floor(sqrt(count));
                if ((side * side) != count) {
                    fprintf(stderr, "Building non-square crossbar\n");
                    abort();
                }

                std::vector<node_ptr> out;
                std::map<std::pair<size_t, size_t>, node_ptr> grid;
                std::map<std::pair<size_t, size_t>, size_t> cbc;
                std::multimap<std::pair<size_t, size_t>, node_ptr> cb;

                /* First just make every node so we can reference them
                 * later. */
                for (size_t y = 0; y < Y * side; ++y) {
                    for (size_t x = 0; x < X * side; ++x) {
                        auto cx = x / side;
                        auto cy = y / side;
                        if (cbc.find(std::make_pair(cx, cy)) == cbc.end())
                            cbc[std::make_pair(cx, cy)] = 0;
                        auto c = cbc.find(std::make_pair(cx, cy))->second;

                        auto n = f(cx, cy, c);
                        out.push_back(n);
                        grid[std::make_pair(x, y)] = n;
                        cb.insert(std::make_pair(std::make_pair(cx, cy), n));

                        c++;
                        cbc[std::make_pair(cx, cy)] = c;
                    }
                }

                /* Now build a mesh network, the crossmesh is strictly
                 * more connected than the mesh is. */
                for (size_t y = 0; y < Y * side; ++y) {
                    for (size_t x = 0; x < X * side; ++x) {
                        if (x > 0)
                            add_map(grid, x, y, x-1, y+0);
                        if (x < (X*side)-1)
                            add_map(grid, x, y, x+1, y+0);
                        if (y > 0)
                            add_map(grid, x, y, x+0, y-1);
                        if (y < (Y*side)-1)
                            add_map(grid, x, y, x+0, y+1);
                    }
                }

                /* Now link the internal nodes with crossbars. */
                for (const auto& ap: cb) {
                    for (const auto& bp: cb) {
                        if (ap.first != bp.first)
                            continue;

                        auto a = ap.second;
                        auto b = bp.second;

                        if (a->name() == b->name())
                            continue;

                        if (a->is_neighbor(b) == true)
                            continue;

                        auto path = std::make_shared<path_t>(a, b);
                        a->add_path(path);
                    }
                }

                return out;
            }

        static
        void add_map(std::map<std::pair<size_t, size_t>, node_ptr>& grid,
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
