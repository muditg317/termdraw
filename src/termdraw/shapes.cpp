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
  int radCeil2 = radCeil * 2;


  bool found = true;
  decltype(circleDataMap)::iterator circleMapIter(circleDataMap.lower_bound(radCeil2));
  if (circleMapIter == circleDataMap.end() || radCeil2 < circleMapIter->first) { // not found
  // if (lastRadius != radius) {
    found = false;
    // std::cout << "calc stuff for " << radCeil2 << "!" << std::endl;
    Eigen::MatrixXcf templateMat ( radCeil2, radCeil2);
    // auto tiledVec = (new Eigen::CwiseNullaryOp<Eigen::internal::linspaced_op<float>,Eigen::VectorXf::Base::PlainObject>(Eigen::VectorXf::LinSpaced(radCeil2, -radCeil+0.5, radCeil+0.5))) -> replicate(1, radCeil2);
    // templateMat->real() = *(new decltype(tiledVec)(tiledVec));
    // float minX = std::max(-radCeil, 0)+0.5;
    // float maxX = std::min(radCeil, WIDTH)+0.5;
    templateMat.real() = Eigen::VectorXf::LinSpaced(radCeil2, -radCeil+0.5, radCeil-0.5).replicate(1, radCeil2);
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
    // std::shared_ptr<MatrixXb> onesPtr(new MatrixXb(Eigen::DenseBase<PixelBuffer>::Ones(radCeil2,radCeil2)));
    auto _ones = Eigen::DenseBase<PixelBuffer>::Ones(radCeil2,radCeil2).eval();
    // ones.emplace(_ones);
    // ones = _ones;
    circleMapIter = circleDataMap.insert(circleMapIter, std::make_pair(radCeil2, circleData(_arrCmp,_ones)));     // hinted insertion
    // std::cout << "right after map insertion: " << &(circleMapIter->second.first) << '\n' << (circleMapIter->second.first) << std::endl;
    // lastRadius = radius;
  } else {
      // ... use circleMapIter->second here
    // if (i%1==0) std::cout << "pre calculated stuff for " << radCeil2 << "!" << std::endl;
  }
  // std::cout << "first:\n" << circleMapIter->second.first << "\n";
  // if (!found) {
    // std::cout << "second:\n" << circleMapIter->second.second << "ones\n";
  // }

  // if (!circleDataMap.count(radCeil2)) {
  //   std::cout << "circle map still doesn't contains the thing..." << std::endl;
  // } else {

  // }

  auto bufferChunk = BUFFER.block(center.x()-radius,center.y()-radius,radCeil2,radCeil2);
  // std::cout << templateMat << "\n==" << templateMat.cwiseAbs2() << "\ngay" << radius*radius << std::endl;

  circleData pair = circleMapIter->second;

  auto arrCmp = pair.first;
  // if (i%1==0) std::cout << "on access arrCmpFromMap: " << &arrCmp.value() << '\n';
  // if (i%1==0) std::cout << arrCmp.value() << std::endl;
  auto ones = pair.second;
  // std::cout << "on access onesFromMap:   " << &onesFromMap << '\n';
  // std::cout << onesFromMap << std::endl;

  // std::cout << "update bufferChunk\n";
  // bufferChunk = arrCmpFromMap.select(onesFromMap, bufferChunk);
  bufferChunk = arrCmp.select(ones, bufferChunk);
  // std::cout << "END update bufferChunk\n";

}

void circle(Eigen::Vector2f center, float radius) {
  circle2(center, radius);
}