#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <Eigen/Eigen>
#include <vector>

typedef Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> ArrayCompType;
typedef Eigen::Matrix<bool, -1, -1> MatrixXb;
typedef std::pair<ArrayCompType,MatrixXb> CacheablePixelData;
template<typename Key>
using cachedDataMap = std::map<Key, CacheablePixelData, std::less<Key>, Eigen::aligned_allocator<std::pair<const Key, CacheablePixelData>>>;

/**
 * @brief draw a circle from the center with specified radius
 * vector center
 * float radius
 */
void circle(Eigen::Vector2f, float);

inline void circle(float x, float y, float r) {
  return circle(Eigen::Vector2f(x,y), r);
}

/**
 * @brief draw rect with top-left corner and specified dims
 * Eigen::Vector2f corner-topleft (x,y)
 * Eigen::Vector2f dims           (w,l)
 */
void rect(Eigen::Vector2f,Eigen::Vector2f);

/**
 * @brief draw rect starting at x,y with dims w,l
 * float x
 * float y
 * float width
 * float length
 */
inline void rect(float x, float y, float w, float l) {
  return rect(Eigen::Vector2f(x,y), Eigen::Vector2f(w,l));
}


#endif