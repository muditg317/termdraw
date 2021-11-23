#include <termdraw/graphics.hpp>
#include <termdraw/shapes.hpp>
#include <utils.hpp>

#include <Eigen/Eigen>
#include <iostream>
#include <map>
#include <memory>

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
> ArrayCompType;
typedef Eigen::CwiseNullaryOp<Eigen::internal::scalar_constant_op<bool>, Eigen::Matrix<bool, -1, -1>> MatrixXbConst;
typedef std::pair<std::shared_ptr<ArrayCompType>,std::shared_ptr<MatrixXbConst>> circleData;
std::map<int, circleData> circleDataMap;

void circle2(Eigen::Vector2f center, float radius) {
  int radCeil = (int) std::ceil(radius);
  int radCeil2p1 = radCeil * 2 + 1;

  // if (circleDataMap.contains(radCeil2p1)) {
  //   std::cout << "calc stuff for 15!" << std::endl;
  // }
  bool found = true;
  decltype(circleDataMap)::iterator circleMapIter(circleDataMap.lower_bound(radCeil2p1));
  // if (circleMapIter == circleDataMap.end() || radCeil2p1 < circleMapIter->first) { // not found
    found = false;
    std::cout << "calc stuff for " << radCeil2p1 << "!" << std::endl;
    std::shared_ptr<Eigen::MatrixXcf> templateMat(new Eigen::MatrixXcf(radCeil2p1, radCeil2p1));
    auto tiledVec = (new Eigen::CwiseNullaryOp<Eigen::internal::linspaced_op<float>,Eigen::VectorXf::Base::PlainObject>(Eigen::VectorXf::LinSpaced(radCeil2p1, -radCeil+0.5, radCeil+0.5))) -> replicate(1, radCeil2p1);
    templateMat->real() = *(new decltype(tiledVec)(tiledVec));
    auto trans = tiledVec.transpose();
    templateMat->imag() = *(new decltype(trans)(trans));
    auto arrCmpXpr = templateMat->cwiseAbs2().array();
    std::shared_ptr<ArrayCompType> arrCmpPtr(new ArrayCompType((*(new decltype(arrCmpXpr)(arrCmpXpr))) <= radius*radius));
    // ArrayCompType arrCmp = (templateMat.cwiseAbs2().array() <= radius*radius);
    std::shared_ptr<MatrixXbConst> onesPtr(new MatrixXbConst(Eigen::DenseBase<PixelBuffer>::Ones(radCeil2p1,radCeil2p1)));
    // MatrixXbConst ones = Eigen::DenseBase<PixelBuffer>::Ones(radCeil2p1,radCeil2p1);
    std::cout << "arrCmp before map insertion: " << arrCmpPtr << '\n' << *arrCmpPtr << std::endl;
    circleMapIter = circleDataMap.insert(circleMapIter, std::make_pair(radCeil2p1, circleData(arrCmpPtr,onesPtr)));     // hinted insertion
    std::cout << "right after map insertion: " << circleMapIter->second.first << '\n' << *(circleMapIter->second.first) << std::endl;

  // } else {
  //     // ... use circleMapIter->second here
  //   std::cout << "pre calculated stuff for " << radCeil2p1 << "!" << std::endl;
  // }
  // std::cout << "first:\n" << circleMapIter->second.first << "\n";
  // if (!found) {
    // std::cout << "second:\n" << circleMapIter->second.second << "ones\n";
  // }

  // if (!circleDataMap.count(radCeil2p1)) {
  //   std::cout << "circle map still doesn't contains the thing..." << std::endl;
  // } else {

  // }

  auto bufferChunk = BUFFER.block(center.x()-radius,center.y()-radius,radCeil2p1,radCeil2p1);
  // std::cout << templateMat << "\n==" << templateMat.cwiseAbs2() << "\ngay" << radius*radius << std::endl;

  circleData pair = circleMapIter->second;

  auto arrCmp = pair.first;
  std::cout << "on access arrCmp: " << arrCmp << '\n';
  std::cout << *arrCmp << std::endl;
  auto ones = pair.second;
  std::cout << "on access ones:   " << ones << '\n';
  std::cout << *ones << std::endl;

  std::cout << "update bufferChunk\n";
  bufferChunk = arrCmp->select(*ones, bufferChunk);
  std::cout << "END update bufferChunk\n";

}

void circle(Eigen::Vector2f center, float radius) {
  circle2(center, radius);
}