//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pretty Cool Fuzzy Guesser (PCFG)
//  --Probabilistic Context Free Grammar (PCFG) Password Guessing Program
//
//  Written by Matt Weir
//  Original backend priority queue code taken from stackexchange user arcomber
//  https://codereview.stackexchange.com/questions/186670/priority-queue-implementation-in-c-based-on-heap-ordered-resizable-array-tak
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//

#ifndef _PCFG_PQUEUE_H
#define _PCFG_PQUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include "grammar.h"
#include "pqueue.h"


// Begin: Example code
typedef struct {
    int weight;
    char* data;
} element;


typedef struct {
    int vertex;
    int weight;
} edge;

// End: Example code


// Parse Tree Item
//
// Contains a parse tree and associated probabilities for a PCFG "pre-terminal"
//
typedef struct PQItem {
    
    // The probability of this item
    double prob;
    
    // The probability of the base_structure that created this
    double base_prob;
    
    // The parse tree itself
    PcfgReplacements *pt[];
    
} PQItem;



extern int descending(const void* a, const void* b);


extern int ascending(const void* a, const void* b);


// Intitialize a PCFG PQueue
extern int initialize_pcfg_pqueue(priority_queue_t **pq, PcfgGrammar *pcfg);


#endif