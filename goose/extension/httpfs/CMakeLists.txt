# Copyright (C) Kumo inc. and its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(HTTPFS_SOURCES
        hffs.cc
        s3fs.cc
        httpfs.cc
        http_state.cc
        crypto.cc
        hash_functions.cc
        create_secret_functions.cc
        httpfs_extension.cc)
if (NOT EMSCRIPTEN)
    set(HTTPFS_SOURCES ${HTTPFS_SOURCES} crypto.cc httpfs_httplib_client.cc
            httpfs_curl_client.cc)
else ()
    set(HTTPFS_SOURCES ${HTTPFS_SOURCES} httpfs_client_wasm.cc)
endif ()

goose_cc_extension(
        NAMESPACE ${PROJECT_NAME}
        NAME httpfs
        SOURCES
        ${HTTPFS_SOURCES}
        ABI_TYPE
        "CPP"
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${HTTPFS_DEPS_LINK}
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)
