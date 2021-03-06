
// Copyright (c) 2014, Tony Kirke. License: MIT License (http://www.opensource.org/licenses/mit-license.php)
//! \author Tony Kirke
// from directory: spuc_src
#include <cmath>
#include <spuc/spuc_defines.h>
#include <spuc/complex.h>
#include <spuc/fir_coeff.h>
#include <spuc/gaussian_fir.h>
namespace SPUC {
//! \file
//! \brief calculates the sampled Gaussian filter impulse response
void gaussian_fir(fir_coeff<float_type>& gaussf, float_type bt, float_type spb) {
  // generate the sampled Gaussian filter impulse response
  // and transfer coefficients to "gaussf".
  // spb   = samples per bit (at full rate)
  // BT    = filter bandwidth
  int i;
  float_type s, t0, ts, gsum;
  int nir = gaussf.num_taps;
  float_type* gf = new float_type[nir];
  // dt = 1.0/spb;
  s = 1.0 / (sqrt(log(2.0)) / (TWOPI * bt));
  t0 = -0.5 * nir;
  gsum = 0.0;  // normalize to unit sum
  for (i = 0; i < nir; i++) {
    t0++;
    ts = s * t0;
    gf[i] = ::exp(-0.5 * ts * ts);
    gsum += gf[i];
  }
  // Unity DC gain
  gsum = 1.0 / gsum;
  for (i = 0; i < nir; i++) gaussf.coeff[i] = gf[i] * gsum;
}
}  // namespace SPUC
