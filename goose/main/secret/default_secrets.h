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

#include <goose/common/common.h>

namespace goose {
class DatabaseInstance;
class ClientContext;
class BaseSecret;
struct CreateSecretInput;
class SecretManager;
struct SecretType;
class CreateSecretFunction;

struct CreateHTTPSecretFunctions {
public:
	//! Get the default secret types
	static vector<SecretType> GetDefaultSecretTypes();
	//! Get the default secret functions
	static vector<CreateSecretFunction> GetDefaultSecretFunctions();

protected:
	//! HTTP secret CONFIG provider
	static unique_ptr<BaseSecret> CreateHTTPSecretFromConfig(ClientContext &context, CreateSecretInput &input);
	//! HTTP secret ENV provider
	static unique_ptr<BaseSecret> CreateHTTPSecretFromEnv(ClientContext &context, CreateSecretInput &input);
};

} // namespace goose
