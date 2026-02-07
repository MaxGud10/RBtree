#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <optional>

#include "red_black_tree.hpp"

namespace Tree
{

template <typename KeyT>
class Print_tree 
{
    // pисует один узел (для Graphviz .dot)
    void emit_node_(std::ofstream& out,   const KeyT& key, 
                    Color          color, bool        is_root) const 
    {
        const char* fill = is_root ? "#5A5A5A"
                                   : (color == Color::red ? "#D85C5C" : "#BDBDBD");
        const char* font = is_root ? "white" : "black";

        out << key
            << " [shape=Mrecord, style=filled, fillcolor=\"" << fill
            << "\", fontcolor=\"" << font
            << "\", label=\"{ key: " << key << " }\" ];\n";
    }

    // рекурсивный обход и соединение узлов
    void emit_nil_(std::ofstream& out, const std::string& nil_id) const 
    {
        out << nil_id
            << " [shape=box, style=filled, fillcolor=\"#BDBDBD\", label=\"NULL\"];\n";
    }

public:
    void dump(const Tree::Red_black_tree<KeyT>& rb_tree,
              const std::string& dot_path     = "graphviz/file_graph.dot",
              const std::string& /*png_path*/ = "graphviz/tree_graph.png",
              bool /*auto_open*/ = true) const
    {
#ifndef CUSTOM_MODE_DEBUG
        (void)rb_tree;
        (void)dot_path;

        std::cerr << "[WARN] Print_tree works only with CUSTOM_MODE_DEBUG\n";
        return;
#else
        const auto dir = std::filesystem::path(dot_path).parent_path();
        if (!dir.empty())
            std::filesystem::create_directories(dir);

        std::ofstream out(dot_path);
        if (!out)
        {
            std::cerr << "[ERROR] can't open " << dot_path << "\n";
            return;
        }

        out << "digraph RB_tree {\n"
               "label = < Red-black tree >;\n"
               "bgcolor = \"#BAF0EC\";\n"
               "rankdir=TB;\n"
               "node  [shape=record, style=filled];\n"
               "edge  [color=black, arrowsize=0.8];\n";

        std::size_t nil_counter = 0;
        bool        saw_any     = false;

        std::optional<KeyT> root_key;

        rb_tree.debug_visit([&](const KeyT&         key,
                                Color               color,
                                std::optional<KeyT> parent,
                                std::optional<KeyT> left,
                                std::optional<KeyT> right)
        {
            (void)left; (void)right;
            saw_any = true;

            if (!parent) root_key = key;
        });

        if (!saw_any)
        {
            out << "empty_tree [label=\"EMPTY TREE\", shape=box, style=filled, fillcolor=\"#CCCCCC\"];\n";
            out << "}\n";
            return;
        }

        rb_tree.debug_visit([&](const KeyT&         key,
                                Color               color,
                                std::optional<KeyT> parent,
                                std::optional<KeyT> left,
                                std::optional<KeyT> right)
        {
            const bool is_root = root_key && (key == *root_key);
            emit_node_(out, key, color, is_root);

            if (left)
                out << key << " -> " << *left << ";\n";

            else
            {
                const std::string nil_id = "nilL_" + std::to_string(nil_counter++);
                emit_nil_(out, nil_id);
                out << key << " -> " << nil_id << ";\n";
            }

            if (right)
                out << key << " -> " << *right << ";\n";

            else
            {
                const std::string nil_id = "nilR_" + std::to_string(nil_counter++);
                emit_nil_(out, nil_id);
                out << key << " -> " << nil_id << ";\n";
            }

            (void)parent; 
        });

        out << "}\n";
        out.close();
#endif
    }
};

} // namespace Tree
