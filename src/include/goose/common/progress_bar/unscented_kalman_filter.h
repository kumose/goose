// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <goose/goose.h>
#include <chrono>

namespace goose {

class UnscentedKalmanFilter {
private:
	static constexpr size_t STATE_DIM = 2; // [progress, velocity]
	static constexpr size_t OBS_DIM = 1;   // [progress]
	static constexpr size_t SIGMA_POINTS = 2 * STATE_DIM + 1;

	// UKF parameters
	static constexpr double ALPHA = 0.044;
	static constexpr double BETA = 1.0;
	static constexpr double KAPPA = 0.0;

	double lambda;
	vector<double> wm, wc; // weights for mean and covariance

	// State: [progress (0-1), velocity (progress/second)]
	vector<double> x;         // state estimate
	vector<vector<double>> P; // covariance matrix
	vector<vector<double>> Q; // process noise
	vector<vector<double>> R; // measurement noise

	double last_time;
	bool initialized;
	double last_progress;
	double scale_factor;

	// Helper functions
	vector<vector<double>> MatrixSqrt(const vector<vector<double>> &mat);
	vector<vector<double>> GenerateSigmaPoints();
	vector<double> StateTransition(const vector<double> &state, double dt);
	vector<double> MeasurementFunction(const vector<double> &state);

public:
	UnscentedKalmanFilter();

	void Update(double progress, double time);

	double GetEstimatedRemainingSeconds() const;

private:
	void Initialize(double initial_progress, double current_time);
	void Predict(double current_time);
	void UpdateInternal(double measured_progress);

	double GetProgress() const;
	double GetVelocity() const;
	double GetProgressVariance() const;
	double GetVelocityVariance() const;
};

} // namespace goose
