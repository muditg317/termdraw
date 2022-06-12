#include <box2d-app/tools.hpp>

#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

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

bool isContactBetween(b2Contact *contact, b2Body *body1, b2Body *body2) {
  b2Body *contactBodyA = contact->GetFixtureA()->GetBody();
  b2Body *contactBodyB = contact->GetFixtureB()->GetBody();
  return (contactBodyA == body1 && contactBodyB == body2) ||
         (contactBodyA == body2 && contactBodyB == body1);
}


void drawCircleBody(b2Body *circleBody) {
  b2Fixture *fixture = circleBody->GetFixtureList();
  if (fixture->GetShape()->GetType() != b2Shape::Type::e_circle) {
    graphics_printf("Trying to draw non circle body as circle!");
    return;
  }
  b2Vec2 pos = circleBody->GetPosition();
  float rad = fixture->GetShape()->m_radius;
  circle(pos.x*PIXELS_PER_METER,pos.y*PIXELS_PER_METER, rad*PIXELS_PER_METER);
}

void drawRectBody(b2Body *rectBody) {
  b2Fixture *fixture = rectBody->GetFixtureList();
  if (fixture->GetShape()->GetType() != b2Shape::Type::e_polygon) {
    graphics_printf("Trying to draw non polygon body as polygon!");
    return;
  }
  b2Vec2 pos = rectBody->GetPosition();
  static b2AABB bounds;
  static b2Transform transform;
  transform.SetIdentity();
  fixture->GetShape()->ComputeAABB(&bounds, transform, 0);
  b2Vec2 corner = pos + bounds.lowerBound;
  b2Vec2 dims = 2 * bounds.GetExtents();
  rect(corner.x*PIXELS_PER_METER, corner.y*PIXELS_PER_METER, dims.x*PIXELS_PER_METER, dims.y*PIXELS_PER_METER);
}