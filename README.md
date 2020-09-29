    Title: FibonacciHeap
    File: FibonacciHeap.h
    Author: Tonia Sanzo (sanzo.tonia@gmail.com)

    A fibonacci heap implemented as described by Kevin Wayne
    (wayne@cs.princeton.edu)
    www.cs.princeton.edu/~wayne/teaching/fibonacci-heap.pdf

    Fibonacci heap was invented by Fredman and Tarjan, 1986.
    I did extensive QA on this program, but if you discover
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
