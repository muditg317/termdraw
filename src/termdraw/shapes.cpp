#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>
#include <utils.hpp>

#include <Eigen/Eigen>
#include <iostream>
#include <map>
#include <memory>
#include <optional>

void circle(float x, float y, float r) {
  return circle(Eigen::Vector2f(x,y), r);
}

void circle1(Eigen::Vector2f center, float radius) {
  // std::cout  << "circle at <" << center.x() << ',' << center.y() << "> | r=" << radius << std::endl;
  Eigen::Vector2i topLeft = (center - Eigen::Vector2f(radius, radius)).array().floor().cast<int>();
  Eigen::Vector2i bottomRight = (center + Eigen::Vector2f(radius, radius)).array().ceil().cast<int>();

  for (int y = topLeft.y(); y <= bottomRight.y(); ++y) {
    for (int x = topLeft.x(); x <= bottomRight.x(); ++x) {
      if (0 <= x && x < WIDTH && 0 <= y && y < HEIGHT) {
        Eigen::Vector2f centerOfPixel = Eigen::Vector2f(x,y) + Eigen::Vector2f(0.5, 0.5);
        Eigen::Vector2f delta = center - centerOfPixel;
        if (delta.squaredNorm() <= radius*radius) {
          BUFFER(x,y) = ON;
        }
      }
    }
  }
}

// typedef Eigen::ArrayBase<...>::CmpLEReturnType ArrayCompType;
// typedef decltype(ret(Eigen::MatrixXcf::array)) ArrT;
// typedef ArrT::Base::CmpLEReturnType sad;
// typedef decltype(ret(ArrT::operator<=)) ArrTCmp;
typedef const Eigen::CwiseBinaryOp<
  Eigen::internal::scalar_cmp_op<float,float, Eigen::internal::cmp_LE>,
  const Eigen::ArrayWrapper<
    const Eigen::CwiseUnaryOp<Eigen::internal::scalar_abs2_op<std::complex<float>>,
    const Eigen::MatrixXcf >
  >,
  const Eigen::CwiseNullaryOp<
    Eigen::internal::scalar_constant_op<float>,
    Eigen::ArrayXXf::Base::PlainObject
  >
> ArrayCompTypeBig;
typedef Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> ArrayCompType;
typedef Eigen::CwiseNullaryOp<
  Eigen::internal::scalar_constant_op<bool>,
  Eigen::Matrix<bool, -1, -1>
> MatrixXbConstBig;
typedef Eigen::Matrix<bool, -1, -1> MatrixXb;
typedef std::pair<ArrayCompType,MatrixXb> circleData;
std::map<int, circleData, std::less<int>, Eigen::aligned_allocator<std::pair<const int, circleData>>> circleDataMap;

// std::optional<ArrayCompType> arrCmp;
// std::optional<MatrixXb> ones;
// ArrayCompType arrCmp = Eigen::DenseBase<PixelBuffer>::Zero(5,5);
// MatrixXb ones = Eigen::DenseBase<PixelBuffer>::Zero(5,5);
// float lastRadius = -1;

void circle2(Eigen::Vector2f center, float radius) {
  static int i = 0;
  i++;
  int radCeil = (int) std::ceil(radius);
  int radCeil2p1 = radCeil * 2+1;


  bool found = true;
  decltype(circleDataMap)::iterator circleMapIter(circleDataMap.lower_bound(radCeil2p1));
  if (circleMapIter == circleDataMap.end() || radCeil2p1 < circleMapIter->first) { // not found
  // if (lastRadius != radius) {
    found = false;
    // std::cout << "calc stuff for " << radCeil2p1 << "!" << std::endl;
    Eigen::MatrixXcf templateMat ( radCeil2p1, radCeil2p1);
    // auto tiledVec = (new Eigen::CwiseNullaryOp<Eigen::internal::linspaced_op<float>,Eigen::VectorXf::Base::PlainObject>(Eigen::VectorXf::LinSpaced(radCeil2p1, -radCeil+0.5, radCeil+0.5))) -> replicate(1, radCeil2p1);
    // templateMat->real() = *(new decltype(tiledVec)(tiledVec));
    // float minX = std::max(-radCeil, 0)+0.5;
    // float maxX = std::min(radCeil, WIDTH)+0.5;
    templateMat.real() = Eigen::VectorXf::LinSpaced(radCeil2p1, -radCeil+0.5, radCeil+0.5).replicate(1, radCeil2p1);
    // auto trans = tiledVec.transpose();
    // templateMat->imag() = *(new decltype(trans)(trans));
    templateMat.imag() = templateMat.real().transpose();
    // auto arrCmpXpr = templateMat->cwiseAbs2().array();
    // std::shared_ptr<ArrayCompType> arrCmpPtr(new ArrayCompType((*(new decltype(arrCmpXpr)(arrCmpXpr))) <= radius*radius));
    auto _arrCmp = (templateMat.cwiseAbs2().array() <= radius*radius).eval();
    // arrCmp = *(new decltype(_arrCmp)(_arrCmp));
    // std::cout << "arrCmp before map insertion: " << &_arrCmp << '\n' << _arrCmp << std::endl;
    // arrCmp.emplace(_arrCmp);
    // arrCmp.reshaped()
    // arrCmp = _arrCmp;
    // std::cout << "arrCmp after map insertion: " << &arrCmp.value() << '\n' << arrCmp.value() << std::endl;
    // std::shared_ptr<MatrixXb> onesPtr(new MatrixXb(Eigen::DenseBase<PixelBuffer>::Ones(radCeil2p1,radCeil2p1)));
    auto _ones = Eigen::DenseBase<PixelBuffer>::Ones(radCeil2p1,radCeil2p1).eval();
    // ones.emplace(_ones);
    // ones = _ones;
    circleMapIter = circleDataMap.insert(circleMapIter, std::make_pair(radCeil2p1, circleData(_arrCmp,_ones)));     // hinted insertion
    // std::cout << "right after map insertion: " << &(circleMapIter->second.first) << '\n' << (circleMapIter->second.first) << std::endl;
    // lastRadius = radius;
  } else {
      // ... use circleMapIter->second here
    // if (i%1==0) std::cout << "pre calculated stuff for " << radCeil2p1 << "!" << std::endl;
  }
  // std::cout << "first:\n" << circleMapIter->second.first << "\n";
  // if (!found) {
    // std::cout << "second:\n" << circleMapIter->second.second << "ones\n";
  // }

  // if (!circleDataMap.count(radCeil2p1)) {
  //   std::cout << "circle map still doesn't contains the thing..." << std::endl;
  // } else {

  // }

  int minX = int(center.x())-radius;
  int minY = int(center.y())-radius;
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

  int maxX = int(center.x())+radius;
  int maxY = int(center.y())+radius;
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


  // auto bufferChunk = BUFFER.block(center.x()-radius,center.y()-radius,radCeil2p1,radCeil2p1);
  auto bufferChunk = BUFFER(Eigen::seq(minX,maxX),Eigen::seq(minY,maxY));//.block(minX,minY,maxX-minX+1,maxY-minY+1);
  // std::cout << templateMat << "\n==" << templateMat.cwiseAbs2() << "\ngay" << radius*radius << std::endl;

  circleData pair = circleMapIter->second;

  auto arrCmp = pair.first(Eigen::seq(dataMinX,dataMaxX),Eigen::seq(dataMinY,dataMaxY));
  // if (i%1==0) std::cout << "on access arrCmpFromMap: " << &arrCmp.value() << '\n';
  // if (i%1==0) std::cout << arrCmp.value() << std::endl;
  auto ones = pair.second(Eigen::seq(dataMinX,dataMaxX),Eigen::seq(dataMinY,dataMaxY));
  // std::cout << "on access onesFromMap:   " << &onesFromMap << '\n';
  // std::cout << onesFromMap << std::endl;


  if (bufferChunk.size() != arrCmp.size()) {
    printf("circle %d: <%.2f,%.2f> r=%.2f\n",i,center.x(),center.y(),radius);
    printf("pixels: <%d,%d> to <%d,%d>\n",minX,minY,maxX,maxY);
    printf("data: <%d,%d> to <%d,%d>\n",dataMinX,dataMinY,dataMaxX,dataMaxY);
    std::cout << "buffer:" << bufferChunk.size() << "\n" << bufferChunk << std::endl;
    std::cout << "arrCmp:" << arrCmp.size() << "\n" << arrCmp << std::endl;
    // std::cout << "ones:" << ones.size() << "\n" << ones << std::endl;
  }
  // std::cout << "update bufferChunk\n";
  // bufferChunk = arrCmpFromMap.select(onesFromMap, bufferChunk);
  bufferChunk = arrCmp.select(ones, bufferChunk);
  // std::cout << "END update bufferChunk\n";

}

void circle(Eigen::Vector2f center, float radius) {
  circle2(center, radius);
}