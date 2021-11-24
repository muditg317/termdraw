#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

#include <Eigen/Eigen>
#include <iostream>

static cachedDataMap<float> circleDataMap;
void circle(Eigen::Vector2f center, float radius) {
  int radCeil = (int) std::ceil(radius);
  int radCeil2p1 = radCeil * 2+1;


  int minX = int(center.x())-radCeil;
  int minY = int(center.y())-radCeil;
  int dataMinX = 0;
  if (minX < 0) {
    dataMinX -= minX;
    minX = 0;
  }
  int dataMinY = 0;
  if (minY < 0) {
    dataMinY -= minY;
    minY = 0;
  }

  int maxX = int(center.x())+radCeil;
  int maxY = int(center.y())+radCeil;
  int dataMaxX = radCeil2p1-1;
  if (maxX >= WIDTH) {
    dataMaxX -= (maxX-WIDTH+1);
    maxX = WIDTH-1;
  }
  int dataMaxY = radCeil2p1-1;
  if (maxY >= HEIGHT) {
    dataMaxY -= (maxY-HEIGHT+1);
    maxY = HEIGHT-1;
  }

  bool found = true;
  decltype(circleDataMap)::iterator circleMapIter(circleDataMap.lower_bound(radius));
  if (circleMapIter == circleDataMap.end() || radius < circleMapIter->first) { // not found
    found = false;
    // std::cout << "calc stuff for " << radCeil2p1 << "!" << std::endl;
    Eigen::MatrixXcf templateMat ( radCeil2p1, radCeil2p1);
    templateMat.real() = Eigen::VectorXf::LinSpaced(radCeil2p1, -radCeil+0.5, radCeil+0.5).replicate(1, radCeil2p1);
    templateMat.imag() = templateMat.real().transpose();
    auto _arrCmp = (templateMat.cwiseAbs2().array() <= radius*radius).eval();
    auto _ones = Eigen::DenseBase<PixelBuffer>::Ones(radCeil2p1,radCeil2p1).eval();
    circleMapIter = circleDataMap.insert(circleMapIter, std::make_pair(radius, CacheablePixelData(_arrCmp,_ones)));     // hinted insertion
  } else {
      // ... use circleMapIter->second here
    // if (i%1==0) std::cout << "pre calculated stuff for " << radCeil2p1 << "!" << std::endl;
  }

  if (minX > WIDTH-1 || maxX < 0 || minY > HEIGHT-1 || maxY < 0) {
    return;
  }

  auto bufferChunk = BUFFER(Eigen::seq(minX,maxX),Eigen::seq(minY,maxY));//.block(minX,minY,maxX-minX+1,maxY-minY+1);

  CacheablePixelData pair = circleMapIter->second;

  auto arrCmp = pair.first(Eigen::seq(dataMinX,dataMaxX),Eigen::seq(dataMinY,dataMaxY));
  auto ones = pair.second(Eigen::seq(dataMinX,dataMaxX),Eigen::seq(dataMinY,dataMaxY));


  if (bufferChunk.size() != arrCmp.size()) {
    printf("circle: <%.2f,%.2f> r=%.2f\n",center.x(),center.y(),radius);
    printf("pixels: <%d,%d> to <%d,%d>\n",minX,minY,maxX,maxY);
    printf("data: <%d,%d> to <%d,%d>\n",dataMinX,dataMinY,dataMaxX,dataMaxY);
    std::cout << "buffer:" << bufferChunk.size() << "\n" << bufferChunk << std::endl;
    std::cout << "arrCmp:" << arrCmp.size() << "\n" << arrCmp << std::endl;
  }
  assert((bufferChunk.size() == arrCmp.size()) && "buffrChunk and arrCmp have different size!!");

  // std::cout << "update bufferChunk\n";
  bufferChunk = arrCmp.select(ones, bufferChunk);
  // std::cout << "END update bufferChunk\n";
}