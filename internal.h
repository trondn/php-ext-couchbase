/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright 2012 Couchbase, Inc.                                       |
  +----------------------------------------------------------------------+
  | Licensed under the Apache License, Version 2.0 (the "License");      |
  | you may not use this file except in compliance with the License.     |
  | You may obtain a copy of the License at                              |
  |     http://www.apache.org/licenses/LICENSE-2.0                       |
  | Unless required by applicable law or agreed to in writing, software  |
  | distributed under the License is distributed on an "AS IS" BASIS,    |
  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      |
  | implied. See the License for the specific language governing         |
  | permissions and limitations under the License.                       |
  +----------------------------------------------------------------------+
*/
#ifndef COUCHBASE_INTERNAL_H
#define COUCHBASE_INTERNAL_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef PHP_WIN32
#	 define PHP_COUCHBASE_API __declspec(dllexport)
#    define PHP_COUCHBASE_LOCAL
#	 define strtoull _strtoui64
#    define PHP_COUCHBASE_UNUSED
#elif defined(__GNUC__) && __GNUC__ >= 4
#	 define PHP_COUCHBASE_API __attribute__ ((visibility("default")))
#    define PHP_COUCHBASE_LOCAL __attribute__ ((visibility("hidden")))
#    define PHP_COUCHBASE_UNUSED __attribute__((unused))
#else
#	 define PHP_COUCHBASE_API
#    define PHP_COUCHBASE_LOCAL
#    define PHP_COUCHBASE_UNUSED
#endif

#ifdef PHP_WIN32
# include "win32/php_stdint.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/url.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_var.h"
#include "ext/json/php_json.h"
#include "ext/standard/php_var.h"
#include <libcouchbase/couchbase.h>
#include "php_couchbase.h"
#include "fastlz/fastlz.h"

#ifdef HAVE_COMPRESSION_ZLIB
# include <zlib.h>
#endif

#ifdef HAVE_EXT_IGBINARY_IGBINARY_H
#include <ext/igbinary/igbinary.h>
#define HAVE_IGBINARY 1
#endif


#include "Zend/zend_API.h"
#include <zend_exceptions.h>
#include <assert.h>
#include <stdarg.h>

#ifdef ZTS
#include "TSRM.h"
#endif

#include <libcouchbase/couchbase.h>

#ifdef ZTS
#define COUCHBASE_G(v) TSRMG(couchbase_globals_id, zend_couchbase_globals *, v)
#else
#define COUCHBASE_G(v) (couchbase_globals.v)
#endif

#define PHP_COUCHBASE_RESOURCE	  "Couchbase"
#define PHP_COUCHBASE_PERSISTENT_RESOURCE "Persistent Couchbase"
#define COUCHBASE_PROPERTY_HANDLE "_handle"

#define COUCHBASE_OPT_SERIALIZER			1
#define COUCHBASE_OPT_COMPRESSION			 2
#define COUCHBASE_OPT_PREFIX_KEY			3
#define COUCHBASE_OPT_IGNOREFLAGS			4
#define COUCHBASE_OPT_VOPTS_PASSTHROUGH		5

#define COUCHBASE_SERIALIZER_PHP			0
#define COUCHBASE_SERIALIZER_DEFAULT		COUCHBASE_SERIALIZER_PHP
#define COUCHBASE_SERIALIZER_DEFAULT_NAME	"php"
#define COUCHBASE_SERIALIZER_JSON			1
#define COUCHBASE_SERIALIZER_JSON_ARRAY		2
#define COUCHBASE_SERIALIZER_IGBINARY		3

#define COUCHBASE_VAL_TYPE_MASK				0xf
#define COUCHBASE_VAL_GET_TYPE(f)			((f) & COUCHBASE_VAL_TYPE_MASK)
#define COUCHBASE_VAL_SET_TYPE(flags, type) ((flags) |= ((type) & COUCHBASE_VAL_TYPE_MASK))
#define COUCHBASE_VAL_IS_STRING				0
#define COUCHBASE_VAL_IS_LONG				1
#define COUCHBASE_VAL_IS_DOUBLE				2
#define COUCHBASE_VAL_IS_BOOL				3
#define COUCHBASE_VAL_IS_SERIALIZED			4
#define COUCHBASE_VAL_IS_IGBINARY			5
#define COUCHBASE_VAL_IS_JSON				6

#define COUCHBASE_COMPRESSION_MASK			 224 /* 7 << 5 */
#define COUCHBASE_COMPRESSION_NONE			 0
#define COUCHBASE_COMPRESSION_ZLIB			 1
#define COUCHBASE_COMPRESSION_FASTLZ		 2

/* pecl-memcached requires this bit be set as well */
#define COUCHBASE_COMPRESSION_MCISCOMPRESSED (1<<4)

#define COUCHBASE_GET_COMPRESSION(f)		 ((f) >> 5)
#define COUCHBASE_SET_COMPRESSION(f, c)		 ((f) = ((f) & ~COUCHBASE_COMPRESSION_MASK) | (c) << 5)
#define COUCHBASE_GET_PRESERVE_ORDER		(1<<0)

#define COUCHBASE_MIN_PERSIST 0
#define COUCHBASE_MAX_PERSIST 4
#define COUCHBASE_MIN_REPLICATE 0
#define COUCHBASE_MAX_REPLICATE 3

#ifndef Z_ADDREF_P
#define Z_ADDREF_P	ZVAL_ADDREF
#endif

struct _php_couchbase_ctx;

typedef struct _php_couchbase_res {
	lcb_t handle;
	char *bucket;
	long seqno;
	char async;
	char serializer;
	char compressor;
	char ignoreflags;
	char *prefix_key;
	int prefix_key_len;
	lcb_error_t rc; /* returned code */
	unsigned char is_connected;
	unsigned char viewopts_passthrough;

	char *errinfo;
	/* asynchronous context */
	struct _php_couchbase_ctx *async_ctx;
} php_couchbase_res;

typedef struct _php_couchbase_ctx {
	zval *rv;
	zval *cas;
	php_couchbase_res *res;
	unsigned char flags;
	void *extended_value;
} php_couchbase_ctx;

ZEND_BEGIN_MODULE_GLOBALS(couchbase)
zend_bool persistent;
zend_bool restflush;
char serializer_real;
char *serializer;
char compressor_real;
char *compressor;
long compression_threshold;
double compression_factor;
long durability_default_poll_interval;
long durability_default_timeout;
long view_timeout;
char *config_cache;
char *config_cache_error;
ZEND_END_MODULE_GLOBALS(couchbase)

PHP_GINIT_FUNCTION(couchbase);
PHP_MINIT_FUNCTION(couchbase);
PHP_MSHUTDOWN_FUNCTION(couchbase);
PHP_RINIT_FUNCTION(couchbase);
PHP_RSHUTDOWN_FUNCTION(couchbase);
PHP_MINFO_FUNCTION(couchbase);

PHP_METHOD(couchbase, __construct);
PHP_METHOD(couchbase, add);
PHP_METHOD(couchbase, set);
PHP_METHOD(couchbase, setMulti);
PHP_METHOD(couchbase, replace);
PHP_METHOD(couchbase, prepend);
PHP_METHOD(couchbase, append);
PHP_METHOD(couchbase, cas);
PHP_METHOD(couchbase, get);
PHP_METHOD(couchbase, getMulti);
PHP_METHOD(couchbase, getReplica);
PHP_METHOD(couchbase, getDelayed);
PHP_METHOD(couchbase, getAndLock);
PHP_METHOD(couchbase, getAndLockMulti);
PHP_METHOD(couchbase, getAndTouch);
PHP_METHOD(couchbase, getAndTouchMulti);
PHP_METHOD(couchbase, unlock);
PHP_METHOD(couchbase, touch);
PHP_METHOD(couchbase, touchMulti);
PHP_METHOD(couchbase, fetch);
PHP_METHOD(couchbase, fetchAll);
PHP_METHOD(couchbase, view);
PHP_METHOD(couchbase, viewGenQuery);
PHP_METHOD(couchbase, delete);
PHP_METHOD(couchbase, getStats);
PHP_METHOD(couchbase, flush);
PHP_METHOD(couchbase, increment);
PHP_METHOD(couchbase, decrement);
PHP_METHOD(couchbase, getResultCode);
PHP_METHOD(couchbase, getResultMessage);
PHP_METHOD(couchbase, setOption);
PHP_METHOD(couchbase, getOption);
PHP_METHOD(couchbase, getVersion);
PHP_METHOD(couchbase, getClientVersion);
PHP_METHOD(couchbase, getNumReplicas);
PHP_METHOD(couchbase, getServers);
PHP_METHOD(couchbase, observe);
PHP_METHOD(couchbase, observeMulti);
PHP_METHOD(couchbase, keyDurability);
PHP_METHOD(couchbase, keyDurabilityMulti);
PHP_METHOD(couchbase, getTimeout);
PHP_METHOD(couchbase, setTimeout);
PHP_METHOD(couchbase, getDesignDoc);
PHP_METHOD(couchbase, setDesignDoc);
PHP_METHOD(couchbase, deleteDesignDoc);
PHP_METHOD(couchbase, listDesignDocs);

PHP_FUNCTION(couchbase_connect);
PHP_FUNCTION(couchbase_add);
PHP_FUNCTION(couchbase_set);
PHP_FUNCTION(couchbase_set_multi);
PHP_FUNCTION(couchbase_replace);
PHP_FUNCTION(couchbase_prepend);
PHP_FUNCTION(couchbase_append);
PHP_FUNCTION(couchbase_cas);
PHP_FUNCTION(couchbase_get);
PHP_FUNCTION(couchbase_get_multi);
PHP_FUNCTION(couchbase_get_delayed);
PHP_FUNCTION(couchbase_get_and_lock);
PHP_FUNCTION(couchbase_get_and_lock_multi);
PHP_FUNCTION(couchbase_get_and_touch);
PHP_FUNCTION(couchbase_get_and_touch_multi);
PHP_FUNCTION(couchbase_unlock);
PHP_FUNCTION(couchbase_touch);
PHP_FUNCTION(couchbase_touch_multi);
PHP_FUNCTION(couchbase_fetch);
PHP_FUNCTION(couchbase_fetch_all);
PHP_FUNCTION(couchbase_view);
PHP_FUNCTION(couchbase_view_gen_query);
PHP_FUNCTION(couchbase_delete);
PHP_FUNCTION(couchbase_get_stats);
PHP_FUNCTION(couchbase_flush);
PHP_FUNCTION(couchbase_increment);
PHP_FUNCTION(couchbase_decrement);
PHP_FUNCTION(couchbase_get_result_code);
PHP_FUNCTION(couchbase_get_result_message);
PHP_FUNCTION(couchbase_set_option);
PHP_FUNCTION(couchbase_get_option);
PHP_FUNCTION(couchbase_get_version);
PHP_FUNCTION(couchbase_get_client_version);
PHP_FUNCTION(couchbase_get_num_replicas);
PHP_FUNCTION(couchbase_get_servers);
PHP_FUNCTION(couchbase_observe);
PHP_FUNCTION(couchbase_observe_multi);
PHP_FUNCTION(couchbase_key_durability);
PHP_FUNCTION(couchbase_key_durability_multi);
PHP_FUNCTION(couchbase_get_timeout);
PHP_FUNCTION(couchbase_set_timeout);
PHP_FUNCTION(couchbase_get_design_doc);
PHP_FUNCTION(couchbase_set_design_doc);
PHP_FUNCTION(couchbase_delete_design_doc);
PHP_FUNCTION(couchbase_list_design_docs);

/**
 * INI Entries. The naming scheme here should be self-evident
 */
#define PCBC_INIENT_OBS_INTERVAL "couchbase.durability_default_poll_interval"
#define PCBC_INIDEFL_OBS_INTERVAL "100000"

#define PCBC_INIENT_OBS_TIMEOUT "couchbase.durability_default_timeout"
#define PCBC_INIDEFL_OBS_TIMEOUT "40000000"

#define PCBC_INIENT_VIEW_TIMEOUT "couchbase.view_timeout"
#define PCBC_INIDEFL_VIEW_TIMEOUT "75"

#define PCBC_INIENT_SERIALIZER "couchbase.serializer"
#define PCBC_INIDEFL_SERIALIZER "php"

#define PCBC_INIENT_COMPALGO "couchbase.compressor"
#define PCBC_INIDEFL_COMPALGO "none"

#define PCBC_INIENT_COMPFACTOR "couchbase.compression_factor"
#define PCBC_INIDEFL_COMPFACTOR "1.3"

#define PCBC_INIENT_COMPTHRESH "couchbase.compression_threshold"
#define PCBC_INIDEFL_COMPTHRESH "2000"

#define PCBC_INIENT_INST_PERSIST "couchbase.instance.persistent"
#define PCBC_INIDEFL_INST_PERSIST "1"

#define PCBC_INIENT_INST_RESTFLUSH "couchbase.restflush"
#define PCBC_INIDEFL_INST_RESTFLUSH "On"

#define PCBC_INIENT_CONFIG_CACHE "couchbase.config_cache"
#define PCBC_INIDEFL_CONFIG_CACHE ""

/**
 * Hash table manipulation functions.
 */
#include "ht.h"
#include "resget.h"

PHP_COUCHBASE_LOCAL
extern void php_couchbase_setup_callbacks(lcb_t handle);

PHP_COUCHBASE_LOCAL
extern void observe_polling_internal(
	php_couchbase_ctx *ctx, zval *adurability, int modify_rv);

PHP_COUCHBASE_LOCAL
ZEND_EXTERN_MODULE_GLOBALS(couchbase);

PHP_COUCHBASE_LOCAL
extern int le_couchbase;

PHP_COUCHBASE_LOCAL
extern int le_pcouchbase;

/**
 * Prototype gettimeofday and usleep for win32
 * TODO: This should ideally be moved to its own file
 */
#ifdef WIN32
PHP_COUCHBASE_LOCAL
extern int gettimeofday(struct timeval *, struct timezone *);

PHP_COUCHBASE_LOCAL
extern void usleep(unsigned long);
#endif /* WIN32 */

/**
 * See https://github.com/php-memcached-dev/php-memcached/blob/1416a09d1d0e78cea5fec227744c1fe7b352017b/php_memcached.c#L2728
 * - pecl-memcached uses a raw unconverted (i.e. no htonl) for determining the
 * expanded payload size.
 */
typedef uint32_t pcbc_payload_len_t;

typedef struct {

	const char *orig;
	const char *compressed;

	char *expanded;
	size_t compressed_len;
	size_t expanded_len;
	size_t orig_len;

} php_couchbase_decomp;

typedef struct {
	char *_base;
	char *data;
	size_t compressed_len;
	size_t alloc;
} php_couchbase_comp;


PHP_COUCHBASE_LOCAL
void cbcomp_free(php_couchbase_comp *str);

PHP_COUCHBASE_LOCAL
int cbcomp_dcmp_init(const char *data, size_t len,
					 php_couchbase_decomp *info);

PHP_COUCHBASE_LOCAL
void cbcomp_dcmp_free(php_couchbase_decomp *info);

PHP_COUCHBASE_LOCAL
int php_couchbase_compress_zlib(const smart_str *input,
								php_couchbase_comp *output);

PHP_COUCHBASE_LOCAL
int php_couchbase_compress_fastlz(const smart_str *input,
								  php_couchbase_comp *output);

PHP_COUCHBASE_LOCAL
int php_couchbase_decompress_zlib(php_couchbase_decomp *info);

PHP_COUCHBASE_LOCAL
int php_couchbase_decompress_fastlz(php_couchbase_decomp *info);



PHP_COUCHBASE_LOCAL
void cbcomp_deploy(php_couchbase_comp *str);

/*
 * The following methods is an extra abstraction away from
 * libcouchbase in case we'd want to do extra stuff
 * around start / stopping of the io model.
 *
 * Ideally we'd like to use the synchronous mode in libcoucbase
 * instead of these methods.
 */
PHP_COUCHBASE_LOCAL
void pcbc_start_loop(struct _php_couchbase_res *res);

PHP_COUCHBASE_LOCAL
void pcbc_stop_loop(struct _php_couchbase_res *res);

PHP_COUCHBASE_LOCAL
int pcbc_check_expiry(INTERNAL_FUNCTION_PARAMETERS, int oo, long expiry, long *out);

PHP_COUCHBASE_LOCAL
char *php_couchbase_zval_to_payload(zval *value,
									size_t *payload_len,
									unsigned int *flags, int serializer, int compressor TSRMLS_DC);

PHP_COUCHBASE_LOCAL
int php_couchbase_zval_from_payload(zval *value,
									char *payload, size_t payload_len,
									unsigned int flags, int serializer, int ignoreflags TSRMLS_DC);

/**
 * Callback initialization
 */

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_arithmetic_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_get_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_store_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_remove_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_view_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_observe_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_flush_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_stat_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_version_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void php_couchbase_callbacks_unlock_init(lcb_t handle);

PHP_COUCHBASE_LOCAL
void pcbc_json_encode(smart_str *buf, zval *value TSRMLS_DC);

PHP_COUCHBASE_LOCAL
void pcbc_json_decode(zval *out, char *data, int ndata, zend_bool assoc
					  TSRMLS_DC);

#include "management/cluster.h"
#include "exceptions.h"
#include "apidecl.h"
#include "simple_observe.h"

PHP_COUCHBASE_LOCAL
void couchbase_report_error(INTERNAL_FUNCTION_PARAMETERS, int oo,
							zend_class_entry *exception, const char *fmt, ...);

PHP_COUCHBASE_LOCAL
int try_setup_cache_dir(const char *path, char **emsg);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet expandtab sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
