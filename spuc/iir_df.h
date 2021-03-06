#ifndef SPUC_IIR_DF
#define SPUC_IIR_DF

// Copyright (c) 2014, Tony Kirke. License: MIT License (http://www.opensource.org/licenses/mit-license.php)
// from directory: spuc_double_templates
#include <spuc/spuc_types.h>
#include <spuc/fir.h>
#include <iostream>
#include <fstream>
#include <spuc/quant.h>
#include <spuc/complex.h>
namespace SPUC {
//! \file
//!  \brief Template Class for Modeling a Direct Form IIR
//
//!  \brief Template Class for Modeling a Direct Form IIR
//
//!  Template works for float_type, long, complex, etc
//!  Taps for FIR and IIR parts are typically loaded from two seperate files
//!  Assumed that IIR part Tap 0 = 1 and is not in file.
//!  Implemented as cascade of FIR part followed by IIR part
//!  IIR function implemented with FIR class using special iir function
//! \author Tony Kirke
//!  \ingroup double_templates iir
template <class Numeric, class Coeff = float_type> class iir_df {
 public:
  fir<Numeric, Coeff> poles;
  fir<Numeric, Coeff> zeros;
  Numeric output;
  int err;

 public:
  //! Reset
  void reset() {
    poles.reset();
    zeros.reset();
    output = (Numeric)0;
  }
  //! Get current output
  Numeric out() { return (output); }
  ~iir_df(void) {}
  //! Constructor
  iir_df(const char* filep, const char* filez) {
    err = 0;
    err = poles.read_taps(filep);
    err += zeros.read_taps(filez);
  }
  iir_df<Numeric, Coeff>(const iir_df<Numeric, Coeff>& cp) : poles(cp.poles.num_taps), zeros(cp.zeros.num_taps) {
    int i;
    for (i = 0; i < cp.poles.num_taps; i++) poles.coeff[i] = cp.poles.coeff[i];
    for (i = 0; i < cp.zeros.num_taps; i++) zeros.coeff[i] = cp.zeros.coeff[i];
  }
  //  iir_df(void) : poles(NULL), zeros(NULL) {; }
  Numeric clock(Numeric in) { return (update(in)); }
  Numeric update(Numeric in) {
    if (err)
      return ((Numeric)0);
    else {
      output = zeros.update(in);
      output = poles.iir(output);
      return (output);
    }
  }
  void print(void) {
    poles.print();
    zeros.print();
  }
};
}  // namespace SPUC
#endif
