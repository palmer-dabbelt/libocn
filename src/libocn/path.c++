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

#include "path.h++"
using namespace libocn;

/* Converts an array of shared pointers into an array of weak pointers. */
static std::vector<std::weak_ptr<node>>
weaken(const std::vector<std::shared_ptr<node>>& strong);

path::path(std::shared_ptr<node>& source, std::shared_ptr<node>& dest)
    : _s(source),
      _d(dest),
      _cost(1),
      _steps()
{
}

path::path(std::shared_ptr<node> source, std::shared_ptr<node> dest,
           const std::vector<std::shared_ptr<node>>& steps, double cost)
    : _s(source),
      _d(dest),
      _cost(cost),
      _steps(weaken(steps))
{
}

std::vector<std::shared_ptr<node>> path::steps(void) const
{
    std::vector<std::shared_ptr<node>> steps;

    for (const auto& step : _steps)
        steps.push_back(step.lock());

    return steps;
}

std::shared_ptr<path> path::cat(const std::shared_ptr<path>& that)
{
    /* Concatonate the two step lists into a single large one. */
    std::vector<std::shared_ptr<node>> step_list;
    for (const auto& step : this->steps())
        step_list.push_back(step);
    step_list.push_back(this->d());
    for (const auto& step : that->steps())
        step_list.push_back(step);

    return std::make_shared<path>(this->s(),
                                  that->d(),
                                  step_list,
                                  this->cost() + that->cost());
}

std::vector<std::weak_ptr<node>>
weaken(const std::vector<std::shared_ptr<node>>& strong)
{
    std::vector<std::weak_ptr<node>> out;

    for (const auto& ptr : strong)
        out.push_back(ptr);

    return out;
}
