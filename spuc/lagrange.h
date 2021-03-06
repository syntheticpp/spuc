#ifndef SPUC_LAGRANGE
#define SPUC_LAGRANGE

// Copyright (c) 2014, Tony Kirke. License: MIT License (http://www.opensource.org/licenses/mit-license.php)
// from directory: spuc_double_templates
#include <spuc/spuc_types.h>
#include <iostream>
#include <fstream>
#include <spuc/complex.h>
#include <spuc/quant.h>
#include <spuc/quantiser.h>
#include <spuc/mixed_type.h>
#include <vector>
namespace SPUC {
//! \file
//! \brief  Template Class for Lagrange interpolation using a FIR  filter.
//
//! \brief  Template Lagrange interpolation via FIR Fitler
//
//! This works best for float_type or complex<float_type>
//! Coefficients are always real (i.e. no imaginary parts).
//! \author Tony Kirke
//! \image html lagrange.png
//! \ingroup double_templates interpolation
//! \author Tony Kirke
//! \image html lagrange.png
//! \ingroup double_templates fir
//! \image html lagrange.gif
//! \image latex lagrange.eps
template <class Numeric, class Coeff = float_type> class lagrange {
 public:
  long num_taps;
  std::vector<Coeff> coeff;

 protected:
  std::vector<Numeric> z;
  Numeric output;
  quantiser<Numeric> Q;
  long round_bits;
  typedef typename mixed_type<Numeric, Coeff>::dtype sum_type;

 public:
  //!
  void reset() {
    for (int i = 0; i <= num_taps; i++) z[i] = (Numeric)0;
    output = (Numeric)0;
  }
  //! Get current output
  Numeric out() { return (output); }
  //! Clock in new sample & compute current output
  Numeric check(long i) { return (z[i]); }

  lagrange(void) : Q(0){};
  //! Constructor
  lagrange(long n, long b = 0) : num_taps(n), coeff(n + 1), z(n + 1), Q(b), round_bits(b) {
    int i;
    SPUC_ASSERT(n < 0);
    for (i = 0; i <= n; i++) {
      z[i] = (Numeric)0;
      coeff[i] = (Coeff)0;
    }
  }
  //! Destructor
  ~lagrange(void) {}
  //! Set order of polynomial
  void set_size(long n) {
    SPUC_ASSERT(n < 0);
    int i;
    num_taps = n;
    coeff.resize(n + 1);
    z.resize(n + 1);
    for (i = 0; i <= n; i++) {
      z[i] = (Numeric)0;
      coeff[i] = (Coeff)0;
    }
  }
  //! Input new sample (do nothing else)
  void input(Numeric in) {
    int i;
    // Update history of inputs
    for (i = num_taps; i > 0; i--) z[i] = z[i - 1];
    // Add new input
    z[0] = in;
  }
  /**
         Update => Clock in new input sample, recalculate coefficients and
         determine output
  */
  Numeric update(Numeric in, float_type offset) {
    int i;
    // Update history of inputs
    for (i = num_taps; i > 0; i--) z[i] = z[i - 1];
    // Add new input
    z[0] = in;
    // Calculate coefficients
    calculate_coeff(offset);
    // Perform FIR
    return (fir());
  }
  //! Calculate output for current coefficients
  Numeric fir(void) {
    int i;
    sum_type sum(0);
    // Perform FIR
    for (i = 0; i <= num_taps; i++) sum = sum + (coeff[i] * z[i]);
    output = Q(sum);
    return (output);
  }
  //! Rephase => recalculate coefficients and output  for new offset (for
  // upsampling)
  Numeric rephase(float_type offset) {
    // Calculate coefficients
    calculate_coeff(offset);
    // Perform FIR
    return (fir());
  }
  //! Calculate coefficients given an offset
  void calculate_coeff(float_type offset) {
    int i, k;
    float_type val;
    // Calculate coefficientsj
    float_type off = offset + num_taps / 2;
    for (k = 0; k <= num_taps; k++) {
      val = 1;
      for (i = 0; i <= num_taps; i++)
        if (k != i) val *= float_type(i - off) / float_type(i - k);
      coeff[num_taps - k] = val;
    }
  }
  //!  Print out coefficients
  void print() {
    std::cout << "Lagrange coefficients ";
    for (long i = 0; i <= num_taps; i++) {
      std::cout << coeff[i] << ' ';
      if ((i + 1) % 6 == 0) std::cout << '\n';
    }
    std::cout << '\n';
    std::cout.flush();
  }
};
// template_instantiations: long; complex<long>; float_type; complex<float_type>
}  // namespace SPUC
#endif
