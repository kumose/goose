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

#include <goose/catalog/default/default_generator.h>
#include <goose/common/common.h>
#include <goose/main/secret/secret.h>
#include <goose/main/secret/secret_manager.h>
#include <goose/main/secret/secret_storage.h>
#include <goose/parser/parsed_data/create_secret_info.h>

namespace goose {
class SecretManager;
struct DBConfig;
class SchemaCatalogEntry;

//! A Secret Entry in the secret manager
struct SecretEntry {
public:
	explicit SecretEntry(unique_ptr<const BaseSecret> secret) : secret(secret != nullptr ? secret->Clone() : nullptr) {
	}
	SecretEntry(const SecretEntry &other)
	    : persist_type(other.persist_type), storage_mode(other.storage_mode),
	      secret((other.secret != nullptr) ? other.secret->Clone() : nullptr) {
	}

	//! Whether the secret is persistent
	SecretPersistType persist_type;
	//! The storage backend of the secret
	string storage_mode;
	//! The secret pointer
	unique_ptr<const BaseSecret> secret;
};

//! Return value of a Secret Lookup
struct SecretMatch {
public:
	SecretMatch() : secret_entry(nullptr), score(NumericLimits<int64_t>::Minimum()) {
	}

	SecretMatch(const SecretMatch &other)
	    : secret_entry((other.secret_entry != nullptr) ? make_uniq<SecretEntry>(*other.secret_entry) : nullptr),
	      score(other.score) {
	}

	SecretMatch(SecretEntry &secret_entry, int64_t score)
	    : secret_entry(make_uniq<SecretEntry>(secret_entry)), score(score) {
	}

	SecretMatch &operator=(const SecretMatch &other) {
		this->secret_entry = (other.secret_entry != nullptr) ? make_uniq<SecretEntry>(*other.secret_entry) : nullptr;
		this->score = other.score;
		return *this;
	};

	//! Get the secret
	const BaseSecret &GetSecret() const;

	bool HasMatch() {
		return secret_entry != nullptr;
	}

	unique_ptr<SecretEntry> secret_entry;
	int64_t score;
};

struct SecretManagerConfig {
	static constexpr const bool DEFAULT_ALLOW_PERSISTENT_SECRETS = true;
	//! The default persistence type for secrets
	SecretPersistType default_persist_type = SecretPersistType::TEMPORARY;
	//! Secret Path can be changed by until the secret manager is initialized, after that it will be set automatically
	string secret_path = "";
	//! The default secret path is determined on startup and can be used to reset the secret path
	string default_secret_path = "";
	//! The storage type for persistent secrets when none is specified;
	string default_persistent_storage = "";
	//! Persistent secrets are enabled by default
	bool allow_persistent_secrets = DEFAULT_ALLOW_PERSISTENT_SECRETS;
};

//! The Secret Manager for Goose. Can handle both temporary and persistent secrets
class SecretManager {
	friend struct SecretEntry;

public:
	explicit SecretManager() = default;
	virtual ~SecretManager() = default;

	//! The default storage backends
	static constexpr const char *TEMPORARY_STORAGE_NAME = "memory";
	static constexpr const char *LOCAL_FILE_STORAGE_NAME = "local_file";

	//! Static Helper Functions
	GOOSE_API static SecretManager &Get(ClientContext &context);
	GOOSE_API static SecretManager &Get(DatabaseInstance &db);

	// Initialize the secret manager with the DB instance
	GOOSE_API void Initialize(DatabaseInstance &db);
	//! Load a secret storage
	GOOSE_API void LoadSecretStorage(unique_ptr<SecretStorage> storage);

	//! Deserialize a secret by automatically selecting the correct deserializer, secret_path can be set to improve
	//! error hints
	GOOSE_API unique_ptr<BaseSecret> DeserializeSecret(Deserializer &deserializer, const string &secret_path = "");
	//! Register a new SecretType
	GOOSE_API void RegisterSecretType(SecretType &type);
	//! Lookup a SecretType
	GOOSE_API SecretType LookupType(const string &type);
	//! Register a Secret Function i.e. a secret provider for a secret type
	GOOSE_API void RegisterSecretFunction(CreateSecretFunction function, OnCreateConflict on_conflict);
	//! Register a secret by providing a secret manually
	GOOSE_API unique_ptr<SecretEntry> RegisterSecret(CatalogTransaction transaction,
	                                                  unique_ptr<const BaseSecret> secret, OnCreateConflict on_conflict,
	                                                  SecretPersistType persist_type, const string &storage = "");
	//! Create a secret from a CreateSecretInfo
	GOOSE_API unique_ptr<SecretEntry> CreateSecret(ClientContext &context, const CreateSecretInput &info);
	//! The Bind for create secret is done by the secret manager
	GOOSE_API BoundStatement BindCreateSecret(CatalogTransaction transaction, CreateSecretInput &info);
	//! Lookup the best matching secret by matching the secret scopes to the path
	GOOSE_API SecretMatch LookupSecret(CatalogTransaction transaction, const string &path, const string &type);
	//! Get a secret by name, optionally from a specific storage
	GOOSE_API unique_ptr<SecretEntry> GetSecretByName(CatalogTransaction transaction, const string &name,
	                                                   const string &storage = "");
	//! Delete a secret by name, optionally by providing the storage to drop from
	GOOSE_API void DropSecretByName(CatalogTransaction transaction, const string &name,
	                                 OnEntryNotFound on_entry_not_found,
	                                 SecretPersistType persist_type = SecretPersistType::DEFAULT,
	                                 const string &storage = "");
	//! List all secrets from all secret storages
	GOOSE_API vector<SecretEntry> AllSecrets(CatalogTransaction transaction);

	//! List all secret types
	GOOSE_API vector<SecretType> AllSecretTypes();

	//! Secret Manager settings
	GOOSE_API virtual void SetEnablePersistentSecrets(bool enabled);
	GOOSE_API virtual void ResetEnablePersistentSecrets();
	GOOSE_API virtual bool PersistentSecretsEnabled();

	GOOSE_API virtual void SetDefaultStorage(const string &storage);
	GOOSE_API virtual void ResetDefaultStorage();
	GOOSE_API virtual string DefaultStorage();

	GOOSE_API virtual void SetPersistentSecretPath(const string &path);
	GOOSE_API virtual void ResetPersistentSecretPath();
	GOOSE_API virtual string PersistentSecretPath();

	//! Utility functions
	GOOSE_API void DropSecretByName(ClientContext &context, const string &name, OnEntryNotFound on_entry_not_found,
	                                 SecretPersistType persist_type = SecretPersistType::DEFAULT,
	                                 const string &storage = "");

private:
	//! Register a secret type
	void RegisterSecretTypeInternal(SecretType &type);
	//! Lookup a SecretType, throws if not found
	SecretType LookupTypeInternal(const string &type);
	//! Try to lookup a SecretType
	bool TryLookupTypeInternal(const string &type, SecretType &type_out);
	//! Register a secret provider
	void RegisterSecretFunctionInternal(CreateSecretFunction function, OnCreateConflict on_conflict);
	//! Lookup a CreateSecretFunction
	optional_ptr<CreateSecretFunction> LookupFunctionInternal(const string &type, const string &provider);
	//! Register a new Secret
	unique_ptr<SecretEntry> RegisterSecretInternal(CatalogTransaction transaction, unique_ptr<const BaseSecret> secret,
	                                               OnCreateConflict on_conflict, SecretPersistType persist_type,
	                                               const string &storage = "");
	//! Initialize the secret catalog_set and persistent secrets (lazily)
	void InitializeSecrets(CatalogTransaction transaction);
	//! Load a secret storage
	void LoadSecretStorageInternal(unique_ptr<SecretStorage> storage);

	//! Autoload extension for specific secret type
	void AutoloadExtensionForType(const string &type);
	//! Autoload extension for specific secret function
	void AutoloadExtensionForFunction(const string &type, const string &provider);

	//! Will throw appropriate error message when type not found
	[[noreturn]] void ThrowTypeNotFoundError(const string &type, const string &secret_path = "");
	[[noreturn]] void ThrowProviderNotFoundError(const string &type, const string &provider, bool was_default = false);

	//! Thread-safe accessors for secret_storages
	vector<reference<SecretStorage>> GetSecretStorages();
	optional_ptr<SecretStorage> GetSecretStorage(const string &name);

	//! Throw an exception if the secret manager is initialized
	void ThrowOnSettingChangeIfInitialized();

	//! Lock for types, functions, settings and storages
	mutex manager_lock;
	//! Secret functions;
	case_insensitive_map_t<CreateSecretFunctionSet> secret_functions;
	//! Secret types;
	case_insensitive_map_t<SecretType> secret_types;
	//! Map of all registered SecretStorages
	case_insensitive_map_t<unique_ptr<SecretStorage>> secret_storages;
	//! While false, secret manager settings can still be changed
	atomic<bool> initialized {false};
	//! Configuration for secret manager
	SecretManagerConfig config;
	//! Pointer to current db instance
	optional_ptr<DatabaseInstance> db;
};

//! The DefaultGenerator for persistent secrets. This is used to store lazy loaded secrets in the catalog
class DefaultSecretGenerator : public DefaultGenerator {
public:
	DefaultSecretGenerator(Catalog &catalog, SecretManager &secret_manager, case_insensitive_set_t &persistent_secrets);

public:
	unique_ptr<CatalogEntry> CreateDefaultEntry(CatalogTransaction transaction, const string &entry_name) override;
	unique_ptr<CatalogEntry> CreateDefaultEntry(ClientContext &context, const string &entry_name) override;
	vector<string> GetDefaultEntries() override;

protected:
	unique_ptr<CatalogEntry> CreateDefaultEntryInternal(const string &entry_name);

	SecretManager &secret_manager;
	mutex lock;
	case_insensitive_set_t persistent_secrets;
};

} // namespace goose
