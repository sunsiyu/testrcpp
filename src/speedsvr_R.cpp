#include <Rcpp.h>
#include <actbase.h>

#define GRAVITY 336         // equivalent to in-/decrease of 1 g
#define MAX_DURATION 100

// filter for the energy signal c(seq(1, 10), seq(10, 1))
Rcpp::NumericVector filter = Rcpp::NumericVector::create(1.0, 2.0, 3.0, 4.0,
		5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0,
		2.0, 1.0);

// Enable C++11
// [[Rcpp::plugins(cpp11)]]

/*
 * processRecording
 *
 * Extract step parameters for speed estimation from a recording
 *
 * Arguments:
 *   - ptr:      Rcpp::XPtr of recording object
 *   - steps:    data frame of steps to be processed (see GetSpeed for details)
 *
 * Returns:
 *   Rcpp::DataFrame parameters for every step:
 *     FMeanE.mean        average total acceleration for step
 *     FSumAbsX.mean      sum of absolute acceleration on X axis
 *     FSumAbsY.mean      sum of absolute acceleration on Y axis
 *     FSumAbsZ.mean      sum of absolute acceleration on Z axis
 */
// [[Rcpp::interfaces(r, cpp)]]
// [[Rcpp::export]]
Rcpp::DataFrame processRecording(SEXP ptr, Rcpp::DataFrame steps) {

	filter = filter / sum(filter);

	// initializing return values
	Rcpp::NumericVector FMeanE(steps.nrows());
	Rcpp::NumericVector FSumAbsX(steps.nrows());
	Rcpp::NumericVector FSumAbsY(steps.nrows());
	Rcpp::NumericVector FSumAbsZ(steps.nrows());

	// get relevant parameters
	Rcpp::IntegerVector indices = steps["time"];
	Rcpp::IntegerVector stepduration = steps["duration"];

	Rcpp::NumericMatrix m;
	Rcpp::NumericVector energySignal(MAX_DURATION + filter.length());

	for (uint64_t i = 0; i < indices.size(); i++) {
		// cannot calculate speed if duration is NA
		if (Rcpp::NumericVector::is_na(stepduration[i])) {
			FMeanE[i] = NA_REAL;
			FSumAbsX[i] = NA_REAL;
			FSumAbsY[i] = NA_REAL;
			FSumAbsZ[i] = NA_REAL;
			continue;
		}

		uint64_t index = indices[i];
		int duration = stepduration[i];

		// step out of range for filtering?
		if ((index + duration + filter.length() / 2) > actbase::ab_getRows(ptr)
				|| index < filter.length() / 2) {
			FMeanE[i] = NA_REAL;
			FSumAbsX[i] = NA_REAL;
			FSumAbsY[i] = NA_REAL;
			FSumAbsZ[i] = NA_REAL;
			continue;
		}

		// to correctly filter the signal we need half the length of additional
		// values each before the index and after index + duration
		m = actbase::ab_readAt(ptr, index - filter.length() / 2,
				filter.length() + duration);

		for (int j = 0; j < duration + filter.length(); j++) {
			if (j < duration + filter.length() / 2
					&& j >= filter.length() / 2) {
				FSumAbsX[i] += (double) abs(m(j, 0)) / GRAVITY;
				FSumAbsY[i] += (double) abs(m(j, 1)) / GRAVITY;
				FSumAbsZ[i] += (double) abs(m(j, 2)) / GRAVITY;
			}

			energySignal[j] = sqrt(
					pow((double) m(j, 0) + GRAVITY, 2) +
					pow((double) m(j, 1), 2) +
					pow((double) m(j, 2), 2));
		}

		// now we can filter the signal
		for (int j = 0; j < duration; j++) {
			for (int k = 0; k < filter.length(); k++) {
				FMeanE[i] += filter[k] * energySignal[j + filter.length() - k];
			}
		}

		FMeanE[i] = FMeanE[i] / duration;
	}

	Rcpp::DataFrame out = Rcpp::DataFrame::create(Rcpp::Named("FMeanE.mean") =
			FMeanE, Rcpp::Named("FSumAbsX.mean") = FSumAbsX,
			Rcpp::Named("FSumAbsY.mean") = FSumAbsY,
			Rcpp::Named("FSumAbsZ.mean") = FSumAbsZ);
	return (out);
}
