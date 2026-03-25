#include <goose/main/capi/capi_internal.h>

namespace goose {
namespace {
struct CFileSystem {
	FileSystem &fs;
	ErrorData error_data;

	explicit CFileSystem(FileSystem &fs_p) : fs(fs_p) {
	}

	void SetError(const char *message) {
		error_data = ErrorData(ExceptionType::IO, message);
	}
	void SetError(const std::exception &ex) {
		error_data = ErrorData(ex);
	}
};

struct CFileOpenOptions {
	goose::FileOpenFlags flags;
};

struct CFileHandle {
	ErrorData error_data;
	unique_ptr<FileHandle> handle;

	void SetError(const char *message) {
		error_data = ErrorData(ExceptionType::IO, message);
	}
	void SetError(const std::exception &ex) {
		error_data = ErrorData(ex);
	}
};

} // namespace
} // namespace goose

goose_file_system goose_client_context_get_file_system(goose_client_context context) {
	if (!context) {
		return nullptr;
	}
	auto ctx = reinterpret_cast<goose::CClientContextWrapper *>(context);
	auto wrapper = new goose::CFileSystem(goose::FileSystem::GetFileSystem(ctx->context));
	return reinterpret_cast<goose_file_system>(wrapper);
}

void goose_destroy_file_system(goose_file_system *file_system) {
	if (!file_system || !*file_system) {
		return;
	}
	const auto fs = reinterpret_cast<goose::CFileSystem *>(*file_system);
	delete fs;
	*file_system = nullptr;
}

goose_file_open_options goose_create_file_open_options() {
	auto options = new goose::CFileOpenOptions();
	return reinterpret_cast<goose_file_open_options>(options);
}

goose_state goose_file_open_options_set_flag(goose_file_open_options options, goose_file_flag flag, bool value) {
	if (!options) {
		return GooseError;
	}
	auto coptions = reinterpret_cast<goose::CFileOpenOptions *>(options);

	switch (flag) {
	case GOOSE_FILE_FLAG_READ:
		coptions->flags |= goose::FileOpenFlags::FILE_FLAGS_READ;
		break;
	case GOOSE_FILE_FLAG_WRITE:
		coptions->flags |= goose::FileOpenFlags::FILE_FLAGS_WRITE;
		break;
	case GOOSE_FILE_FLAG_APPEND:
		coptions->flags |= goose::FileOpenFlags::FILE_FLAGS_APPEND;
		break;
	case GOOSE_FILE_FLAG_CREATE:
		coptions->flags |= goose::FileOpenFlags::FILE_FLAGS_FILE_CREATE;
		break;
	case GOOSE_FILE_FLAG_CREATE_NEW:
		coptions->flags |= goose::FileOpenFlags::FILE_FLAGS_EXCLUSIVE_CREATE;
		break;
	default:
		return GooseError;
	}
	return GooseSuccess;
}

void goose_destroy_file_open_options(goose_file_open_options *options) {
	if (!options || !*options) {
		return;
	}
	auto coptions = reinterpret_cast<goose::CFileOpenOptions *>(*options);
	delete coptions;
	*options = nullptr;
}

goose_state goose_file_system_open(goose_file_system fs, const char *path, goose_file_open_options options,
                                     goose_file_handle *out_file) {
	if (!fs) {
		if (out_file) {
			*out_file = nullptr;
		}
		return GooseError;
	}
	auto cfs = reinterpret_cast<goose::CFileSystem *>(fs);
	if (!out_file) {
		cfs->SetError("Invalid out file to goose_file_system_open");
		return GooseError;
	}
	if (!path || !options) {
		cfs->SetError("Invalid input to goose_file_system_open");
		*out_file = nullptr;
		return GooseError;
	}

	try {
		auto coptions = reinterpret_cast<goose::CFileOpenOptions *>(options);
		auto handle = cfs->fs.OpenFile(goose::string(path), coptions->flags);
		auto wrapper = new goose::CFileHandle();
		wrapper->handle = std::move(handle);
		*out_file = reinterpret_cast<goose_file_handle>(wrapper);
		return GooseSuccess;
	} catch (const std::exception &ex) {
		cfs->SetError(ex);
		*out_file = nullptr;
		return GooseError;
	} catch (...) {
		cfs->SetError("Unknown error occurred during file open");
		*out_file = nullptr;
		return GooseError;
	}
}

goose_error_data goose_file_system_error_data(goose_file_system fs) {
	auto wrapper = new goose::ErrorDataWrapper();
	if (!fs) {
		return reinterpret_cast<goose_error_data>(wrapper);
	}
	auto cfs = reinterpret_cast<goose::CFileSystem *>(fs);
	wrapper->error_data = cfs->error_data;
	return reinterpret_cast<goose_error_data>(wrapper);
}

void goose_destroy_file_handle(goose_file_handle *file) {
	if (!file || !*file) {
		return;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(*file);
	cfile->handle->Close(); // Ensure the file is closed before destroying
	delete cfile;
	*file = nullptr;
}

goose_error_data goose_file_handle_error_data(goose_file_handle file) {
	auto wrapper = new goose::ErrorDataWrapper();
	if (!file) {
		return reinterpret_cast<goose_error_data>(wrapper);
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	wrapper->error_data = cfile->error_data;
	return reinterpret_cast<goose_error_data>(wrapper);
}

int64_t goose_file_handle_read(goose_file_handle file, void *buffer, int64_t size) {
	if (!file || !buffer || size < 0) {
		return -1;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	try {
		return cfile->handle->Read(buffer, static_cast<idx_t>(size));
	} catch (std::exception &ex) {
		cfile->SetError(ex);
		return -1;
	} catch (...) {
		cfile->SetError("Unknown error occurred during file read");
		return -1;
	}
}

int64_t goose_file_handle_write(goose_file_handle file, const void *buffer, int64_t size) {
	if (!file || !buffer || size < 0) {
		return -1;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	try {
		return cfile->handle->Write(const_cast<void *>(buffer), static_cast<idx_t>(size));
	} catch (std::exception &ex) {
		cfile->SetError(ex);
		return -1;
	} catch (...) {
		cfile->SetError("Unknown error occurred during file write");
		return -1;
	}
}

int64_t goose_file_handle_tell(goose_file_handle file) {
	if (!file) {
		return -1;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	try {
		return static_cast<int64_t>(cfile->handle->SeekPosition());
	} catch (std::exception &ex) {
		cfile->SetError(ex);
		return -1;
	} catch (...) {
		cfile->SetError("Unknown error occurred when getting file position");
		return -1;
	}
}

int64_t goose_file_handle_size(goose_file_handle file) {
	if (!file) {
		return -1;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	try {
		return static_cast<int64_t>(cfile->handle->GetFileSize());
	} catch (std::exception &ex) {
		cfile->SetError(ex);
		return -1;
	} catch (...) {
		cfile->SetError("Unknown error occurred when getting file size");
		return -1;
	}
}

goose_state goose_file_handle_seek(goose_file_handle file, int64_t position) {
	if (!file || position < 0) {
		return GooseError;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	try {
		cfile->handle->Seek(static_cast<idx_t>(position));
		return GooseSuccess;
	} catch (std::exception &ex) {
		cfile->SetError(ex);
		return GooseError;
	} catch (...) {
		cfile->SetError("Unknown error occurred when seeking in file");
		return GooseError;
	}
}

goose_state goose_file_handle_sync(goose_file_handle file) {
	if (!file) {
		return GooseError;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	try {
		cfile->handle->Sync();
		return GooseSuccess;
	} catch (std::exception &ex) {
		cfile->SetError(ex);
		return GooseError;
	} catch (...) {
		cfile->SetError("Unknown error occurred when syncing file");
		return GooseError;
	}
}

goose_state goose_file_handle_close(goose_file_handle file) {
	if (!file) {
		return GooseError;
	}
	auto cfile = reinterpret_cast<goose::CFileHandle *>(file);
	try {
		cfile->handle->Close();
		return GooseSuccess;
	} catch (std::exception &ex) {
		cfile->SetError(ex);
		return GooseError;
	} catch (...) {
		cfile->SetError("Unknown error occurred when closing file");
		return GooseError;
	}
}
