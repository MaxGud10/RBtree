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
    using NodeT = Tree::detail::Node<KeyT>;

    // draws a single node (for Graphviz .dot)
    void emit_node_(const NodeT &node, std::ofstream &out, bool is_root) const
    {
        const char* fill = is_root ? "#5A5A5A"
                                   : (node.color == Color::red ? "#D85C5C" : "#BDBDBD");
        const char* font = is_root ? "white" : "black";

        out << node.key_
            << " [shape=Mrecord, style=filled, fillcolor=\"" << fill
            << "\", fontcolor=\"" << font
            << "\", label=\"{ key: " << node.key_ << " }\" ];\n";
    }

    // recursive traversal and node connection
    void emit_nil_(std::ofstream &out, const std::string &nil_id) const
    {
        out << nil_id
            << " [shape=box, style=filled, fillcolor=\"#BDBDBD\", label=\"NULL\"];\n";
    }

    void print_(const NodeT   &node,
                std::ofstream &out,
                std::size_t   &nil_counter,
                bool          is_root = false) const
    {
        emit_node_(node, out, is_root);

        if (node.left_)
        {
            out << node.key_ << " -> " << node.left_->key_ << ";\n";
            print_(*node.left_, out, nil_counter, false);
        }
        else
        {
            const std::string nil_id = "nilL_" + std::to_string(nil_counter++);
            emit_nil_(out, nil_id);
            out << node.key_ << " -> " << nil_id << ";\n";
        }

        if (node.right_)
        {
            out << node.key_ << " -> " << node.right_->key_ << ";\n";
            print_(*node.right_, out, nil_counter, false);
        }
        else
        {
            const std::string nil_id = "nilR_" + std::to_string(nil_counter++);
            emit_nil_(out, nil_id);
            out << node.key_ << " -> " << nil_id << ";\n";
        }
    }

public:
    void dump(const Tree::Red_black_tree<KeyT> &rb_tree,
              const std::string &dot_path     = "graphviz/file_graph.dot",
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

        const NodeT* root = rb_tree.debug_root();
        if (!root)
        {
            out << "empty_tree [label=\"EMPTY TREE\", shape=box, style=filled, fillcolor=\"#CCCCCC\"];\n";
            out << "}\n";
            return;
        }

        std::size_t nil_counter = 0;
        print_(*root, out, nil_counter, true);

        out << "}\n";
        out.close();
#endif
    }
};

} // namespace Tree
