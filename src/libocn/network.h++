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

#ifndef LIBOCN__NETWORK_HXX
#define LIBOCN__NETWORK_HXX

#include "node.h++"
#include "path.h++"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

namespace libocn {
    /* This is the top-level OCN class: it stores a single network.
     * Note that while there's a lot of shared pointers inside here I
     * need to use some weak pointers to avoid circular references
     * between nodes -- essentially what this means is that if you're
     * doing any computation on the network you want to keep around a
     * reference to that network. */
    template<class node_t> class network {
    protected:
        typedef std::shared_ptr<node_t> node_ptr;
        typedef path<node_t> path_t;

    private:
        /* This stores the list of every node in the network. */
        std::vector<node_ptr> _node_list;
        std::unordered_map<std::string, node_ptr> _nodes;

    public:
        /* This constructor will probably only be useful if you're a
         * subclass of a network that aims to avoid parsing
         * configuration files. */
        network(const std::vector<node_ptr>& nodes)
            : _node_list(nodes),
              _nodes(build_name_map(nodes))
            {
            }

        /* This constructor reads a file to produce a list of nodes
         * along with their neighbors. */
        network(const std::string& filename)
            : _node_list(read_file(filename)),
              _nodes(build_name_map(_node_list))
            {
            }

        /* Returns a list that contains every node in this network. */
        std::vector<node_ptr> nodes(void) const { return _node_list; }

    private:
        /* This is used by the constructor to convert a node list to a
         * map of nodes. */
        static std::unordered_map<std::string, node_ptr>
        build_name_map(const std::vector<node_ptr>& nodes)
            {
                std::unordered_map<std::string, node_ptr> out;

                for (const auto& node : nodes)
                    out[node->name()] = node;

                return out;
            }

        /* Reads a file (by path) to produce a list of nodes. */
        static std::vector<node_ptr> read_file(const std::string& path)
            {
                std::vector<node_ptr> out;
                std::unordered_map<std::string, node_ptr> name_map;
                size_t line_num = 0;
                FILE *f = fopen(path.c_str(), "r");
                char line[BUFFER_SIZE];

                while (fgets(line, BUFFER_SIZE, f) != NULL) {
                    line_num++;

                    /* This signifies a comment character. */
                    if (line[0] == '#')
                        continue;

                    /* Now we parse the string...  Here '%[^\"]' means
                     * a string that ends with '"'. */
                    char source[BUFFER_SIZE], dest[BUFFER_SIZE];
                    int source_port, dest_port, cost;
                    int scanned = sscanf(line,
                                         "\"%[^\"]\" %d -> \"%[^\"]\" %d: %d",
                                         source, &source_port,
                                         dest, &dest_port,
                                         &cost);
                    if (scanned != 5) {
                        fprintf(stderr, "Unable to parse line: '%s'\n", line);
                        fprintf(stderr, "  Read %d tokens\n", scanned);

                        switch (scanned) {
                        case 5:
                            fprintf(stderr, "  5: %d\n", cost);
                        case 4:
                            fprintf(stderr, "  4: %d\n", dest_port);
                        case 3:
                            fprintf(stderr, "  3: '%s'\n", dest);
                        case 2:
                            fprintf(stderr, "  2: %d\n", source_port);
                        case 1:
                            fprintf(stderr, "  1: '%s'\n", source);
                        }

                        abort();
                    }

                    /* At this point we have the parsed file, so we
                     * just need to add this to the big list.  First
                     * we make sure that these nodes have already
                     * added to the list of nodes we know about. */
                    auto add_node = [&out, &name_map]
                        (const std::string& s) -> node_ptr
                        {
                            auto l = name_map.find(s);
                            if (l == name_map.end()) {
                                auto n = std::make_shared<node_t>(s);
                                name_map[s] = n;
                                out.push_back(n);
                                return n;
                            }

                            return l->second;
                        };
                    auto s = add_node(source);
                    auto d = add_node(dest);

                    /* Create a direct path between these two
                     * nodes. */
                    auto p = std::make_shared<path_t>(s, d, cost);
                    s->add_path(p, source_port, dest_port);
                }

                fclose(f);
                return out;
            }
    };
}

#endif
