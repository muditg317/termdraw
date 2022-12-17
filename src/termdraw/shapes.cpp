#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>

#include <Eigen/Eigen>
#include <iostream>

// void applyCachedData(ArrCompType &comparison, MatrixXb &onesMatrix, int minX, int minY, int maxX, int maxY);

void applyCachedData(CacheablePixelData &mask, int minX, int minY, int maxX, int maxY, int dataMaxX, int dataMaxY, pixelValue value = WHITE) {
  if (minX > WIDTH-1 || maxX < 0 || minY > HEIGHT-1 || maxY < 0) {
    return;
  }

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

  if (maxX >= WIDTH) {
    dataMaxX -= (maxX-WIDTH+1);
    maxX = WIDTH-1;
  }
  if (maxY >= HEIGHT) {
    dataMaxY -= (maxY-HEIGHT+1);
    maxY = HEIGHT-1;
  }

  auto bufferChunk = BUFFER(Eigen::seq(minX,maxX),Eigen::seq(minY,maxY));//.block(minX,minY,maxX-minX+1,maxY-minY+1);

  ArrayCompType arrCmp = mask(Eigen::seq(dataMinX,dataMaxX),Eigen::seq(dataMinY,dataMaxY));

  if (bufferChunk.size() != arrCmp.size()) {
    // printf("circle: <%.2f,%.2f> r=%.2f\n",center.x(),center.y(),radius);
    graphics_printf("pixels: <%d,%d> to <%d,%d>\n",minX,minY,maxX,maxY);
    graphics_printf("data: <%d,%d> to <%d,%d>\n",dataMinX,dataMinY,dataMaxX,dataMaxY);
    std::cout << "buffer:" << bufferChunk.size() << "\n" << bufferChunk << std::endl;
    std::cout << "arrCmp:" << arrCmp.size() << "\n" << arrCmp << std::endl;
  }
  assert((bufferChunk.size() == arrCmp.size()) && "buffrChunk and arrCmp have different size!!");

  // std::cout << "update bufferChunk\n";
  bufferChunk = arrCmp.select(value, bufferChunk);
  // std::cout << "END update bufferChunk\n";
}

static cachedDataMap<float> circleDataMap;
void circle(Eigen::Vector2f center, float radius, pixelValue value) {
  // graphics_printf("circle: <%.2f,%.2f> r=%.2f\n",center.x(),center.y(),radius);
  int radCeil = (int) std::ceil(radius);
  int radCeil2p1 = radCeil * 2+1;

  decltype(circleDataMap)::iterator circleMapIter(circleDataMap.lower_bound(radius));
  if (circleMapIter == circleDataMap.end() || radius < circleMapIter->first) { // not found
    // std::cout << "calc stuff for " << radCeil2p1 << "!" << std::endl;
    Eigen::MatrixXcf templateMat(radCeil2p1, radCeil2p1);
    templateMat.real() = Eigen::VectorXf::LinSpaced(radCeil2p1, -radCeil+0.5, radCeil+0.5).replicate(1, radCeil2p1);
    templateMat.imag() = templateMat.real().transpose();
    auto _arrCmp = (templateMat.cwiseAbs2().array() <= radius*radius).eval();
    // auto _ones = Eigen::DenseBase<PixelBuffer>::Ones(radCeil2p1,radCeil2p1).eval();
    circleMapIter = circleDataMap.insert(circleMapIter, std::make_pair(radius, _arrCmp));     // hinted insertion
  } else {
      // ... use circleMapIter->second here
    // if (i%1==0) std::cout << "pre calculated stuff for " << radCeil2p1 << "!" << std::endl;
  }
  
  int minX = int(center.x())-radCeil;
  int minY = int(center.y())-radCeil;

  int maxX = int(center.x())+radCeil;
  int maxY = int(center.y())+radCeil;

  applyCachedData(circleMapIter->second, minX, minY, maxX, maxY, radCeil2p1-1,radCeil2p1-1, value);
}

static cachedDataMap<std::pair<int,int>> rectDataMap;
void rect(Eigen::Vector2f cornerTL, Eigen::Vector2f dimsWH, pixelValue value) {
  // graphics_printf("rect: <%.2f,%.2f> d=<%.2f,%.2f>\n",cornerTL.x(),cornerTL.y(),dimsWH.x(),dimsWH.y());

  int w = std::ceil(dimsWH.x());
  int h = std::ceil(dimsWH.y());
  std::pair<int,int> dimsPair(w,h);
  decltype(rectDataMap)::iterator rectMapIter(rectDataMap.lower_bound(dimsPair));
  if (rectMapIter == rectDataMap.end() || dimsPair < rectMapIter->first) { // not found
    auto _arrCmp = Eigen::DenseBase<PixelBuffer>::Ones(w,h).eval();
    // auto _ones = _arrCmp;
    rectMapIter = rectDataMap.insert(rectMapIter, std::make_pair(dimsPair, _arrCmp));     // hinted insertion
  } else {
      // ... use rectMapIter->second here
    // if (i%1==0) std::cout << "pre calculated stuff for " << radCeil2p1 << "!" << std::endl;
  }

  int minX = std::round(cornerTL.x());
  int minY = std::round(cornerTL.y());

  int maxX = std::round(cornerTL.x()+dimsWH.x()-1);
  int maxY = std::round(cornerTL.y()+dimsWH.y()-1);

  // graphics_printf("rect from <%d,%d> to <%d,%d>\n", minX, minY, maxX, maxY);
  // graphics_printf("max inds: <%d,%d>\n", maxX-minX, maxY-minY);
  // graphics_printf("chunk dims: <%d,%d>\n", rectMapIter->second.first.rows(), rectMapIter->second.first.cols());

  applyCachedData(rectMapIter->second, minX, minY, maxX, maxY, maxX-minX, maxY-minY, value);
}
