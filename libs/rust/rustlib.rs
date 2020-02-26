#![no_std]

extern crate stm32f0;

use stm32f0::stm32f0x2;

use core::panic::PanicInfo;

#[no_mangle]
pub extern "C" fn rust_fun(a: i32) -> i32 {
    a + 1
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

