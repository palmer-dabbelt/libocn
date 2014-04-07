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

namespace libocn {
    /* This is a special sort of network that allows for the creation
     * of a mesh network.  The idea is that the user doesn't need to
     * specify a network configuration file but can instead simply
     * create one of these. */
    class mesh_network : public network {
    public:
        /* Mesh networks are entirely paramaterized by their */
        mesh_network(size_t x_min, size_t x_max, size_t y_min, size_t y_max);

        /* This is exactly the same as calling "mesh_network(0, xc, 0,
         * yc)".*/
        mesh_network(size_t x_count, size_t y_count);
    };
}

#endif
