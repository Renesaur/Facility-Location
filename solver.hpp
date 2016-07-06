#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "heap.hpp"
#include <vector>
#include <string>

#if 1
#define PRINT_ADDITIONAL_INFORMATION
#endif

class Solver {
public:
  typedef unsigned flow_t;
  typedef double dist_t;
  class Point;

  Solver(flow_t u, const std::vector<Point> &D, const std::vector<Point> &I, dist_t facility_cost);
  void solve ();

private:
  class Customer;
  class Facility;

  /// finds the optimal Facility locations for current assignment (Barycenters)
  void optimize_I ();
  
  /// finds the optimal assignment for the current Facility locations (MaxFlow/SSP)
  void optimize_x ();
  
  /// computes the cost of the current assignment
  void compute_cost ();
  
  // prints the current assignment
  void print ();
  
  /// Successive Shortest Path Algorithm Implementation
  void ssp_algorithm ();
  
  /**
   * Increases the current flow along paths in the dij_tree by as much as possible.
   * Returns increase in flow.
   */
  void increase_flow (std::vector<Customer *> &unsupplied);
  
  /// tries to supply c through the dij_tree, deleting the edge considered from the tree
  bool supply (Customer *c);
  
  /// resets distances, dij_tree, heapnodes in preparation for dij_algorithm
  void dij_init ();
  
  /**
   * Runs dijkstra on the residual Graph.
   * Node is is sufficiant to only add Facilitys into the heap, as Customers either have
   * outdegree 0, in which case we don't need to propagate them any further,
   * or outdegree 1, in which case we can shortcut without increasing number of Edges.
   */
  void dij_algorithm ();
  
  /// brings the dij_tree computed previously into the form needed for ssp_push
  void dij_compute_children ();
  
  const flow_t _u;
  std::vector<Customer> _D;
  std::vector<Facility> _I;
  Heap<Facility> _heap;
  dist_t _cost;
  dist_t _facility_cost;
};

struct Solver::Point
{
  Point (dist_t x, dist_t y) : x(x), y(y) { }
  inline std::string show() const { return "\t"+std::to_string(x)+"\t"+std::to_string(y); }
  inline static dist_t dist(const Point &p, const Point &q) { return (p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y); };
  dist_t x;
  dist_t y;
};

struct Solver::Customer : public Point {
  inline Customer(Point p)
    : Point(p.x, p.y) { } 
  dist_t dij_dist;
  Facility *dij_parent;
  /// represents the incoming Edge
  Facility *flow_parent;
};

struct Solver::Facility : public Point {
  inline Facility(Point p)
    : Point(p.x, p.y) { }
  inline bool operator<(const Facility &rhs) const { return dij_dist < rhs.dij_dist; } // for heap in dijkstra
  dist_t dij_dist;
  dist_t pi;
  Customer *dij_parent;
  Heap<Facility>::Node *heap_node;
  /// represents the incoming Edge
  flow_t outflow;
};

#endif
