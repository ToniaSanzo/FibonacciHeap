/**
    Copyright 2020 Tonia Sanzo © 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.


    Title: FibonacciHeap
    File: FibonacciHeap.h
    Author: Tonia Sanzo (sanzo.tonia@gmail.com)

    A fibonacci heap implemented as described by Kevin Wayne
    (wayne@cs.princeton.edu)
    www.cs.princeton.edu/~wayne/teaching/fibonacci-heap.pdf

    Fibonacci heap was invented by Fredman and Tarjan, 1986.
    I did extensive QA on this program, but if you do discover
    a bug please contact me at <sanzo.tonia@gmail.com>

    ______________________________________________________________
    Priority Queues Performance Cost Summary:
    make_heap:         O(1)
    is_empty:          O(1)
    insert:            O(1)
    extract_min:       O(log(n))
    delete_min:        O(log(n))
    change_priority:   O(1)
    delete:            O(log(n))
    +, += [union]:     O(1)
    find_min:          O(1)

    (amortized)
    (n = number of elements in priority queue)
    ______________________________________________________________
*/
#pragma once
#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class fhNode {
/// <summary>
/// Node class, each node contains a value, and priority, and is
/// implemented in such a way to form a heap.
/// </summary>
/// <typeparam name="T">Generic object contained within this node</typeparam>
public:
    // Pointer to the Nodes parent.
    std::shared_ptr<fhNode<T>> parent = nullptr;

    // Collection of Nodes children.
    std::vector<std::shared_ptr<fhNode<T>>> children;

    // If this node has lost any children.
    bool marked = false;

    // The nodes priority.
    long long priority;

    // Value contained in this Node.
    T value;

    // Number of tree layers below this Node.
    unsigned degree = 0;

    fhNode(T value, long long priority) {
        /// <summary>
        /// Construct a Node with a set priority and value, initially set
        /// to be a heap of size 1.
        /// </summary>
        /// <param name="value">generic object contained by the node</param>
        /// <param name="priority">priority key value</param>
        this->value = value;
        this->priority = priority;
    }

    void print() {
        /// <summary>
        /// Print a Node's content, priority, and children.
        /// </summary>
        for (size_t i = 0; i < degree; i++) {
            std::cout << "\\____________   ";
        }
        std::cout << "Content: " << value << " Priority: " << priority << " Degree: " << degree 
                  << " Marked: " << marked << std::endl;
        for (auto &child : children) {
            std::cout << "\\____________[CHILD] ";
            child->print();
        }
        std::cout << std::endl;
    }

    std::shared_ptr<fhNode<T>> search(const T &key, const long long &priority) {
        /// <summary>
        /// Search a Node's tree structure for a specific Node, return a 
        /// pointer to the Node if its found otherwise false.
        /// </summary>
        /// <param name="key">Value contained in the target node.</param>
        /// <param name="priority">Priority of the target node.</param>
        /// <returns>Pointer to target node; otherwise a nullptr.</returns>
        std::shared_ptr<fhNode<T>> node;

        // Search the subtree for the key
        for (auto &child : children) {
            // If the child node matches the node being searched
            // for, return that node.
            if (child->value == key && child->priority == priority) {
                return child;
            }

            // Because of the property of heaps if the priority 
            // is greater than the target priority we can skip
            // looking through this subtree.
            if (child->priority > priority) {
                continue;
            }

            // If this child has children search them.
            if (child->children.size() != 0) {
                node = child->search(key, priority);
            }

            // If node is not the nullptr than the correct node
            // was found and should be returned.
            if (node.use_count() != 0) {
                return node;
            }
        }

        // After every element has been searched and a match has
        // not been found return the nullptr.
        return nullptr;
    }
};


template <typename T>
class FibonacciHeap {
/// <summary>
/// Fibonacci Heap, Similar to binomial heap, but less rigid, lazily 
/// defers consolidation until next delete_min.
/// </summary>
/// <typeparam name="T">Generic object contained within this collection.</typeparam>
private:
    // Pointer to the max_node.
    std::shared_ptr<fhNode<T>> min_node;

    // Collection of trees in the FibonacciHeap
    std::vector<std::shared_ptr<fhNode<T>>> roots;

    // Number of elements in the collection.
    std::size_t size = 0;

    // Vector of roots used to consolidate the trees, so they
    // all have unique degrees.
    std::vector<std::shared_ptr<fhNode<T>>> rank;

public:
    /*
    Default constructor, the FibonacciHeap is ininitialized to an
    empty collection.
    */
    FibonacciHeap() {}

    /*
    Copy constructior, the fibonacciHeap is initialized to the
    parameter.

    @parameter: copy (FibonacciHeap) - Fibonacci Heap to make this 
                                       instance a copy of.
    */
    FibonacciHeap(const FibonacciHeap<T> &copy) { 
        min_node = copy.min_node;
        roots = copy.roots;
        size = copy.size;
        rank = copy.rank;
    }

    void insert(const T &value, const long long &priority) {
        /// <summary>
        /// Insert a new Node into the fibonacci heap.
        /// </summary>
        /// <param name="value">Generic object contained by the node.</param>
        /// <param name="priority">Priority key value.</param>
        
        // First, allocate a node representing a singleton tree to
        // the heap.
        std::shared_ptr<fhNode<T>> new_node(
            new fhNode<T>(value, priority)
        );

        // Add the new_node to the collection as a singleton tree.
        roots.push_back(new_node);

        // Update min_node pointer if necessary.
        if (!min_node || min_node->priority > priority) {
            min_node = new_node;
        }

        // Increase the size of the collection.
        ++size;
    }

    void delete_min() {
        /// <summary>
        /// Delete min and consolidate trees so that no two roots have the
        /// same rank.
        /// </summary>

        // If the collection is empty prematurely exit the function.
        if (!min_node) { return; }

        // Meld the children into the root list
        for (auto &child : min_node->children) {
            child->parent = nullptr;
            roots.push_back(child);
        }
        min_node->children.clear();

        // Remove min_node from the root list, erase min_node, and
        // set a new minimum.
        auto it = std::find(roots.begin(), roots.end(), min_node);
        roots.erase(it);
        --size;
        set_min();
    
        // Consolidate the trees so that no two roots have the same 
        // rank.
        consolidate_tree();    
    }
    
    void consolidate_tree() {
        /// <summary>
        /// Function to consolidate the trees within the heap. After this 
        /// function no trees will have the same(rank / degree / number of tree
        /// layers).
        /// </summary>


        // Current root being checked
        size_t current_index = 0;

        // Current degree of the root being checked.
        unsigned current_degree = 0;

        // For each root add its degree to the rank, and combine
        // trees when necessary.
        while (current_index < roots.size()) {
            current_degree = roots[current_index]->degree;

            // Rank grows dynamically based on the trees degrees.
            if (current_degree >= rank.size()) {
                rank_grow(current_degree);
            }

            // If the rank does not have an element at the roots
            // degree add the root to the rank.
            if (rank[current_degree].use_count() == 0) {
                rank[current_degree] = roots[current_index];
                ++current_index;
            }
            else {
                // This rank has an element, so combine these trees.
                // The tree root is determined depending on which
                // root has lower priority.

                // If the tree root stored in rank has a lower 
                // priority than the current tree root.
                if (rank[current_degree]->priority < roots[current_index]->priority) {
                    // Update the new trees degree.
                    if (rank[current_degree]->degree <= roots[current_index]->degree) {
                        rank[current_degree]->degree = roots[current_index]->degree + 1;
                    }

                    // Combine the trees, and remove the larger
                    // priority tree from the vector.
                    rank[current_degree]->children.push_back(roots[current_index]);
                    roots[current_index]->parent = rank[current_degree];
                    auto it = std::find(roots.begin(), roots.end(), rank[current_degree]);
                    roots[current_index] = rank[current_degree];
                    rank[current_degree] = nullptr;
                    roots.erase(it);
                    --current_index;
                }

                // If the current tree root has a lower priority
                // than the tree root stored in rank.
                else {
                    // Update the new trees degree.
                    if(roots[current_index]->degree <= rank[current_degree]->degree){
                        roots[current_index]->degree = rank[current_degree]->degree + 1;
                    }

                    // Combine the trees, and remove the larger
                    // priority tree from the vector.
                    roots[current_index]->children.push_back(rank[current_degree]);
                    rank[current_degree]->parent = roots[current_index];
                    auto it = std::find(roots.begin(), roots.end(), rank[current_degree]);
                    roots.erase(it);
                    rank[current_degree] = nullptr;
                    --current_index;
                }
            }
        }

        clear_rank();
    }

    void rank_grow(unsigned degree) {
        /// <summary>
        /// Given a degree grow rank to match that degree
        /// </summary>
        
        // grow rank to the correct degree size
        for (size_t i = 0; i < degree - (rank.size() - 1); ++i) {
            rank.push_back(nullptr);
        }
    }

    void print_heap() { 
        /// <summary>
        /// Print the contents of this collection.
        /// </summary>
        if (min_node) { std::cout << "min_node: " << min_node->value << std::endl; }
        for (auto &root : roots) { root->print(); }
    }

    void set_min() {
        /// <summary>
        /// Set the min_node to the root with the lowest priority.
        /// </summary>
        min_node = nullptr;
        for (auto &root : roots) {
            if (!min_node || root->priority <= min_node->priority)
                min_node = root;
        }
    }

    void clear_rank() {
        /// <summary>
        /// Clear the contents of rank.
        /// </summary>
        for (auto &elem : rank) { elem = nullptr; }
    }

    std::size_t get_size() {
        /// <summary>
        /// Returns the number of elements contained in the
        /// FibonacciHeap.
        /// </summary>
        /// <returns>The number of elements contained in the
        /// FibonacciHeap.</returns>
        return size;
    }

    bool is_empty() {
        /// <summary>
        /// Returns true if there are no elements in the collection;
        /// otherwise false.
        /// </summary>
        /// <returns>True if there are no elements in the collection;
        /// otherwise false.</returns>
        return !(size); 
    }

    void change_priority(const T &key, const long long &old_priority, const long long &new_priority) {
        /// <summary>
        /// Change the priority of a key.
        /// </summary>
        /// <param name="key">Value contained by the target node.</param>
        /// <param name="old_priority">Old priority of the key.</param>
        /// <param name="new_priority">New priority of the key.</param>
        std::shared_ptr<fhNode<T>> parent_node, curr_node, child;

        // Find the node in the collection.
        curr_node = find(key, old_priority);

        // If the node being searched for was not found exit.
        if (!(curr_node.use_count())) { return; }

        // Update the curr_node's priority, and set the parent_node.
        curr_node->priority = new_priority;
        parent_node = curr_node->parent;

        // If the curr_nodes priority has a chance of being less than 
        // the parents priority.
        if (new_priority < old_priority) {
            // While the curr_node has a parent, and the parents
            // priority is greater than the current node's priority.
            while (parent_node.use_count() && (curr_node->priority < parent_node->priority)) {
                auto it = std::find(parent_node->children.begin(), parent_node->children.end(), curr_node);
                parent_node->children.erase(it);
                roots.push_back(curr_node);
                curr_node->parent = nullptr;
                curr_node->marked = false;
                set_degree(parent_node);
                mark_utility(parent_node);

                // Set curr_node and parent_node appropriately.
                curr_node = parent_node;
                parent_node = curr_node->parent;
            }
        }

        // If the curr_node's child has a chance of being less than 
        // the curr_node's priority.
        if (new_priority > old_priority) {

            for (size_t index = 0; index < curr_node->children.size(); ++index) {
                child = curr_node->children[index];

                // Make sure no child has lower priority than the
                // node's parent.
                if (child->priority < curr_node->priority) {
                    // Cut the tree rooted at child, meld into root
                    // list.
                    auto it = std::find(curr_node->children.begin(), curr_node->children.end(), child);
                    curr_node->children.erase(it);
                    roots.push_back(child);
                    child->parent = nullptr;
                    child->marked = false;
                    set_degree(curr_node);
                    mark_utility(curr_node);
                    index--;
                }
            }
        }

        // In case the min_nodes priority changed reset the min_node.
        set_min();
    }

    std::shared_ptr<fhNode<T>> find(const T& key, const long long& priority) {
        /// <summary>
        /// Given a key, finds and returns a pointer to that key.
        /// </summary>
        /// <param name="key">Target node.</param>
        /// <param name="priority">Current priority of target 
        /// node.</param>
        /// <returns>Pointer to target node;
        /// otherwise a nullptr.</returns>
        std::shared_ptr<fhNode<T>> node;
        
        // Search each tree for the key
        for (auto &root : roots) {
            // If the root node matches the node being searched
            // for, return that node.
            if (root->value == key && root->priority == priority) {
                return root;
            }

            // Because of the property of heaps if the priority 
            // is greater than the target priority we can skip
            // looking through this tree.
            if (root->priority > priority) {
                continue;
            }

            // If this root has children search them.
            if (root->children.size() != 0) {
                node = root->search(key, priority);
            }

            // If node is not the nullptr than the correct node
            // was found and should be returned.
            if (node.use_count() != 0) {
                return node;
            }
        }

        // After every element has been searched and a match has
        // not been found return the nullptr.
        return nullptr;
    }

    void set_degree(std::shared_ptr<fhNode<T>> &node) {
        /// <summary>
        /// Determine the degree of the current node, given it's children.
        /// Set the correct degree of each of the parents if necessary.
        /// </summary>
        /// <param name="node">Node having its 
        /// degree updated.</param>

        // determine the degree of the node before altering the node.
        unsigned prev_degree = node->degree;
        node->degree = 0;
        
        // Check each of the node's children and update the node's
        // degree when necessary.
        for (const auto &child : node->children) {
            if (child->degree >= node->degree) {
                node->degree = child->degree + 1;
            }
        }

        // If the node's degree has changed, and the node has a 
        // parent update the parent's degree when necessary.
        if (prev_degree != node->degree && node->parent.use_count() != 0) {
            set_degree(node->parent);
        }
    }

    std::shared_ptr<fhNode<T>> find_min() {
        /// <summary>
        /// Return the minimum node in the priority queue, without removing it.
        /// </summary>
        /// <returns>Pointer to the minimum 
        /// node in the collection.</returns>
        return min_node; 
    }

    void mark_utility(std::shared_ptr<fhNode<T>> &node) {
        /// <summary>
        /// Recursive utility function that will recurse through the tree and
        /// either mark or remove marked node's.
        /// </summary>
        /// <param name="node">Node being marked 
        /// or removed from
        /// the heap.</param>

        // Set parent_node equal to the node's parent.
        std::shared_ptr<fhNode<T>> parent_node = node->parent;

        // Exit mark_utility if the node is the root of the tree,
        // otherwise process the node.
        if(parent_node.use_count() != 0){

            // If the node is not marked, mark it and exit the 
            // function.
            if (!(node->marked)) {
                node->marked = true;
                return;
            }

            // Cut the tree rooted at node, meld it into the root 
            // list, set the parents degree than recursively call the
            // mark_utility function.
            auto it = std::find(parent_node->children.begin(), parent_node->children.end(), node);
            parent_node->children.erase(it);
            roots.push_back(node);
            node->parent = nullptr;
            node->marked = false;
            set_degree(parent_node);
            mark_utility(parent_node);
        }
    }

  std::shared_ptr<fhNode<T>> extract_min() {
        /// <summary>
        /// Return the minimum node in the collection, the minimum
        ///  node will be removed from the collection after this
        ///  method call.
        /// </summary>
        /// <returns>Pointer to the minimum 
        /// node in the collection.</returns>
        std::shared_ptr<fhNode<T>> rtn_val = min_node;
        delete_min();
        return rtn_val;
    }

    std::vector<std::shared_ptr<fhNode<T>>> get_roots() {
        /// <summary>
        /// Returns this collections roots.
        /// </summary>
        /// <returns>Collection of
        /// this roots.</returns>
        return roots;
    }

    FibonacciHeap<T> operator+(FibonacciHeap<T> &other) {
        /// <summary>
        /// Combine two FibonacciHeaps.
        /// </summary>
        /// <typeparam name="T">Other collection being
        /// combined with the current
        /// collection.</typeparam>
        FibonacciHeap<T> rtn_val(*this);

        // Update min_node when necessary.
        if (other.min_node->priority < rtn_val.min_node->priority) {
            rtn_val.min_node = other.min_node;
        }

        // Push each root from the other collection into the new 
        // collection.
        for (auto &root : other.roots) {
            rtn_val.roots.push_back(root);
        }

        return rtn_val;
    }

 FibonacciHeap<T>& operator+=(FibonacciHeap<T> &other) {
        /// <summary>
        /// Combine two FibonacciHeaps.
        /// </summary>
        /// <typeparam name="T">Other collection being
        /// combined with the current
        /// collection.</typeparam>

        // Update min_node when necessary.
        if (other.min_node->priority < min_node->priority) {
            min_node = other.min_node;
        }

        // Push each root from the other collection into the new 
        // collection.
        for (auto &root : other.roots) {
            roots.push_back(root);
        }

        return *this;
    }
};
