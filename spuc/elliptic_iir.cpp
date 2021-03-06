
// Copyright (c) 2014, Tony Kirke. License: MIT License (http://www.opensource.org/licenses/mit-license.php)
//! \author Tony Kirke
// from directory: spuc_src
#include <iostream>
#include <fstream>
#include <spuc/spuc_defines.h>
#include <cmath>
#include <cfloat>
#include <spuc/complex.h>
#include <spuc/elliptic_iir.h>
#define MACHEP 2.0 * FLT_MIN
#define ROOTMAX FLT_MAX
using namespace std;
namespace SPUC {
void elliptic_iir(iir_coeff& filt, float_type fcd, bool lpf, float_type fstop, float_type stopattn, float_type ripple) {
  float_type m1, m2;
  float_type a, m, Kk1, Kpk1, k, wr, fstp, Kk, u;
  const float_type ten = 10.0;

  long order = filt.order;
  float_type epi = pow(ten, (ripple / ten));
  epi = sqrt(epi - 1.0);
  //! wca - pre-warped angular frequency
  float_type bw = fstop - fcd;
  float_type wca = tan(PI * bw);
  float_type wc = tan(0.5 * PI * fcd);
  //! if stopattn < 1 dB assume it is stopband edge instead
  if (stopattn > 1.0) {
    a = pow(ten, (stopattn / ten));
    m1 = epi * epi / (a - 1.0);
    m2 = 1 - m1;
    Kk1 = ellpk(m1);
    Kpk1 = ellpk(m2);
    u = Kpk1 / (order * Kk1);
    k = msqrt(u);
    wr = 1.0 / k;
    fstp = atan(wca * wr) / PI;
  } else { fstp = stopattn; }
  wr = tan(fstp * PI) / wca;
  if (wr < 0.0) wr = -wr;
  k = 1.0 / wr;
  m = k * k;
  Kk = ellpk(m);
  u = lamda_plane(k, m, order, epi);
  s_plane(filt.poles, filt.zeros, lpf, order, u, m, k, Kk, wc);

  filt.bilinear();
  filt.convert_to_ab();
}
//! get roots in Lamda plane
float_type lamda_plane(float_type k, float_type m, int n, float_type eps) {
  float_type m1;
  float_type Kk;
  float_type Kpk;
  float_type Kk1;
  float_type u;
  float_type phi;

  Kk = ellpk(m);
  Kpk = ellpk(1 - m);
  u = (n * Kpk / Kk);
  m1 = msqrt(u);
  m1 *= m1;
  Kk1 = ellpk(m1);
  phi = atan(1.0 / eps);
  u = Kk * ellik(phi, 1.0 - m1) / (n * Kk1);
  return u;
}
//! calculate s plane poles and zeros
void s_plane(std::vector<complex<float_type> >& roots, std::vector<complex<float_type> >& zeros, bool lpf, int n,
             float_type u, float_type m, float_type k, float_type Kk, float_type wc) {
  float_type b;
  float_type sn1, cn1, dn1;
  float_type sn, cn, dn;
  float_type r;
  int i, j;
  float_type kon = Kk / (float_type)n;
  ellpj(u, 1.0 - m, sn1, cn1, dn1);
  for (i = 0; i < (n + 1) / 2; i++) {
    b = (n - 1 - 2 * i) * kon;
    j = (n - 1) / 2 - i;
    ellpj(b, m, sn, cn, dn);
    r = k * sn * sn1;
    r = 1.0 / (cn1 * cn1 + r * r);
    if (lpf) {
      if (sn != 0) { zeros[j] = -complex<float_type>(0.0, wc / (k * sn)); } else {
        zeros[j] = ROOTMAX;
      }
      roots[j] = -complex<float_type>(-wc * cn * dn * sn1 * cn1 * r, wc * sn * dn1 * r);
    } else {
      if (sn != 0) { zeros[j] = -complex<float_type>(0.0, -1.0 / (wc / (k * sn))); } else {
        zeros[j] = 0;
      }
      roots[j] = -1.0 / complex<float_type>(-wc * cn * dn * sn1 * cn1 * r, wc * sn * dn1 * r);
    }
  }
}
int ellpj(float_type u, float_type m, float_type& sn, float_type& cn, float_type& dn) {
  float_type ai, b, phi, t, twon;
  float_type a[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  float_type c[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int i;

  a[0] = 1.0;
  b = sqrt(1.0 - m);
  c[0] = sqrt(m);
  twon = 1.0;
  i = 0;

  while ((ABS(c[i] / a[i]) > MACHEP) & (i < 8)) {
    ai = a[i];
    ++i;
    c[i] = (ai - b) / 2.0;
    t = sqrt(ai * b);
    a[i] = (ai + b) / 2.0;
    b = t;
    twon *= 2.0;
  }

  phi = twon * a[i] * u;
  do {
    t = c[i] * sin(phi) / a[i];
    b = phi;
    phi = (asin(t) + phi) / 2.0;
  } while (--i);

  sn = sin(phi);
  t = cos(phi);
  cn = t;
  dn = t / cos(phi - b);
  return (0);
}
//  int ellik(float_type phi,float_type k,float_type *fe,float_type *ee);
//          phi -- argument in radians (pi/2 for complete elliptic integral)
//          k  -- (k) modulus, (0.0 - 1.0)
float_type ellik(float_type phi, float_type k) {
  float_type g, fac, a, b, c, d, a0, b0, d0, r;
  float_type fe;

  g = 0.0;
  a0 = 1.0;
  r = k * k;
  b0 = sqrt(1.0 - r);
  d0 = phi;
  if (k == 1.0) { return (log((1.0 + sin(d0)) / cos(d0))); } else {
    fac = 1.0;
    for (int n = 1; n < 41; n++) {
      a = 0.5 * (a0 + b0);
      b = sqrt(a0 * b0);
      c = 0.5 * (a0 - b0);
      fac *= 2.0;
      r += (fac * c * c);
      d = d0 + atan((b0 / a0) * tan(d0));
      g += (c * sin(d));
      d0 = d + PI * int(d / PI + 0.5);
      a0 = a;
      b0 = b;
      if (c < 1.0e-7) break;
    }
    fe = d / (fac * a);
  }
  return (fe);
}
float_type ellpk(float_type k) {
  float_type fac, a, b, c, a0, b0, r;
  float_type fe;
  a0 = 1.0;
  b0 = sqrt(1.0 - k);
  c = sqrt(k);
  if (k == 1.0)
    fe = FLT_MAX;
  else {
    fac = 0.5;
    r = fac * c * c;
    for (int n = 1; n < 41; n++) {
      a = 0.5 * (a0 + b0);
      c = 0.5 * (a0 - b0);
      b = sqrt(a0 * b0);
      fac *= 2.0;
      r += (fac * c * c);
      a0 = a;
      b0 = b;
      if (c < 1.0e-7) break;
    }
    fe = PI / (2.0 * a);
  }
  return (fe);
}
//! modulus from ratio of K/K'
float_type msqrt(float_type u) {
  float_type dt1;
  float_type a = 1.0;
  float_type b = 1.0;
  float_type r = 1.0;
  //! see http://www.physik-astro.uni-bonn.de/~dieckman/InfProd/InfProd.html
  //! calculate  theta2 / theta3
  //! m = (theta2/theta3) ^ 4
  float_type q = ::exp(-PI * u);
  float_type p = q;

  do {
    r *= p;
    a += 2.0 * r;
    dt1 = r / a;
    r *= p;
    b += r;
    p *= q;
  } while (dt1 > 1.0e-7);

  a = b / a;
  a = 4.0 * sqrt(q) * a * a;
  return (a);
}
}  // namespace SPUC
