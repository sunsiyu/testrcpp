#include <Rcpp.h>
using namespace Rcpp;

// Enable C++11
// [[Rcpp::plugins(cpp11)]]


//' Function to get the double of the value
//' 
//' Calculating the double of x, i.e. x times 2
//' 
//' @param x A single value.
//' @return y A single value, double of x
//' @export
// [[Rcpp::export]]
NumericVector timesTwo(NumericVector x) {
  return x * 2;
}

//' Example 1 Vector Input Scalar Output
//' 
//' Calculate mean of a numeric vector
//' 
//' @param x A numeric vector
//' @return y A double as mean
//' @export
// [[Rcpp::export]]
double meancpp(NumericVector x) {
  
  int n = x.size();
  double total = 0;
  
  for (int i = 0; i < n; i++) {
    total += x[i];
  }
  
  return total/n;
}

//' Example 2 Vector Input Vector Output
//' 
//' Calculate Euclidean distance
//' 
//' @param x A numeric number
//' @return y A numeric vector
//' @export
// [[Rcpp::export]]
NumericVector pdistcpp(double x, NumericVector ys) {
  
  int n = ys.size();
  NumericVector out(n);
  double total = 0;
  
  for (int i = 0; i < n; ++i) {
    out[i] = sqrt(pow(ys[i] - x, 2.0));
  }
  
  return out;
}