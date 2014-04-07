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

#include "node.h++"
using namespace libocn;

node::node(const std::string& name)
    : _name(name),
      _paths()
{
}

void node::add_path(std::shared_ptr<path> p)
{
    _paths[p->d()->name()] = p;
}

std::vector<std::shared_ptr<path>> node::paths(void) const
{
    std::vector<std::shared_ptr<path>> out;

    for (const auto& pair : _paths)
        out.push_back(pair.second);

    return out;
}
