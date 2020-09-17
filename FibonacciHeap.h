/**
    Title: FibonacciHeap
    File: FibonacciHeap.h
    Author: Tonia Sanzo (sanzo.tonia@gmail.com)

    A fibonacci heap implemented as descriped by Kevin Wayne
    (wayne@cs.princeton.edu)
    www.cs.princeton.edu/~wayne/teaching/fibonacci-heap.pdf

    Fibonacci heap was invented by Fredman and Tarjan, 1986

    ______________________________________________________________
    Priority Queues Performance Cost Summary:
    make_heap:         O(1)
    is_empty:          O(1)
    insert:            O(1)
    delete_min:        O(log(n))
    change_priority:   O(1)
    delete:            O(log(n))
    union:             O(1)
    find_min:          O(1)

    (amortized)
    (n = number of elements in priority queue)
    ______________________________________________________________
*/
#pragma once
#include <iostream>
#include <memory>
#include <vector>

/*
Node class, each node contains a value, and priority, and is
implemented in such a way to form a heap.
*/
template <typename T>
class fhNode {
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

    /*
    Construct a Node with a set priority and value, initially set
    to be a heap of size 1.

    @paramter: value (T) - generic object contained by the node
    @paramter: priority (long long) - priority key value
    */
    fhNode(T value, long long priority) {
        this->value = value;
        this->priority = priority;
    }

    /*
    Print a Node's content, priority, and children
    */
    void print() {
        std::cout << "Content: " << value << " Priority: " << priority << " Degree: " << degree 
                  << std::endl;
        for (auto &child : children) {
            std::cout << "child";
            child->print();
        }
    }

    /*
    Search a Node's tree structure for a specific Node, return a 
    pointer to the Node if its found otherwise false.

    @parameter: key (T) - Value contained in the target node.
    @parameter: priority (long long) - Priority of the target node.
    @return: (std::shared_ptr<fhNode<T>>) - Pointer to target node;
                                            otherwise a nullptr.
    */
    std::shared_ptr<fhNode<T>> search(const T &key, const long long &priority) {
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


/*
Fibonacci Heap, Similar to binomial heap, but less rigid, lazily
defers consolidation until next delete_min.
*/
template <typename T>
class FibonacciHeap {
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
    Insert a new Node into the fibonacci heap

    @parameter: value (T) - generic object contained by the node
    @parameter: priority (long long) - priority key value
    */
    void insert(const T &value, const long long &priority) {
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

    /*
    Delete max and consolidate trees so that no two roots have the
    same rank.
    */
    void delete_min() {
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
        min_node.reset();
        set_min();

        // Consolidate the trees so that no two roots have the same 
        // rank.
        consolidate_tree();    
    }

    /*
    Function to consolidate the trees within the heap. After this 
    function no trees will have the same (rank/degree/number of tree
    layers).
    */
    void consolidate_tree() {
        // Current root being checked
        auto current_index = 0;

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
                    auto it = std::find(roots.begin(), roots.end(), roots[current_index]);
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

    /*
    Given a degree grow rank to match that degree
    */
    void rank_grow(unsigned degree) {
        // grow rank to the correct degree size
        for (auto i = 0; i < degree - (rank.size() - 1); ++i) {
            rank.push_back(nullptr);
        }
    }

    /*
    Print the contents of this collection
    */
    void print_heap() { 
        if (min_node) { std::cout << "min_node: " << min_node->value << std::endl; }
        for (auto &root : roots) { root->print(); }
    }

    /*
    Set the min_node to the root with the lowest priority
    */
    void set_min() {
        min_node = nullptr;
        for (auto &root : roots) {
            if (!min_node || root->priority <= min_node->priority)
                min_node = root;
        }
    }

    /*
    Clear the contents of rank
    */
    void clear_rank() {
        for (auto &elem : rank) { elem = nullptr; }
    }

    /*
    @return: (size_t) The number of elements contained in the
                      FibonacciHeap.
    */
    std::size_t get_size() { return size; }

    /*
    @return: (bool) True if there are no elements in the collection;
                    otherwise false.
    */
    bool is_empty() { return !(size); }

    /*
    Change the priority of a key.

    @parameter: key (T) - Value contained by the target node.
    @parameter: new_priority (long long) - New priority of the key.
    */
    void change_priority(const T& key, const long long& old_priority , const long long& new_priority) {
        std::shared_ptr<fhNode<T>> parent_node, curr_node;
        std::vector<std::shared_ptr<fhNode<T>>>::iterator it;
        

        // If the new_priority is not less than the old_priority
        // exit.
        if (new_priority == old_priority) { return; }
        
        // Get a pointer to the node being changed.
        curr_node = find(key, old_priority);

        // If the node being searched for was not found exit.
        if (curr_node.use_count() == 0) { return; }

        // Change the priority of the node.
        curr_node->priority = new_priority;
        
        // If the curr_node has a parent.
        if (curr_node->parent.use_count != 0) {
            // Initialize parent_node to containe curr_node's parent
            parent_node = curr_node->parent;

            // If the curr_node's priority became less than the curr_node's 
            // parents priority update the collection.
            if (curr_node->priority < parent_node->priority) {
                // Cut the tree rooted at curr_node, meld into root
                // list, and unmark the curr_node.
                it = std::find(parent_node->children.begin(), parent_node->children.end(), curr_node);
                parent_node->children.erase(it);
                roots.push_back(curr_node);
                curr_node->parent = nullptr;
                curr_node->marked = false;

                // While the parent_node is marked, remove the 
                // necessary nodes from the heap.
                while (parent_node->marked) {
                    curr_node = parent_node;
                    parent_node = curr_node->parent;
                    
                    // If the parent_node is a nullptr, than exit
                    // this function.
                    if(parent_node.use_count() == 0){
                        break;    
                    }

                    // Cut the tree rooted at curr_node, meld into
                    // root list, and unmark the curr_node.
                    it = std::find(parent_node->children.begin(), parent_node->children.end(), curr_node);
                    parent_node->children.erase(it);
                    roots.push_back(curr_node);
                    curr_node->parent = nullptr;
                    curr_node->marked = false;
                }
                
                // If the parent_node is not a nullptr, mark it.
                if (parent_node.use_count() == 0) {
                    parent_node->marked = true;
                }
            }
            //__________________IMPLEMENT HERE____________________
            // Account for the situation where the priority of the current node becomes higher than the priority of the child node.
            else if() {
                
            }
        }
    }

    /*
    Given a key, finds and returns a pointer to that key.

    @parameter: key (T) - Target node.
    @parameter: priority (long long) - Current priority of target 
                                       node.
    @return: (std::shared_ptr<fhNode<T>>) - Pointer to target node;
                                            otherwise a nullptr.
    */
    std::shared_ptr<fhNode<T>> find(const T& key, const long long& priority) {
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
};
