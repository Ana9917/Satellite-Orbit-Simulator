#pragma once
#include <vector>
using namespace std;
struct Vertex3
{
    double x, y, z;
};
struct Mesh
{
    vector<Vertex3> vertices;
    vector<pair<int, int>> edges; ///vector of index pairs in vertex list
};