# Install script for directory: /Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Users/roy/.espressif/tools/xtensa-esp32-elf/esp-2022r1-11.2.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/build_info.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/compat-2.x.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/legacy_or_psa.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/lms.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs7.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/private_access.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "/Users/roy/src/git/espressif/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

