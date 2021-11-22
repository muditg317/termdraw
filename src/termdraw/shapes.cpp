#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

#include <linalg/vectors.hpp>


void circle(float x, float y, float r) {
  return circle(vecf{x,y}, r);
}

void circle(vecf center, float radius) {
  // veci b = vec_2int(vec_floor(center - as_vec<float,2>(radius)));
  // veci e = vec_2int(vec_floor(center + as_vec<float,2>(radius)));

  // for (int y = Y(b); y <= e.y; ++y) {
  //     for (int x = b.x; x <= e.x; ++x) {
  //         V2f p = v2f_sum(v2f2i(v2i(x, y)), v2ff(0.5f));
  //         V2f d = v2f_sub(c, p);
  //         if (v2f_sqrlen(d) <= r*r) {
  //             if (0 <= x && x < WIDTH && 0 <= y && y < HEIGHT) {
  //                 display[y*WIDTH + x] = FORE;
  //             }
  //         }
  //     }
  // }
}