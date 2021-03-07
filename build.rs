extern crate cc;
extern crate bindgen;

use std::fs;
use std::env;
use std::path::PathBuf;

fn main() {
    let mut files = vec![
        "quickjs/quickjs.c",
        "quickjs/libregexp.c",
        "quickjs/libunicode.c",
        "quickjs/cutils.c",
        "quickjs/quickjs-libc.c"
    ];

    if env::var("TARGET").unwrap() == "x86_64-fortanix-unknown-sgx" {
        files.push("sgx_libc.c")
    }

    let version = fs::read_to_string("quickjs/VERSION")
        .expect("VERSION file missing");

    cc::Build::new()
        .flag("-w")
        .flag(&format!("-DCONFIG_VERSION=\"{}\"", version.trim()))
        .flag("-Wno-array-bounds")
        .flag("-Wno-format-truncation")
        .flag("-D_GNU_SOURCE")
        .flag("-DEMSCRIPTEN")
        .flag("-fno-stack-protector")
        .files(files)
        .compile("libquickjs.a");

    let bindings = bindgen::Builder::default()
        .header("quickjs/quickjs.h")
        .header("quickjs/quickjs-libc.h")
        .whitelist_function("JS_NewRuntime")
        .whitelist_function("JS_NewContext")
        .whitelist_function("JS_Eval")
        .whitelist_function("JS_FreeContext")
        .whitelist_function("JS_FreeRuntime")
        .whitelist_function("JS_ToCStringLen2")
        .whitelist_function("JS_FreeCString")
        .whitelist_function("js_std_free_handlers")
        .whitelist_function("js_std_loop")
        .rustfmt_bindings(true)
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
