#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cfg(target_env = "sgx")]
extern crate rs_libc;

extern crate libc;

pub mod sgx_c_bindings;

use std::io::{Error, ErrorKind};
use std::ffi::CString;
use std::ffi::CStr;

pub struct Context {
    rt: *mut JSRuntime,
    ctx: *mut JSContext
}

impl Context {
    pub fn new() -> Context {
        unsafe {
            let rt = JS_NewRuntime();
            let ctx = JS_NewContext(rt);

            Context { 
                rt: rt,
                ctx: ctx
            }
        }
    }

    pub fn eval(&self, expr: &str) -> Result<String, Error> {
        let expr_len = expr.len();
        let expr = CString::new(expr).unwrap();
        let filename = CString::new("<cmdline>").unwrap();

        unsafe {
            let val = JS_Eval(self.ctx, expr.as_ptr(), expr_len, filename.as_ptr(), 0);
            let res = get_value_as_string(self.ctx, val);

            js_std_loop(self.ctx);

            res
        }
    }
}

unsafe fn get_value_as_string(ctx: *mut JSContext, val: JSValue) -> Result<String, Error> {
    let res = JS_ToCStringLen2(ctx, std::ptr::null_mut(), val, 0);
    if res.is_null() {
        return Err(Error::new(ErrorKind::Other, "received null pointer!"));
    }

    let c_str = CStr::from_ptr(res);
    match c_str.to_str() {
        Err(_) => Err(Error::new(ErrorKind::Other, "could not parse string!")),
        Ok(str_slice) => {
            let str_buf = str_slice.to_owned();
            JS_FreeCString(ctx, res);
            Ok(str_buf)
        },
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        unsafe {
            js_std_free_handlers(self.rt);
            JS_FreeContext(self.ctx);
            JS_FreeRuntime(self.rt);
        }
    }
}
