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

#ifndef LIBOCN__CROSSBAR_NETWORK_HXX
#define LIBOCN__CROSSBAR_NETWORK_HXX

#include "crossbar_network.h++"

namespace libocn {
    /* This is the special sort of mesh network that exists on
     * DREAMER, which is essentially a mesh network that's offset by 1
     * in the X direction. */
    template<class node_t>
    class crossbar_network : public network<node_t> {
        typedef std::shared_ptr<node_t> node_ptr;
        typedef path<node_t> path_t;

    public:
        /* Creates a DREAMER mesh network of the given size. */
        crossbar_network(size_t count,
                      std::function<node_ptr(size_t)> f)
            : network<node_t>(build_crossbar_network(count, f))
            {
            }

    public:
        /* This is the default node creation function for a mesh
         * network.  The general idea is that this allows users of
         * this class to create nodes in their own way -- for example,
         * they could create a subclass of node that's got some
         * extra-special information in there if they need it. */
        static node_ptr create_node(size_t x)
            {
                char buffer[BUFFER_SIZE];
                snprintf(buffer, BUFFER_SIZE, SIZET_FORMAT, x);
                return std::make_shared<node_t>(buffer);
            }

    private:
        /* This is pretty much just the constructor, I just want it
         * all created before I pass it up. */
        static std::vector<node_ptr>
        build_crossbar_network(size_t count,
                           std::function<node_ptr(size_t)> f)
            {
                std::vector<node_ptr> nodes;
                for (size_t i = 0; i < count; ++i)
                    nodes.push_back(f(i));

                for (const auto& s: nodes)
                    for (const auto& d: nodes)
                        if (s->name() != d->name())
                            s->add_path(std::make_shared<path_t>(s, d));

                return nodes;
            }

    };
}

#endif
