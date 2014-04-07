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

#include "network.h++"
using namespace libocn;

static std::map<std::string, std::shared_ptr<node>>
build_name_map(const std::vector<std::shared_ptr<node>>& nodes);

network::network(const std::vector<std::shared_ptr<node>>& nodes)
    : _nodes(build_name_map(nodes))
{
}

std::vector<std::shared_ptr<node>> network::nodes(void) const
{
    std::vector<std::shared_ptr<node>> out;

    for (const auto& pair : _nodes)
        out.push_back(pair.second);

    return out;
}

std::map<std::string, std::shared_ptr<node>>
build_name_map(const std::vector<std::shared_ptr<node>>& nodes)
{
    std::map<std::string, std::shared_ptr<node>> out;

    for (const auto& node : nodes)
        out[node->name()] = node;

    return out;
}
