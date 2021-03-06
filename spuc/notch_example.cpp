
// Copyright (c) 2014, Tony Kirke. License: MIT License (http://www.opensource.org/licenses/mit-license.php)
#include <iostream>
#include <fstream>
#include <spuc/fir_coeff.h>
#include <spuc/fir_decim.h>
#include <spuc/notch_iir.h>
#include <spuc/raised_cosine.h>
#include <spuc/notch_example.h>
namespace SPUC {
float_type notch_example(float_type x) {
  // these constants are for 11khz

  static notch_iir<float_type, float_type> N100(0.99, -0.99837);
  static notch_iir<float_type, float_type> N200(0.995, -0.9996);
  static notch_iir<float_type, float_type> N(0.995, -0.9996);

  static fir_coeff<float_type> HB1(17);
  static fir_coeff<float_type> HB2(17);

  static fir_decim<float_type> DB1(17);
  static fir_decim<float_type> DB2(17);

  static int init = 0;
  static int i = 0;
  static float_type res = 0;

  float_type f100, f200;
  float_type x1, x2;

  if (init == 0) {
    raised_cosine(HB1, 0.1, 2);
    raised_cosine(HB2, 0.1, 2);
    DB1.settaps(HB1);
    DB2.settaps(HB2);
    init = 1;
  }

  DB1.input(x);
  if (i % 2 == 0) {
    x1 = DB1.decim();
    DB2.input(x1);
    if (i % 4 == 0) {
      x2 = DB2.decim();
      f100 = N100.clock(x2);
      f200 = N200.clock(f100);
      res = N.clock(f200);
    }
  }

  return (res);
}
}
