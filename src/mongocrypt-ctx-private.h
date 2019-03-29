/*
 * Copyright 2019-present MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MONGOCRYPT_CTX_PRIVATE_H
#define MONGOCRYPT_CTX_PRIVATE_H

#include "mongocrypt.h"
#include "mongocrypt-buffer-private.h"
#include "mongocrypt-key-broker-private.h"

typedef enum {
   _MONGOCRYPT_TYPE_NONE,
   _MONGOCRYPT_TYPE_ENCRYPT,
   _MONGOCRYPT_TYPE_DECRYPT,
} _mongocrypt_ctx_type_t;


typedef bool (*_mongocrypt_ctx_mongo_op_fn) (mongocrypt_ctx_t *ctx,
                                             mongocrypt_binary_t *out);


typedef bool (*_mongocrypt_ctx_mongo_feed_fn) (mongocrypt_ctx_t *ctx,
                                               mongocrypt_binary_t *in);


typedef bool (*_mongocrypt_ctx_mongo_done_fn) (mongocrypt_ctx_t *ctx);


typedef bool (*_mongocrypt_ctx_finalize_fn) (mongocrypt_ctx_t *ctx,
                                             mongocrypt_binary_t *out);

typedef void (*_mongocrypt_ctx_cleanup_fn) (mongocrypt_ctx_t *ctx);


typedef struct {
   _mongocrypt_ctx_mongo_op_fn mongo_op_collinfo;
   _mongocrypt_ctx_mongo_feed_fn mongo_feed_collinfo;
   _mongocrypt_ctx_mongo_done_fn mongo_done_collinfo;

   _mongocrypt_ctx_mongo_op_fn mongo_op_markings;
   _mongocrypt_ctx_mongo_feed_fn mongo_feed_markings;
   _mongocrypt_ctx_mongo_done_fn mongo_done_markings;

   _mongocrypt_ctx_finalize_fn finalize;

   _mongocrypt_ctx_cleanup_fn cleanup;
} _mongocrypt_vtable_t;


struct _mongocrypt_ctx_t {
   mongocrypt_t *crypt;
   mongocrypt_ctx_state_t state;
   _mongocrypt_ctx_type_t type;
   mongocrypt_status_t *status;
   _mongocrypt_key_broker_t kb;
   _mongocrypt_vtable_t vtable;
};


/* Transition to the error state. An error status must have been set. */
bool
_mongocrypt_ctx_fail (mongocrypt_ctx_t *ctx);


/* Set an error status and transition to the error state. */
bool
_mongocrypt_ctx_fail_w_msg (mongocrypt_ctx_t *ctx, const char *msg);


typedef struct {
   mongocrypt_ctx_t parent;
   char *ns;
   const char *coll_name; /* points inside ns */
   _mongocrypt_buffer_t list_collections_filter;
   _mongocrypt_buffer_t schema;
   _mongocrypt_buffer_t original_cmd;
   _mongocrypt_buffer_t marking_cmd;
   _mongocrypt_buffer_t marked_cmd;
   _mongocrypt_buffer_t encrypted_cmd;
} _mongocrypt_ctx_encrypt_t;


typedef struct {
   mongocrypt_ctx_t parent;
   _mongocrypt_buffer_t original_doc;
   _mongocrypt_buffer_t decrypted_doc;
} _mongocrypt_ctx_decrypt_t;

bool
mongocrypt_ctx_encrypt_init (mongocrypt_ctx_t *ctx,
                             const char *ns,
                             uint32_t ns_len);

bool
mongocrypt_ctx_decrypt_init (mongocrypt_ctx_t *ctx, mongocrypt_binary_t *doc);

#endif /* MONGOCRYPT_CTX_PRIVATE_H */