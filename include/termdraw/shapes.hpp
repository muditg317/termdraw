#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <Eigen/Eigen>
#include <vector>

/**
 * @brief draw a circle centered at x,y with radius r
 * float x
 * float y
 * float radius
 */
void circle(float,float,float);

/**
 * @brief draw a circle from the center with specified radius
 * vector center
 * float radius
 */
void circle(Eigen::Vector2f, float);

/**
 * @brief draw rect starting at x,y with dims w,l
 * float x
 * float y
 * float width
 * float length
 */
void rect(float,float,float,float);

/**
 * @brief draw rect with top-left corner and specified dims
 * Eigen::Vector2f corner-topleft
 * Eigen::Vector2f dims
 */
void rect(Eigen::Vector2f,Eigen::Vector2f);

#endif