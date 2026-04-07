// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
