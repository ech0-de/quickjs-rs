#[cfg(target_env = "sgx")]
use std::ffi::CStr;

#[cfg(target_env = "sgx")]
use std::convert::TryInto;

#[no_mangle]
#[cfg(target_env = "sgx")]
// int puts(const char *s);
pub unsafe extern fn puts(s: *const libc::c_char) -> libc::c_int {
    // println!("puts({:?})", s);
    let c_str = CStr::from_ptr(s);
    if let Ok(str_slice) = c_str.to_str() {
	print!("{}", &str_slice);
        return str_slice.len().try_into().unwrap()
    } else {
        return 0
    }
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// long int lrint(double x);
pub unsafe extern fn lrint(x: libc::c_double) -> libc::c_long {
    x as libc::c_long
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// int fesetround(int rounding_mode);
pub unsafe extern fn fesetround(rounding_mode: libc::c_int) -> libc::c_int {
    rounding_mode
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// double acosh(double x);
pub unsafe extern fn acosh(x: libc::c_double) -> libc::c_double {
    x.acosh()
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// double asinh(double x);
pub unsafe extern fn asinh(x: libc::c_double) -> libc::c_double {
    x.sinh()
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// double atanh(double x);
pub unsafe extern fn atanh(x: libc::c_double) -> libc::c_double {
    x.atanh()
}

#[no_mangle]
#[cfg(target_env = "sgx")]
pub unsafe extern fn current_time() -> u64 {
    let sys_time = std::time::SystemTime::now();
    let epoch = sys_time.duration_since(std::time::UNIX_EPOCH).unwrap();
    return epoch.as_secs();
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// int fputs(const char *s, FILE *stream);
pub unsafe extern fn fputs(_s: *const libc::c_char, _stream: *mut libc::c_void) -> libc::c_int {
    // not allowed
    0
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// int putchar(int c);
pub unsafe extern fn putchar(c: libc::c_int) -> libc::c_int {
    print!("{}", std::char::from_u32_unchecked(u32::from_be_bytes(c.to_be_bytes())));
    1
}

#[no_mangle]
#[cfg(target_env = "sgx")]
// void abort(void);
pub unsafe extern fn abort() {
    panic!("abort");
}
