#   Copyright 2017 KeycapEmu
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
cmake_minimum_required(VERSION 3.26)

find_package(Botan 3.1.1 REQUIRED)
# adler32
# aead
# aes
# aes_armv8
# aes_ni
# aes_power8
# aes_vperm
# argon2
# argon2_avx2
# argon2_ssse3
# argon2fmt
# aria
# asio
# asn1
# auto_rng
# base
# base32
# base58
# base64
# bcrypt
# bcrypt_pbkdf
# bigint
# blake2
# blake2mac
# block
# blowfish
# boost
# bzip2
# camellia
# cascade
# cast128
# cbc
# ccm
# certstor_flatfile
# certstor_sql
# certstor_sqlite3
# certstor_system
# certstor_system_macos
# certstor_system_windows
# cfb
# chacha
# chacha20poly1305
# chacha_avx2
# chacha_avx512
# chacha_rng
# chacha_simd32
# checksum
# cmac
# comb4p
# commoncrypto
# compression
# cpuid
# crc24
# crc32
# cryptobox
# ctr
# curve25519
# des
# dh
# dilithium
# dilithium_aes
# dl_algo
# dl_group
# dlies
# dsa
# dyn_load
# eax
# ec_group
# ec_h2c
# ecc_key
# ecdh
# ecdsa
# ecgdsa
# ecies
# eckcdsa
# ed25519
# elgamal
# eme_oaep
# eme_pkcs1
# eme_raw
# emsa_pkcs1
# emsa_pssr
# emsa_raw
# emsa_x931
# entropy
# fd_unix
# ffi
# filters
# fpe_fe1
# gcm
# getentropy
# ghash
# ghash_cpu
# ghash_vperm
# gmac
# gost_28147
# gost_3410
# gost_3411
# hash
# hash_id
# hex
# hkdf
# hmac
# hmac_drbg
# hotp
# http_util
# idea
# idea_sse2
# iso9796
# kdf
# kdf1
# kdf1_iso18033
# kdf2
# keccak
# keypair
# kyber
# kyber_90s
# lion
# locking_allocator
# lzma
# mac
# mce
# md4
# md5
# mdx_hash
# mem_pool
# mgf1
# mode_pad
# modes
# mp
# nist_keywrap
# noekeon
# noekeon_simd
# numbertheory
# ocb
# ofb
# par_hash
# passhash9
# pbes2
# pbkdf
# pbkdf2
# pem
# pgp_s2k
# pk_pad
# pkcs11
# poly1305
# poly_dbl
# prf_tls
# prf_x942
# processor_rng
# psk_db
# pubkey
# raw_hash
# rc4
# rdseed
# rfc3394
# rfc6979
# rmd160
# rng
# roughtime
# rsa
# salsa20
# scrypt
# seed
# serpent
# serpent_avx2
# serpent_avx512
# serpent_simd
# sessions_sql
# sessions_sqlite3
# sha1
# sha1_armv8
# sha1_sse2
# sha1_x86
# sha2_32
# sha2_32_armv8
# sha2_32_bmi2
# sha2_32_x86
# sha2_64
# sha2_64_bmi2
# sha3
# sha3_bmi2
# shacal2
# shacal2_armv8
# shacal2_avx2
# shacal2_simd
# shacal2_x86
# shake
# shake_cipher
# simd
# simd_avx2
# simd_avx512
# siphash
# siv
# skein
# sm2
# sm3
# sm4
# sm4_armv8
# socket
# sodium
# sp800_108
# sp800_56a
# sp800_56c
# sphincsplus_sha2
# sphincsplus_shake
# sqlite3
# srp6
# stateful_rng
# stream
# streebog
# system_rng
# thread_utils
# threefish_512
# tls12
# tls13
# tls_cbc
# tpm
# trunc_hash
# tss
# twofish
# utils
# uuid
# whirlpool
# win32_stats
# x509
# x919_mac
# xmss
# xts
# zfec
# zfec_sse2
# zfec_vperm
# zlib

botan_generate(botan_lib
    base32
    hex
    numbertheory
    md5
    sha1
    sha2_64
    hmac
    hmac_drbg
    auto_rng
    bigint
    system_rng 
)

add_library(keycaproot
    ${version_file}
    utility/crc32.cpp
    utility/md5.cpp
    utility/random.cpp
    utility/utility.cpp
    "utility/string.cpp"
    "configuration/config_file.cpp"
    compression/zip.cpp
    cryptography/ARC4.cpp
    cryptography/OTP.cpp
    network/connection.cpp
    network/data_router.cpp
    network/memory_stream.cpp
    network/message_handler.cpp
    network/service_base.cpp
    network/service_locator.cpp
    network/srp6/client.cpp
    network/srp6/server.cpp
    network/srp6/group_parameters.cpp
    network/srp6/utility.cpp
)
add_library(keycap::root ALIAS keycaproot)

target_include_directories(keycaproot
    PRIVATE
        ${PROJECT_SOURCE_DIR}/contrib/gsl/include
    PUBLIC
        ${PROJECT_SOURCE_DIR}/contrib/json/include
        ${PROJECT_SOURCE_DIR}/contrib/spdlog/include
        ${PROJECT_SOURCE_DIR}/src/include
)

target_link_libraries(keycaproot
    PRIVATE
        keycap::root::project_options
        keycap::root::project_warnings

        botan_lib
        Boost::system
        Boost::lexical_cast
        zlibstatic

    PUBLIC
        Boost::asio
        Boost::crc
        Boost::uuid
)