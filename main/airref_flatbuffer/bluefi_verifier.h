#ifndef BLUEFI_VERIFIER_H
#define BLUEFI_VERIFIER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#ifndef BLUEFI_READER_H
#include "bluefi_reader.h"
#endif
#include "flatcc/flatcc_verifier.h"
#include "flatcc/flatcc_prologue.h"

static int WifiCredentials_verify_table(flatcc_table_verifier_descriptor_t *td);
static int BluefiCommand_verify_table(flatcc_table_verifier_descriptor_t *td);
static int BluefiReply_verify_table(flatcc_table_verifier_descriptor_t *td);

static int WifiCredentials_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_string_field(td, 0, 0) /* ssid */)) return ret;
    if ((ret = flatcc_verify_string_field(td, 1, 0) /* password */)) return ret;
    return flatcc_verify_ok;
}

static inline int WifiCredentials_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, WifiCredentials_identifier, &WifiCredentials_verify_table);
}

static inline int WifiCredentials_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, WifiCredentials_type_identifier, &WifiCredentials_verify_table);
}

static inline int WifiCredentials_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &WifiCredentials_verify_table);
}

static inline int WifiCredentials_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &WifiCredentials_verify_table);
}

static int BluefiCommand_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_table_field(td, 0, 0, &WifiCredentials_verify_table) /* wifiCredential */)) return ret;
    if ((ret = flatcc_verify_field(td, 1, 4, 4) /* doScan */)) return ret;
    return flatcc_verify_ok;
}

static inline int BluefiCommand_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, BluefiCommand_identifier, &BluefiCommand_verify_table);
}

static inline int BluefiCommand_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, BluefiCommand_type_identifier, &BluefiCommand_verify_table);
}

static inline int BluefiCommand_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &BluefiCommand_verify_table);
}

static inline int BluefiCommand_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &BluefiCommand_verify_table);
}

static int BluefiReply_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_table_vector_field(td, 0, 0, &WifiCredentials_verify_table) /* wifiCredentials */)) return ret;
    return flatcc_verify_ok;
}

static inline int BluefiReply_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, BluefiReply_identifier, &BluefiReply_verify_table);
}

static inline int BluefiReply_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, BluefiReply_type_identifier, &BluefiReply_verify_table);
}

static inline int BluefiReply_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &BluefiReply_verify_table);
}

static inline int BluefiReply_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &BluefiReply_verify_table);
}

#include "flatcc/flatcc_epilogue.h"
#endif /* BLUEFI_VERIFIER_H */
