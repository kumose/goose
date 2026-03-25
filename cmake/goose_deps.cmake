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
############################################################
# system pthread and rt, dl
############################################################
set(KMCMAKE_SYSTEM_DYLINK)
if (APPLE)
    find_library(CoreFoundation CoreFoundation)
    list(APPEND KMCMAKE_SYSTEM_DYLINK ${CoreFoundation} pthread)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    list(APPEND KMCMAKE_SYSTEM_DYLINK rt dl pthread)
endif ()

if (KMCMAKE_BUILD_TEST)
    enable_testing()
    #include(require_gtest)
    #include(require_gmock)
    #include(require_doctest)
endif (KMCMAKE_BUILD_TEST)

if (KMCMAKE_BUILD_BENCHMARK)
    #include(require_benchmark)
endif ()

find_package(Threads REQUIRED)
find_package(turbo REQUIRED)
find_package(xre2 REQUIRED)
find_package(xfsst REQUIRED)
find_package(lz4 REQUIRED)
find_package(Snappy CONFIG REQUIRED)
find_package(unofficial-brotli CONFIG REQUIRED)
find_package(MbedTLS CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(cantor CONFIG REQUIRED)
find_package(xicu CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
############################################################
#
# add you libs to the KMCMAKE_DEPS_LINK variable eg as turbo
# so you can and system pthread and rt, dl already add to
# KMCMAKE_SYSTEM_DYLINK, using it for fun.
##########################################################
set(KMCMAKE_DEPS_LINK
        #${TURBO_LIB}
        xre2::xre2_static
        turbo::turbo_static
        xfsst::xfsst_static
        xicu::xicu_static
        lz4::lz4
        Snappy::snappy
        zstd::libzstd
        MbedTLS::mbedtls
        unofficial::brotli::brotlidec
        unofficial::brotli::brotlienc
        cantor::cantor_static
        ${CURL_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        ${KMCMAKE_SYSTEM_DYLINK}
        )
set(HTTPFS_DEPS_LINK
        ${CURL_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
)
list(REMOVE_DUPLICATES KMCMAKE_DEPS_LINK)
kmcmake_print_list_label("Denpendcies:" KMCMAKE_DEPS_LINK)

############################################################
# for binary
############################################################
set(KMCMAKE_STATIC_BIN_OPTION -static-libgcc -static-libstdc++)



