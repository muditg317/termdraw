#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <Eigen/Eigen>

class Particle {
  public:
    void applyForce(Eigen::Vector2f);
    void update();
    Particle(Eigen::Vector2f initialPosition, Eigen::Vector2f initialVelocity);
    Particle(Eigen::Vector2f initialPosition) {
      Particle(initialPosition,Eigen::Vector2f(0,0));
    }
    void draw();
  private:
    Eigen::Vector2f position;
    Eigen::Vector2f velocity;
};

#endif