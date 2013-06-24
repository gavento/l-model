#include <vector>
#include <set>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

#include <google/protobuf/text_format.h>
#include "lshape-solution.pb.h"

#ifdef WITH_NAUTY
#include <gtools.h>
#include <cstdio>
#endif

using std::ostream;
using std::vector;
using std::string; using std::stringstream;
using std::cin;
using std::cout;
using std::cerr;

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
  vector<vector<bool> > adjacent_array;
  Solution inital_solution;

  static int char_to_num(int c)
  {
    return c - 'a';
  }

  static char num_to_char(int c)
  {
    return c + 'a';
  }


  bool adjacent(int v1, int v2) const
  {
    return adjacent_array[v1][v2];
  }

#ifdef WITH_NAUTY
  Graph(const string graph6)
  {
    char *s = (char *)graph6.c_str();
    graph g[32];
    int m, n;
    n = graphsize(s);
    m = (n + WORDSIZE - 1) / WORDSIZE;
    stringtograph(s, g, m);

    vertices = n;
    adjacent_array.resize(vertices);
    for(int v = 0; v < vertices; v++) {
      adjacent_array[v].resize(vertices);
      for(int w = 0; w < vertices; w++) {
	adjacent_array[v][w] = ISELEMENT(GRAPHROW(g, v, m), w) || ISELEMENT(GRAPHROW(g, w, m), v);
//	assert(ISELEMENT(GRAPHROW(g, v, m), w) == ISELEMENT(GRAPHROW(g, w, m), v));
      }
    }
#else /* WITH_NAUTY */
  Graph(const string plantri_ascii)
  {
    // parse plantri ASCII repr.
    int idx = 0;
    int idx2 = plantri_ascii.find(' ');
    vertices = atol(plantri_ascii.substr(0, idx2).c_str());
    adjacent_array.resize(vertices);
    idx = idx2 + 1;
    for(int v = 0; v < vertices; v++) {
      adjacent_array[v].resize(vertices);
      int c;
      while(c = plantri_ascii[idx], c >= 'a' && c <= 'z')
      {
	int v2 = char_to_num(c);
	adjacent_array[v][v2] = true;
	idx++;
      }
      idx++;
    }
#endif /* WITH_NAUTY */
    /* Check symmetry */
    for(int v1 = 0; v1 < vertices; v1++)
      for(int v2 = 0; v2 < vertices; v2++)
	if (adjacent_array[v1][v2] != adjacent_array[v2][v1])
	  cerr << "Adjacency asymmetry between " << num_to_char(v1) << " and " << num_to_char(v2) << "\n";
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
  o << "Graph(" << g.vertices << "|";
  for(int i = 0; i < g.vertices; i++) {
    o << " " << i << ":";
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
  float maxx = 0, maxy = 0;
//  float dd = max_coord >> (d + 2);
  float dd = 0.5;
  for(int i = 0; i < d; i++) {
    const Solution::Point &p = s0.point(i);
    maxx = MAX(p.xmin(), maxx);
    maxy = MAX(p.ymin(), maxy);
    ss << "set arrow from " << p.x() << "," << p.y() << " to " << p.xmin()+dd << "," << p.y() << " nohead\n";
    ss << "set arrow from " << p.x() << "," << p.y() << " to " << p.x() << "," << p.ymin()+dd << " nohead\n";
    ss << "set label \"" << Graph::num_to_char(i) << "\" at " << p.x()-0.5 << "," << p.y() <<"\n";
  }
  ss << "set xrange [" << -2*dd << ":" << maxx+2*dd << "]\nset yrange [" << -2*dd << ":" << maxy+4*dd << "]\nplot -1.0\n\n";
  return ss.str();
}

string solution_proto(const google::protobuf::Message &s0)
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

int verify_solution(const Graph &g, const Solution &s, int allow_equal=0)
{
  int errors = 0;
  for (int i = 0; i < g.vertices; i++)
    for (int j = 0; j < g.vertices; j++)
      if (i != j) {
        const Solution::Point &pi = s.point(i);
        const Solution::Point &pj = s.point(j);
        if (!allow_equal) {
	  if (pi.x() == pj.x()) assert(0);
          if (pi.y() == pj.y()) assert(0);
	}
        if (g.adjacent(i,j)) {
          if ((pi.x() < pj.x()) && (pi.y() < pj.y())) assert(0);
          if ((pi.x() > pj.x()) && (pi.y() > pj.y())) assert(0);
          if ((pi.x() < pj.x()) && (pi.y() > pj.y()) && ((pi.xmin() < pj.x()) || (pj.ymin() < pi.y()))) assert(0);
          if ((pi.x() > pj.x()) && (pi.y() < pj.y()) && ((pj.xmin() < pi.x()) || (pi.ymin() < pj.y()))) assert(0);
	  if ((pi.x() == pj.x()) && (pi.y() > pj.y()) && (pj.ymin() < pi.y())) assert(0);
	  if ((pi.y() == pj.y()) && (pi.x() > pj.x()) && (pj.xmin() < pi.x())) assert(0);
        } else {
          if ((pi.x() <= pj.x()) && (pi.y() >= pj.y()) && (pi.xmin() >= pj.x()) && (pj.ymin() >= pi.y())) assert(0);
          if ((pi.x() >= pj.x()) && (pi.y() <= pj.y()) && (pj.xmin() >= pi.x()) && (pi.ymin() >= pj.y())) assert(0);
        }
      }
  return (errors == 0);
}

// Fix xmin, ymin given point coordinates
// Sets xmax, ymax to max_coord (not suitable for extending!)
void update_arm_mins(Graph &g, Solution &s0)
{
  for (int v = 0; v < g.vertices; v++) {
    Solution::Point &pv = *s0.mutable_point(v);
    coord xmin = pv.x();
    coord ymin = pv.y();
    for (int w = 0; w < g.vertices; w++) {
      Solution::Point &pw = *s0.mutable_point(w);
      if ((v != w) && (g.adjacent(v, w))) {
	xmin = MAX(xmin, pw.x());
	ymin = MAX(ymin, pw.y());
      }
    }
    assert(xmin >= pv.x());
    assert(ymin >= pv.y());
    pv.set_xmin(xmin);
    pv.set_ymin(ymin);
    pv.set_xmax(max_coord);
    pv.set_ymax(max_coord);
  }
}

void to_lowleft(Graph &g, Solution &s0)
{
  int change = 1;
  while(change) {
    change = 0;
    // To left
    for (int v = 0; v < g.vertices; v++) {
      Solution s_old(s0);
      Solution::Point &pv = *s0.mutable_point(v);
      coord minx = min_coord;
      for (int w = 0; w < g.vertices; w++) {
	Solution::Point &pw = *s0.mutable_point(w);
	if ((v != w)) {
	  if (g.adjacent(v, w)) {
	    if (pw.x() < pv.x())
	      minx = MAX(minx, pw.x() + 1); // Is (+1) required?
	  } else {
	    if ((pw.x() < pv.x()) && (pw.y() >= pv.y()) && (pw.y() <= pv.ymin()))
	      // Non-neighvor above pv but below pv's ymin, watch out for pw's right arm!
	      minx = MAX(minx, pw.xmin() + 1);
	    if ((pw.x() < pv.x()) && (pw.y() < pv.y()) && (pw.ymin() >= pv.y()))
	      // Non-neighvor below pv but pw's ymin above pv, watch out for pw's up arm!
	      minx = MAX(minx, pw.x() + 1);
	  }
	}
      }
      assert(minx <= pv.x());
      if (minx != pv.x())
	change = 1;
      pv.set_x(minx);
      update_arm_mins(g, s0);
      assert(verify_solution(g, s0, 1));
    }
    // To low
    for (int v = 0; v < g.vertices; v++) {
      Solution s_old(s0);
      Solution::Point &pv = *s0.mutable_point(v);
      coord miny = min_coord;
      for (int w = 0; w < g.vertices; w++) {
	Solution::Point &pw = *s0.mutable_point(w);
	if ((v != w)) {
	  if (g.adjacent(v, w)) {
	    if (pw.y() < pv.y())
	      miny = MAX(miny, pw.y() + 1); // Is (+1) required?
	  } else {
	    if ((pw.y() < pv.y()) && (pw.x() >= pv.x()) && (pw.x() <= pv.xmin()))
	      // Non-neighvor low-right of pv but left of pv's xmin, watch out for pw's up arm!
	      miny = MAX(miny, pw.ymin() + 1);
	    if ((pw.y() < pv.y()) && (pw.x() < pv.x()) && (pw.xmin() >= pv.x()))
	      // Non-neighvor low-left of pv but pw's xmin right of pv, watch out for pw's right arm!
	      miny = MAX(miny, pw.y() + 1);
	  }
	}
      }
      assert(miny <= pv.y());
      if (miny != pv.y())
	change = 1;
      pv.set_y(miny);
      update_arm_mins(g, s0);
      assert(verify_solution(g, s0, 1));
    }
  }
}

int add_point(Graph &g, Solution &s0, vector<int> &depths, std::set<string> *sols)
{
  int d = s0.point_size(); // Depth
  depths[d]++;
  if (d == g.vertices)
  {
    assert(verify_solution(g, s0));
    to_lowleft(g, s0);
    if (sols) {
      // Collect them all!
      string gs = solution_gnuplot(s0);
      sols->insert(gs);
      return 0;
    } else {
      // Print the first and exit
#ifdef WRITE_SOLUTION
      cerr << "\n** SOLUTION: **\n" << solution_proto(s0) << "\n";
#endif /* WRITE_SOLUTION */
#ifdef WRITE_GNUPLOT
      cerr << "\n** GNUPLOT: **\n" << solution_gnuplot(s0) << "\n";
#endif /* WRITE_GNUPLOT */
      return 1;
    }
  }

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
      int r = add_point(g, s, depths, sols);
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
    /* skip comments */
    if (s[0] == '#')
      continue;
    cerr << "*** Working at input " << i << ": " << s << "\n";
    Graph g(s);
    vector<int> depths;
    std::set<string> sols;
    depths.resize(g.vertices + 1);
#ifdef WRITE_GRAPH
    cerr << "\n** GRAPH: " << g << "\n";
#endif /* WRITE_GRAPH */
    int r = add_point(g, g.inital_solution, depths, NULL);
#ifdef WRITE_DEPTHS
    for(int k = 0; k < (int)depths.size(); k++)
      cerr << k << ": " << depths[k] << "\n";
#endif /* WRITE_DEPTHS */
    if (r == 0) {
      cout << "N " << s << "\n";
    }
    i++;
    if (i%10000 == 0)
      cout << "I " << i << " done\n";
  }
  cout << "I all " << i << " done\n";
  return 0;
}

