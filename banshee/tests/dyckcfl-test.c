/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <assert.h>
#include "nonspec.h"
#include "dyckcfl.h"
#include "mr_dyckcfl.h"

bool test_dyck_clustering = TRUE;
bool test_dyck_contra = TRUE;

static void myassert(int a) {
	if (!a) {
		printf("myassertion failed");
		assert(0);
		exit(1);
	}
}


static void test_dyck_isomorphism()
{
  dyck_node d0,d1,d2,d3,d4,d5,d6,d7,d8,d9;
  int indices[2] = {0,1};
  dyck_node nodes[2] = {NULL,NULL};
  dyck_node c0,c1,c2,c3,c4,c5;
  dyck_node e0,e1,e2,e3,e4,e5;
//   flag_dyck_print_constraints = 1;
  
  d0 = make_tagged_dyck_node("d0");
  d1 = make_tagged_dyck_node("d1");
  d2 = make_tagged_dyck_node("d2");
  d3 = make_tagged_dyck_node("d3");
  d4 = make_tagged_dyck_node("d4");
  d5 = make_tagged_dyck_node("d5");
  d6 = make_tagged_dyck_node("d6");
  d7 = make_tagged_dyck_node("d7");
  d8 = make_tagged_dyck_node("d8");
  d9 = make_tagged_dyck_node("d9");

  make_dyck_subtype_edge(d1,d2);
  make_dyck_open_edge(d0,d1,1);
  make_dyck_close_edge(d2,d3,1);
  make_dyck_close_edge(d2,d4,2);
  make_dyck_open_edge(d5,d1,2);
  make_dyck_close_edge(d7,d2,3);
  make_dyck_subtype_edge(d1,d8);
  make_dyck_close_edge(d8,d9,3);
  mark_dyck_node_global(d8);

//   flag_dyck_print_constraints = 1;

  // Clustering test
  if (test_dyck_clustering) {
    c0 = make_tagged_dyck_node("c0");
    c1 = make_tagged_dyck_node("c1");
    c2 = make_tagged_dyck_node("c2");
    c3 = make_tagged_dyck_node("c3");
    c4 = make_tagged_dyck_node("c4");
    c5 = make_tagged_dyck_node("c5");
    nodes[0] = c0;
    nodes[1] = c1;
    make_clustered_dyck_open_edges(nodes,c2,indices,2);
    make_dyck_subtype_edge(c2,c3);
    make_dyck_close_edge_for_cluster(c3,c4,0);
    make_dyck_close_edge_for_cluster(c3,c5,1);
  }

  if (test_dyck_contra) {
    e0 = make_tagged_dyck_node("e0");
    e1 = make_tagged_dyck_node("e1");
    e2 = make_tagged_dyck_node("e2");
    e3 = make_tagged_dyck_node("e3");
    e4 = make_tagged_dyck_node("e4");
    e5 = make_tagged_dyck_node("e5");
    
    make_dyck_contra_open_edge(e0,e1,1);
    make_dyck_contra_open_edge(e4,e1,2);
    make_dyck_subtype_edge(e1,e2);
    make_dyck_contra_close_edge(e2,e3,1);   
    make_dyck_contra_close_edge(e2,e5,2);
  }

  dyck_finished_adding();	// we've finished building the graph


  myassert(dyck_check_reaches(d1,d1)); // check that reflexivity works
  myassert(dyck_check_reaches(d1,d2)); // check that subtyping alone works
  myassert(dyck_check_reaches(d0,d3)); // check that an open/close matching works
  myassert(dyck_check_reaches(d5,d4)); // check another open/close matching

  myassert(!dyck_check_reaches(d0,d4)); // make sure that (_1 )_2 doesn't work
  myassert(!dyck_check_reaches(d5,d3)); // make sure that (_2 )_1 doesn't work
  myassert(!dyck_check_reaches(d0,d2)); // we're only doing matched reachability
  myassert(!dyck_check_reaches(d5,d2)); // we're only doing matched reachability 

  myassert(!dyck_check_reaches(d0,d6)); // make sure nothing reaches d6 except d6
  myassert(!dyck_check_reaches(d1,d6));
  myassert(!dyck_check_reaches(d2,d6));
  myassert(!dyck_check_reaches(d3,d6));
  myassert(!dyck_check_reaches(d4,d6));
  myassert(!dyck_check_reaches(d5,d6));
  myassert(dyck_check_reaches(d6,d6));

  // PN reachability checking
  myassert(dyck_check_pn_reaches(d1,d1)); // check that reflexivity works
  myassert(dyck_check_pn_reaches(d1,d2)); // check that subtyping alone works
  myassert(dyck_check_pn_reaches(d0,d3));	// check that an open/close matching works
  myassert(dyck_check_pn_reaches(d5,d4)); // check another open/close matching

  myassert(!dyck_check_pn_reaches(d0,d4)); // make sure that (_1 )_2 doesn't work
  myassert(!dyck_check_pn_reaches(d5,d3)); // make sure that (_2 )_1 doesn't work
  myassert(dyck_check_pn_reaches(d5,d2)); // check n reachability
  myassert(dyck_check_pn_reaches(d0,d2)); // check n reachability
  myassert(dyck_check_pn_reaches(d1,d3)); // check p reachability
  myassert(dyck_check_pn_reaches(d2,d4)); // check p reachability
  myassert(dyck_check_pn_reaches(d2,d3));	// check p reachability
  myassert(dyck_check_pn_reaches(d2,d4)); // check p reachability

  myassert(dyck_check_pn_reaches(d7,d3)); // check pn reachability
  myassert(dyck_check_pn_reaches(d7,d2)); // check pn reachability

  // Global reachability checking
  myassert(dyck_check_reaches(d0,d8)); // should reach by a match
  myassert(dyck_check_reaches(d5,d8)); // should reach by a match
  myassert(dyck_check_pn_reaches(d0,d9)); // pn reachability using a global
  myassert(dyck_check_pn_reaches(d5,d9));	// pn reachability using a global
  myassert(!dyck_check_reaches(d7,d9)); // no matched reachability, even through global
  //myassert(!dyck_check_reaches(d0,d9)); // no matched reachability using a global
  //myassert(!dyck_check_reaches(d5,d9)); // no matched reachability using a global
  myassert(dyck_check_reaches(d8,d9)); // check for promotion of p reachability to matched

  if (test_dyck_clustering) {
    myassert(dyck_check_reaches(c0,c4));
    myassert(dyck_check_reaches(c1,c5));
    myassert(!dyck_check_reaches(c0,c5));
    myassert(!dyck_check_reaches(c1,c4));
    myassert(!dyck_check_reaches(c0,c2));
    myassert(!dyck_check_reaches(c1,c2));
    myassert(!dyck_check_reaches(c0,c3));
    myassert(!dyck_check_reaches(c1,c3));
  }

  if (test_dyck_contra) {
    myassert(dyck_check_reaches(e3,e0));
    myassert(dyck_check_reaches(e5,e4));
    myassert(!dyck_check_reaches(e3,e4));
    myassert(!dyck_check_reaches(e5,e0));
  }

  // Print the closed graph
/*   { */
/*     FILE *f = fopen("closed_graph.dot","w"); */
/*     myassert(f); */
/*     dyck_print_closed_graph(f); */
/*     fclose(f); */
/*   } */

  
  printf("Passed dyck isomorphism test\n");
}

static void test_dyck_reduction()
{
  mr_dyck_node d0,d1,d2,d3,d4,d5,d6,d7,d8,d9;//,d10,d11;

// flag_mr_dyck_print_constraints = 1;
  
  d0 = make_tagged_mr_dyck_node("d0");
  d1 = make_tagged_mr_dyck_node("d1");
  d2 = make_tagged_mr_dyck_node("d2");
  d3 = make_tagged_mr_dyck_node("d3");
  d4 = make_tagged_mr_dyck_node("d4");
  d5 = make_tagged_mr_dyck_node("d5");
  d6 = make_tagged_mr_dyck_node("d6");
  d7 = make_tagged_mr_dyck_node("d7");  
  d8 = make_tagged_mr_dyck_node("d8");
  d9 = make_tagged_mr_dyck_node("d9");
//   d10 = make_tagged_mr_dyck_node("d10");
//   d11 = make_tagged_mr_dyck_node("d11");

  make_mr_dyck_subtype_edge(d1,d2);
  make_mr_dyck_open_edge(d0,d1,1);
  make_mr_dyck_close_edge(d2,d3,1);
  make_mr_dyck_close_edge(d2,d4,2);
  make_mr_dyck_open_edge(d5,d1,2);
  make_mr_dyck_close_edge(d7,d2,3);
  make_mr_dyck_subtype_edge(d1,d8);
  make_mr_dyck_close_edge(d8,d9,3);
  mark_mr_dyck_node_global(d8);
//   make_mr_dyck_close_edge(d10,d11,4);

  mr_dyck_finished_adding();	// we've finished building the graph

  myassert(mr_dyck_check_reaches(d1,d1)); // check that reflexivity works
  myassert(mr_dyck_check_reaches(d1,d2)); // check that subtyping alone works
  myassert(mr_dyck_check_reaches(d0,d3));	// check that an open/close matching works
  myassert(mr_dyck_check_reaches(d5,d4)); // check another open/close matching
  
  myassert(!mr_dyck_check_reaches(d0,d4)); // make sure that (_1 )_2 doesn't work
  myassert(!mr_dyck_check_reaches(d5,d3)); // make sure that (_2 )_1 doesn't work
  myassert(!mr_dyck_check_reaches(d0,d2)); // we're only doing matched reachability
  myassert(!mr_dyck_check_reaches(d5,d2)); // we're only doing matched reachability 

//   myassert(!mr_dyck_check_reaches(d10,d11)); // only matched reachability

  // PN reachability checking
  myassert(mr_dyck_check_pn_reaches(d1,d1)); // check that reflexivity works
  myassert(mr_dyck_check_pn_reaches(d1,d2)); // check that subtyping alone works
  myassert(mr_dyck_check_pn_reaches(d0,d3)); // check that an open/close matching works
  myassert(mr_dyck_check_pn_reaches(d5,d4)); // check another open/close matching

  myassert(!mr_dyck_check_pn_reaches(d0,d4)); // make sure that (_1 )_2 doesn't work
  myassert(!mr_dyck_check_pn_reaches(d5,d3)); // make sure that (_2 )_1 doesn't work
  myassert(mr_dyck_check_pn_reaches(d5,d2)); // check n reachability
  myassert(mr_dyck_check_pn_reaches(d0,d2)); // check n reachability
  myassert(mr_dyck_check_pn_reaches(d1,d3)); // check p reachability
  myassert(mr_dyck_check_pn_reaches(d2,d4)); // check p reachability
  myassert(mr_dyck_check_pn_reaches(d2,d3)); // check p reachability
  myassert(mr_dyck_check_pn_reaches(d2,d4)); // check p reachability

  myassert(mr_dyck_check_pn_reaches(d7,d3)); // check pn reachability
  myassert(mr_dyck_check_pn_reaches(d7,d2)); // check pn reachability

  // Global reachability checking
  myassert(mr_dyck_check_reaches(d0,d8)); // should reach by a match
  myassert(mr_dyck_check_reaches(d5,d8)); // should reach by a match
  myassert(mr_dyck_check_pn_reaches(d0,d9)); // pn reachability using a global
  myassert(mr_dyck_check_pn_reaches(d5,d9)); // pn reachability using a global
  myassert(!mr_dyck_check_reaches(d7,d9)); // no matched reachability, even through global
  //myassert(!mr_dyck_check_reaches(d0,d9)); // no matched reachability using a global
  //myassert(!mr_dyck_check_reaches(d5,d9)); // no matched reachability using a global

  // Print the closed graph
/*   { */
/*     FILE *f = fopen("mr_closed_graph.dot","w"); */
/*     myassert(f); */
/*     mr_dyck_print_closed_graph(f); */
/*     fclose(f); */
/*   } */
  
  printf("Passed dyck reduction test\n");
}



int main()
{
  nonspec_init();
  dyck_init(TRUE);
  flag_merge_projections = FALSE;
  mr_dyck_init(TRUE,NULL);

  test_dyck_isomorphism();
  test_dyck_reduction();

  /* Do it one more time to test queries interspersed between
     constraint additions */
  test_dyck_isomorphism();

  return 0;
}
