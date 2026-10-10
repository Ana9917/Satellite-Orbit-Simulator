#pragma once
#include "mesh.h"

Mesh makeBox(double halfW, double halfH, double halfD); //8 corners of a cuboid and 12 edges
Mesh makeLatLongSphere(double radius, int rings, int segments); //the sphere based on horizontal rigs
Mesh makeSatelliteModel(); //the same 8 corners with extra edges for the panels