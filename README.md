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

    A fibonacci heap implemented as descriped by Kevin Wayne
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
    union:             O(1)
    find_min:          O(1)

    (amortized)
    (n = number of elements in priority queue)
    ______________________________________________________________
