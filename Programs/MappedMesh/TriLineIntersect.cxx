//===================================================================
// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
//
// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;}
//        operators for:
//            == to test equality
//            != to test inequality
//            (Vector)0 = (0,0,0)         (null vector)
//            Point  = Point ± Vector
//            Vector = Point - Point
//            Vector = Scalar * Vector    (scalar product)
//            Vector = Vector * Vector    (cross product)
//    Line and Ray and Segment with defining points {Point P0, P1;}
//        (a Line is infinite, Rays and Segments start at P0)
//        (a Ray extends beyond P1, but a Segment ends at P1)
//    Plane with a point and a normal {Point V0; Vector n;}
//    Triangle with defining vertices {Point V0, V1, V2;}
//    Polyline and Polygon with n vertices {int n; Point *V;}
//        (a Polygon has V[n]=V[0])
//===================================================================

#include <math.h>


#include "TriLineIntersect.h"



// intersect_RayTriangle(): intersect a ray with a 3D triangle
//    Input:  a ray R, and a triangle T
//    Output: *I = intersection point (when it exists)
//    Return: -1 = triangle is degenerate (a segment or point)
//             0 = disjoint (no intersect)
//             1 = intersect in unique point I1
//             2 = are in the same plane
int
intersect_RayTriangle( Ray R, Triangle T, Point* I )
{
    Vector    u, v, n;             // triangle vectors
    Vector    dir, w0, w;          // ray vectors
    Vector    zero;
    double    r, a, b;             // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = T.V1 - T.V0;
    v = T.V2 - T.V0;
    n = u * v;                     // cross product
//    n /= n.GetNorm();

    zero.Fill(0);
    if (n == zero)                 // triangle is degenerate
        return -1;                 // do not deal with this case

    dir = R.P1 - R.P0;             // ray direction vector
//    dir /= dir.GetNorm();
    w0 = R.P0 - T.V0;
//    w0 /= w0.GetNorm();
    a = -dotproduct(n,w0);
    b = dotproduct(n,dir);
    if (fabs(b) < SMALL_NUM) {     // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return 2;
        else return 0;             // ray disjoint from plane
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)                   // ray goes away from triangle
        return 0;                  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect
    
    *I = R.P0 + dir * r;           // intersect point of ray and plane

    std::cout << "Direction: " << dir << std::endl;
    std::cout << "R: " << r << std::endl;
    std::cout << "R.P0: " << R.P0 << std::endl;
    std::cout << "I: " << *I << std::endl;
    std::cout << "A: " << a << " " << b << std::endl;

    // is I inside T?
    double    uu, uv, vv, wu, wv, D;
    uu = dotproduct(u,u);
    uv = dotproduct(u,v);
    vv = dotproduct(v,v);
    w = *I - T.V0;
    wu = dotproduct(w,u);
    wv = dotproduct(w,v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    double s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)        // I is outside T
        return 0;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)  // I is outside T
        return 0;

    return 1;                      // I is in T
}


/*
   Determine whether or not the line segment p1,p2
   Intersects the 3 vertex facet bounded by pa,pb,pc
   Return true/false and the intersection point p

   The equation of the line is p = p1 + mu (p2 - p1)
   The equation of the plane is a x + b y + c z + d = 0
                                n.x x + n.y y + n.z z + d = 0
*/

bool LineFacet(Ray R, Triangle T, Point *p)
{
   double d;
   double a1,a2,a3;
   double total,denom,mu;
   Point n,pa1,pa2,pa3;
   Point tmp;

   /* Calculate the parameters for the plane */
   n[0] = (T.V1[1] - T.V0[1])*(T.V2[2] - T.V0[2]) - (T.V1[2] - T.V0[2])*(T.V2[1] - T.V0[1]);
   n[1] = (T.V1[2] - T.V0[2])*(T.V2[0] - T.V0[0]) - (T.V1[0] - T.V0[0])*(T.V2[2] - T.V0[2]);
   n[2] = (T.V1[0] - T.V0[0])*(T.V2[1] - T.V0[1]) - (T.V1[1] - T.V0[1])*(T.V2[0] - T.V0[0]);
   n /= n.GetNorm();
//std::cout << "Norm: " << n << std::endl;

   d = - n[0] * T.V0[0] - n[1] * T.V0[1] - n[2] * T.V0[2];

   /* Calculate the position on the line that intersects the plane */
   denom = n[0] * (R.P1[0] - R.P0[0]) + n[1] * (R.P1[1] - R.P0[1]) + n[2] * (R.P1[2] - R.P0[2]);
   if (fabs(denom) < SMALL_NUM)         /* Line and plane don't intersect */
      return(false);
   mu = - (d + n[0] * R.P0[0] + n[1] * R.P0[1] + n[2] * R.P0[2]) / denom;
   tmp[0] = R.P0[0] + mu * (R.P1[0] - R.P0[0]);
   tmp[1] = R.P0[1] + mu * (R.P1[1] - R.P0[1]);
   tmp[2] = R.P0[2] + mu * (R.P1[2] - R.P0[2]);
   *p = tmp;
//std::cout << "Mu: " << mu << std::endl;
//std::cout << "R.P0: " << R.P0 << std::endl;
//std::cout << "R.P1: " << R.P1 << std::endl;
//std::cout << "Point: " << tmp << std::endl;

//   if (mu < 0 || mu > 1)   /* Intersection not along line segment */
//      return(false);

   /* Determine whether or not the intersection point is bounded by pa,pb,pc */
   pa1 = T.V0 - *p;
   pa1 /= pa1.GetNorm();
   
   pa2 = T.V1 - *p;
   pa2 /= pa2.GetNorm();

   pa3 = T.V2 - *p;
   pa3 /= pa3.GetNorm();

   a1 = pa1[0]*pa2[0] + pa1[1]*pa2[1] + pa1[2]*pa2[2];
   a2 = pa2[0]*pa3[0] + pa2[1]*pa3[1] + pa2[2]*pa3[2];
   a3 = pa3[0]*pa1[0] + pa3[1]*pa1[1] + pa3[2]*pa1[2];
   total = (acos(a1) + acos(a2) + acos(a3)) * 180/PI;

//std::cout << "total: " << total << std::endl;

   if (fabs(total - 360) > SMALL_NUM)
      return(false);

   return(true);
}


