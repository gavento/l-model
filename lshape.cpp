#include <vector>
#include <cstdlib>
#include <cassert>
#include <iostream>

#include <google/protobuf/text_format.h>
#include "lshape-solution.pb.h"

using namespace std;

typedef uint64_t coord;
const coord min_coord = 0;
//const coord max_coord = 0x1000000000000000ULL;
const coord max_coord = 0x10ULL;

coord avg_coord(coord a, coord b)
{
  assert(b > a + 1);
  return (a + b) / 2;
}

class Graph {
 public:
  int vertices; // Numbered 0..(vertices-1)
  vector<vector<int> > neighbors;
  Solution inital_solution;

  int char_to_num(int c)
  {
    return c - 'a';
  }
    
  Graph(const string plantri_ascii)
  {
    // parse plantri ASCII repr.
    int idx = 0;
    int idx2 = plantri_ascii.find(' ');
    vertices = atol(plantri_ascii.substr(0, idx2).c_str());
    neighbors.resize(vertices);
    idx = idx2 + 1;
    for(int v = 0; v < vertices; v++) {
      vector<int> &l = neighbors[v];
      int c;
      while(c = plantri_ascii[idx], c >= 'a' && c <= 'z')
      {
	l.push_back(char_to_num(c));
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
  for(int i = 0; i < g.vertices; i++)
  {
    vector<int> &v = g.neighbors[i];
    o << i << ": ";
    for(vector<int>::iterator j = v.begin(); j!= v.end(); j++) {
      o << *j << ", ";
    }
  }
  o << ")";
}

int add_point(const Graph &g, const Solution &s0, vector<int> &depths)
{
  int d = s0.point_size(); // Depth
  depths[d]++;
  if (d == g.vertices)
  {
    string out;
    google::protobuf::TextFormat::PrintToString(s0, &out);
    cout << out << "\n";
    return 1;
  }
  Solution s(s0);
  int ix, iy;
  coord x, y;
  for(ix = 1; ix < s0.candidatex_size() - 1; ix++)
    for(iy = 1; iy < s0.candidatey_size() - 1; iy++)
    {
      x = s0.candidatex(ix);
      y = s0.candidatey(iy);
      Solution::Point &p = *s.add_point();
      p.set_x(x);
      p.set_xmin(x);
      p.set_xmax(max_coord);
      p.set_y(y);
      p.set_ymin(y);
      p.set_ymax(max_coord);
      // Check for possibility

      // Make arms shorter/longer

      // Create a new candidate X list
      s.clear_candidatex();
      for(int k = 0; k < s0.candidatex_size(); k++)
	if (s0.candidatex(k) == x) {
	  s.add_candidatex(avg_coord(s0.candidatex(k - 1), s0.candidatex(k)));
	  s.add_candidatex(avg_coord(s0.candidatex(k), s0.candidatex(k + 1)));
	} else {
	  s.add_candidatex(s0.candidatex(k));
	}
      // Create a new candidate Y list
      s.clear_candidatey();
      for(int k = 0; k < s0.candidatey_size(); k++)
	if (s0.candidatey(k) == y) {
	  s.add_candidatey(avg_coord(s0.candidatey(k - 1), s0.candidatey(k)));
	  s.add_candidatey(avg_coord(s0.candidatey(k), s0.candidatey(k + 1)));
	} else {
	  s.add_candidatey(s0.candidatey(k));
	}
      // Recurse
      int r = add_point(g, s, depths);
      if (r) return r;
    }
  return 0;
}

int main()
{
  Graph g("3 b,ac,b");
  vector<int> depths;
  depths.resize(g.vertices + 1);
  cout << g << "\n";
  add_point(g, g.inital_solution, depths);
  for(int i = 0; i < depths.size(); i++)
    cout << i << ": " << depths[i] << "\n";
}

