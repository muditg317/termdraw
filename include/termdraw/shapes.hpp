#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <termdraw/graphics.hpp>

#include <Eigen/Eigen>
#include <map>
#include <vector>

typedef Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> ArrayCompType;
typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> MatrixXb;
typedef ArrayCompType CacheablePixelData;
template<typename Key>
using cachedDataMap = std::map<Key, CacheablePixelData, std::less<Key>, Eigen::aligned_allocator<std::pair<const Key, CacheablePixelData>>>;

/**
 * @brief draw a circle from the center with specified radius
 * vector center
 * float radius
 */
void circle(Eigen::Vector2f, float, pixelValue = WHITE);

inline void circle(float x, float y, float r, pixelValue value = WHITE) {
  return circle(Eigen::Vector2f(x,y), r, value);
}

/**
 * @brief draw rect with top-left corner and specified dims
 * Eigen::Vector2f corner-topleft (x,y)
 * Eigen::Vector2f dims           (w,h)
 */
void rect(Eigen::Vector2f,Eigen::Vector2f, pixelValue=WHITE);

/**
 * @brief draw rect starting at x,y with dims w,h
 * float x
 * float y
 * float width
 * float height
 */
inline void rect(float x, float y, float w, float h, pixelValue value = WHITE) {
  return rect(Eigen::Vector2f(x,y), Eigen::Vector2f(w,h), value);
}


#endif