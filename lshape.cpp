#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

#include <google/protobuf/text_format.h>
#include "lshape-solution.pb.h"

using namespace std;

typedef uint64_t coord;
const coord min_coord = 0;
//const coord max_coord = 0x1000000000000000ULL;
const coord max_coord = 0x10000ULL;

coord avg_coord(coord a, coord b)
{
  assert(b > a + 1);
  return (a + b) / 2;
}

typedef vector<int> NeighborList;

class Graph {
 public:
  int vertices; // Numbered 0..(vertices-1)
//  vector<NeighborList> neighbors;
  vector<vector<bool> > adjacent_array;
  Solution inital_solution;

  int char_to_num(int c) const
  {
    return c - 'a';
  }

  bool adjacent(int v1, int v2) const
  {
    return adjacent_array[v1][v2];
  }

  Graph(const string plantri_ascii)
  {
    // parse plantri ASCII repr.
    int idx = 0;
    int idx2 = plantri_ascii.find(' ');
    vertices = atol(plantri_ascii.substr(0, idx2).c_str());
//    neighbors.resize(vertices);
    adjacent_array.resize(vertices);
    idx = idx2 + 1;
    for(int v = 0; v < vertices; v++) {
      adjacent_array[v].resize(vertices);
//      NeighborList &l = neighbors[v];
      int c;
      while(c = plantri_ascii[idx], c >= 'a' && c <= 'z')
      {
	int v2 = char_to_num(c);
//	l.push_back(v2);
	adjacent_array[v][v2] = true;
	idx++;
      }
      idx++;
    }
    inital_solution.add_candidatex(min_coord);
    inital_solution.add_candidatex(avg_coord(min_coord, max_coord));
    inital_solution.add_candidatex(max_coord);
    inital_solution.add_candidatey(min_coord);
    inital_solution.add_candidatey(avg_coord(min_coord, max_coord));
    inital_solution.add_candidatey(max_coord);
  }
};

ostream& operator<<(ostream &o, Graph &g)
{
  o << "Graph(" << g.vertices << ", ";
  for(int i = 0; i < g.vertices; i++) {
    o << i << ": ";
    for(int j = 0; j < g.vertices; j++)
      if ((i != j) && g.adjacent(i, j))
	o << j << ",";
  }
  o << ")";
  return o;
}

int limit_x(Solution::Point &what, Solution::Point &where)
{
  assert(what.x() < where.x());
  what.set_xmax(MIN(what.xmax(), where.x() - 1));
  return (what.xmin() <= what.xmax());
}

int limit_y(Solution::Point &what, Solution::Point &where)
{
  assert(what.y() < where.y());
  what.set_ymax(MIN(what.ymax(), where.y() - 1));
  return (what.ymin() <= what.ymax());
}

int extend_x(Solution &sol, Graph &g, int what_no, int to_no)
{
  Solution::Point &what = *sol.mutable_point(what_no);
  Solution::Point &to = *sol.mutable_point(to_no);
  assert(what.x() < to.x());
  assert(what.y() > to.y());
  what.set_xmin(MAX(what.xmin(), to.x()));
  for(int i = 0; i < sol.point_size(); i++)
    if ((what_no != i) && (!g.adjacent(i, what_no)))
    {
      Solution::Point &p = *sol.mutable_point(i);
      if ((p.x() > what.x()) && (p.y() < what.y()) && (p.x() <= what.xmin()))
	limit_y(p, what);
    }
  if (what.xmin() <= what.xmax()) return 1;
  return 0;
}

int extend_y(Solution &sol, Graph &g, int what_no, int to_no)
{
  Solution::Point &what = *sol.mutable_point(what_no);
  Solution::Point &to = *sol.mutable_point(to_no);
  assert(what.y() < to.y());
  assert(what.x() > to.x());
  what.set_ymin(MAX(what.ymin(), to.y()));
  for(int i = 0; i < sol.point_size(); i++)
    if ((what_no != i) && (!g.adjacent(i, what_no)))
    {
      Solution::Point &p = *sol.mutable_point(i);
      if ((p.y() > what.y()) && (p.x() < what.x()) && (p.y() <= what.ymin()))
	limit_x(p, what);
    }
  if (what.ymin() <= what.ymax()) return 1;
  return 0;
}

string solution_gnuplot(const Solution &s0)
{
  int d = s0.point_size();
  stringstream ss(stringstream::out);
  for(int i = 0; i < d; i++) {
    const Solution::Point &p = s0.point(i);
    float dd = max_coord >> (d + 2);
    ss << "set arrow from " << p.x() << "," << p.y() << " to " << p.xmin()+dd << "," << p.y() << " nohead\n";
    ss << "set arrow from " << p.x() << "," << p.y() << " to " << p.x() << "," << p.ymin()+dd << " nohead\n";
  }
  ss << "set xrange [0:" << max_coord << "]\nset yrange [0:" << max_coord << "]\nplot 1\n\n";
  return ss.str();
}

string solution_proto(const Solution &s0)
{
  string s;
  google::protobuf::TextFormat::PrintToString(s0, &s);
  return s;
}

string solution_coords(const Solution &s0)
{
  stringstream ss(stringstream::out);
  for(int i = 0; i < s0.point_size(); i++) {
    const Solution::Point &p = s0.point(i);
    ss << i << ": x=" << p.x() << " to " << p.xmin() << "-" << p.xmax() <<
              "   y=" << p.y() << " to " << p.ymin() << "-" << p.ymax() << "\n";
  }
  return ss.str();
}

int verify_solution(const Graph &g, const Solution &s)
{
  int errors = 0;
  for (int i = 0; i < g.vertices; i++)
    for (int j = 0; j < g.vertices; j++)
      if (i != j) {
        const Solution::Point &pi = s.point(i);
        const Solution::Point &pj = s.point(j);
        if (pi.x() == pj.x()) assert(0);
        if (pi.y() == pj.y()) assert(0);
        if (g.adjacent(i,j)) {
          if ((pi.x() < pj.x()) && (pi.y() < pj.y())) assert(0);
          if ((pi.x() > pj.x()) && (pi.y() > pj.y())) assert(0);
          if ((pi.x() < pj.x()) && (pi.y() > pj.y()) && ((pi.xmin() < pj.x()) || (pj.ymin() < pi.y()))) assert(0);
          if ((pi.x() > pj.x()) && (pi.y() < pj.y()) && ((pj.xmin() < pi.x()) || (pi.ymin() < pj.y()))) assert(0);
        } else {
          if ((pi.x() < pj.x()) && (pi.y() > pj.y()) && (pi.xmin() >= pj.x()) && (pj.ymin() >= pi.y())) assert(0);
          if ((pi.x() > pj.x()) && (pi.y() < pj.y()) && (pj.xmin() >= pi.x()) && (pi.ymin() >= pj.y())) assert(0);
        }
      }   
  return (errors == 0);
}

int add_point(Graph &g, Solution &s0, vector<int> &depths)
{
  int d = s0.point_size(); // Depth
  depths[d]++;
  if (d == g.vertices)
  {
    assert(verify_solution(g, s0));
    cout << "\n**SOLUTION: **\n" << solution_proto(s0) << "\n";
    cout << "\n**GNUPLOT: **\n" << solution_gnuplot(s0) << "\n";
//    for(int i = 0; i < (int)depths.size(); i++)
//      cout << i << ": " << depths[i] << "\n";
    return 1;
  }

//  if ((d == 4) && (s0.point(1).x()==1536) && (s0.point(2).x()==512) && s0.point(3).x()==1792) {
//    cout << "\n***" << d << "\n" << solution_coords(s0) << "\n";
//  }

  int ix, iy;
  coord x, y;

  for(ix = 1; ix < s0.candidatex_size() - 1; ix++)
    for(iy = 1; iy < s0.candidatey_size() - 1; iy++)
    {
      Solution s(s0);
      Solution::Point &p = *s.add_point();
      int ip = s.point_size() - 1;
      assert(ip == d);

      int ok = 1;
      x = s0.candidatex(ix);
      y = s0.candidatey(iy);
      p.set_x(x);
      p.set_xmin(x);
      p.set_xmax(max_coord);
      p.set_y(y);
      p.set_ymin(y);
      p.set_ymax(max_coord);

      // Check for neighbor possibility
      for(int in = 0; (in < d) && ok; in++)
      {
	if (g.adjacent(in, ip)) {
	  coord nx = s.point(in).x();
	  coord ny = s.point(in).y();
	  if ((nx < x) && (ny < y)) ok = 0;
	  if ((nx > x) && (ny > y)) ok = 0;
	}
      }
      if (!ok) {
	continue;
      }

      // Make arms shorter/longer
      for (int iv = 0; (iv < d) && ok; iv++) {
	Solution::Point &v = *s.mutable_point(iv);
	coord vx = v.x();
	coord vy = v.y();
	if (g.adjacent(ip, iv)) {
	  // Extend arms to meet
	  if ((vx < x) && (vy > y)) {
	    ok &= extend_x(s, g, iv, ip);
	    ok &= extend_y(s, g, ip, iv);
	  } else if ((vx > x) && (vy < y)) {
	    ok &= extend_y(s, g, iv, ip);
	    ok &= extend_x(s, g, ip, iv);
	  } else assert(0);
	} else { // not neighbor
	  if ((vx < x) && (vy > y)) {
//	    if (p.ymin() > v.y()) ok &= limit_x(v, p);
	    if (v.xmin() >= p.x()) ok &= limit_y(p, v);
	  }
	  if ((vx > x) && (vy < y)) {
//	    if (p.xmin() > v.x()) ok &= limit_y(v, p);
	    if (v.ymin() >= p.y()) ok &= limit_x(p, v);
	  }
	}
      }
      if (!ok) {
	continue;
      }

      // Create a new candidate X list
      s.clear_candidatex();
      for(int k = 0; k < s0.candidatex_size(); k++)
	if (s0.candidatex(k) == x) {
	  s.add_candidatex(s0.candidatex(k) - (max_coord >> (d + 2)));
	  s.add_candidatex(s0.candidatex(k) + (max_coord >> (d + 2)));
	} else {
	  s.add_candidatex(s0.candidatex(k));
	}
      // Create a new candidate Y list
      s.clear_candidatey();
      for(int k = 0; k < s0.candidatey_size(); k++)
	if (s0.candidatey(k) == y) {
	  s.add_candidatey(s0.candidatey(k) - (max_coord >> (d + 2)));
	  s.add_candidatey(s0.candidatey(k) + (max_coord >> (d + 2)));
	} else {
	  s.add_candidatey(s0.candidatey(k));
	}
      // Recurse
      int r = add_point(g, s, depths);
      if (r) return r;
    }
  return 0;
}

int main(int argc, char **argv)
{
  assert(argc == 1);
  string s;
  int i = 0;

  while(getline(cin, s), s.size() >= 3) {
    cout << "working at " << s << "\n";
    Graph g(s);
    vector<int> depths;
    depths.resize(g.vertices + 1);
    cout << g << "\n";
    int r = add_point(g, g.inital_solution, depths);
    for(int k = 0; k < (int)depths.size(); k++)
      cout << k << ": " << depths[k] << "\n";
    if (r == 0) {
      cerr << "N " << s << "\n";
    }
    i++;
    if (i%10000 == 0)
      cerr << i << " done\n";
  }
  return 0;
}

