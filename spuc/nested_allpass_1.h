#ifndef SPUC_NESTED_ALLPASS_1
#define SPUC_NESTED_ALLPASS_1

// Copyright (c) 2014, Tony Kirke. License: MIT License (http://www.opensource.org/licenses/mit-license.php)
// from directory: spuc_double_templates
#include <spuc/spuc_types.h>
#include <iostream>
#include <fstream>
#include <spuc/spuc_types.h>
#include <spuc/complex.h>
#include <spuc/quantiser.h>
#include <spuc/mixed_type.h>
#include <spuc/delay.h>
namespace SPUC {

template <class Numeric, class Coeff = float_type> class nested_allpass_1 {
 protected:
  //! number of unit sample delays
  long size;
  long round_bits;
  float_type gain;
  float_type vgain;
  float_type prev_sum;
  float_type prev_mult;
  allpass_1<Numeric> mult;
  allpass_1<Numeric> sum;
  typedef typename mixed_type<Numeric, Coeff>::dtype sum_type;

 public:
  //! Constructor with gain and delay
  nested_allpass_1(Coeff Gain = 0, Coeff VGain = 0) : mult(VGain), sum(VGain) {
    gain = Gain;
    prev_mult = prev_sum = 0;
  }
  Coeff get_coefficient(void) { return ((Coeff)(gain)); }
  //! Reset
  void reset() {
    mult.reset();
    sum.reset();
  }
  //! Initializes gain and delay
  void init(Coeff Gain, Coeff VGain, long s) {
    gain = Gain;
    vgain = VGain;
    size = s;
    mult.init(VGain, s);
    sum.init(VGain, s);
  }
  //! Shift inputs by one time sample and place new sample into array
  Numeric clock(Numeric input) {
    Numeric current_mult, out;
    Numeric current_sum;

    out = prev_sum;
    current_sum = (input + prev_mult);
    current_mult = -gain * current_sum;

    out -= current_mult;
    // update delay lines
    prev_mult = mult.clock(current_mult);
    prev_sum = sum.clock(current_sum);
    return (out);
  }
};

}  // namespace SPUC
#endif
