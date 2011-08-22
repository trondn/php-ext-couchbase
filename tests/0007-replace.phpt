--TEST--
Couchbase Replace test
--SKIPIF--
<?php if (!extension_loaded("couchbase")) echo 'skip'; ?>
--FILE--
<?php

$host = "localhost:9000";
$cb = couchbase_create($host, "Administrator", "asdasd", "default");
var_dump($cb);

couchbase_remove($cb, "k", function($error, $key) {
    var_dump($key);
});

couchbase_execute($cb);

couchbase_replace($cb, "k", "x", function($error, $key) {
    var_dump($error === COUCHBASE_KEY_ENOENT);
    var_dump($key);
});

couchbase_execute($cb);

couchbase_set($cb, "k", "x", function($error, $key) {
    var_dump($error);
    var_dump($key);
});

couchbase_execute($cb);

couchbase_replace($cb, "k", "y", function($error, $key) {
    var_dump("> replace");
    var_dump($error);
    var_dump($key);
    var_dump("< replace");
});

couchbase_execute($cb);
var_dump("cuted");

$cb = couchbase_create($host, "Administrator", "asdasd", "default");

couchbase_mget($cb, "k", function($error, $key, $value) {
    var_dump("> get");
    var_dump($error);
    var_dump($key);
    var_dump($value);
    var_dump("< get");
});

var_dump("precute");
couchbase_execute($cb);

var_dump("end");
?>
--EXPECTF--
resource(%d) of type (Couchbase Instance)
string(1) "k"
bool(true)
string(1) "k"
NULL
string(1) "k"
NULL
string(1) "k"
string(1) "y"
string(3) "end"
