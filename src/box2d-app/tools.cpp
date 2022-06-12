#include <box2d-app/tools.hpp>

#include <iostream>

b2Body *addBody(b2World world, float x, float y, float vx, float vy, b2BodyType type) {
  b2BodyDef bodyDef;
  bodyDef.type = type;
  bodyDef.position.Set(
      x,y
  );
  bodyDef.linearVelocity.Set(
    vx,vy
  );
  bodyDef.fixedRotation = true;
  return world.CreateBody(&bodyDef);
}

void addFixtureToBodyWithShape(b2Body *body, b2Shape *shape) {
  b2FixtureDef fixtureDef;
  fixtureDef.shape = shape;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.0f;
  fixtureDef.restitution = 1.0f;
  fixtureDef.restitutionThreshold = 0;
  fixtureDef.filter.categoryBits = 0x02;
  fixtureDef.filter.maskBits = 0xffff;
  std::cout << "body.bodytype: " << body->GetType() << std::endl;
  std::cout << "static: " << b2_staticBody << std::endl;
  std::cout << "kinematic: " << b2_kinematicBody << std::endl;
  std::cout << "dynamic: " << b2_dynamicBody << std::endl;
  body->CreateFixture(&fixtureDef);
}

b2Body *addDynamicCircle(b2World world, float x, float y, float vx, float vy, float rad) {
  b2Body *body = addBody(world, x,y,vx,vy, b2_dynamicBody);
  // bodies[numBodies++] = body;
  b2CircleShape shape;
  shape.m_radius = rad;// * this->getScale();
  // this->createFixture(&shape);
  addFixtureToBodyWithShape(body, &shape);
  return body;
}

b2Body *addStaticRect(b2World world, float x, float y, float hx, float hy) {
  b2Body *body = addBody(world, x,y,0,0,b2_staticBody);
  // bodies[numBodies++] = body;
  b2PolygonShape shape;
  shape.SetAsBox(hx,hy);
  // shape.m_radius = r;// * this->getScale();
  // this->createFixture(&shape);
  addFixtureToBodyWithShape(body, &shape);
  return body;
}

BoundingBox makeBoundingBox(b2World world, float width, float height) {
  BoundingBox box;
  box.bottomWall = addStaticRect(world, width/2,height+1,width/2,1);
  box.topWall = addStaticRect(world, width/2,-1,width/2,1);
  box.rightWall = addStaticRect(world, width+1,height/2,1,height/2);
  box.leftWall = addStaticRect(world, -1,height/2,1,height/2);
  return box;
}