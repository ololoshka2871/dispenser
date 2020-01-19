/*!
 * \file
 * \brief Contains Special trap for HardFault_Handler interrupt of cpu
 */

/*! \ingroup Utils
 *  @{
 */

extern "C" {

#include <stdint.h>

/// The prototype shows it is a naked function - in effect this is just an
/// assembly function.
void HardFault_Handler(void) __attribute__((naked));

/// The fault handler implementation calls a function called
/// prvGetRegistersFromStack().
void HardFault_Handler(void) {
  __asm volatile(
      "   .syntax unified                        \n"
      "   ldr   r0, =0xFFFFFFFD                  \n"
      "   cmp   r0, lr                           \n"
      "   bne   HardFault_Handler_ChooseMSP      \n"
      /* Reading PSP into R0 */
      "   mrs   r0, PSP                          \n"
      "   b     HardFault_Handler_Continue       \n"
      "HardFault_Handler_ChooseMSP:              \n"
      /* Reading MSP into R0 */
      "   mrs   r0, MSP                          \n"
      /* -----------------------------------------------------------------
       * If we have selected MSP check if we may use stack safetly.
       * If not - reset the stack to the initial value. */
      "   ldr   r1, =_estack                     \n"
      "   ldr   r2, =_estack-0x200               \n"
      /* MSP is in the range of <__StackTop, __StackLimit) */
      "   cmp   r0, r1                           \n"
      "   bhi   HardFault_MoveSP                 \n"
      "   cmp   r0, r2                           \n"
      "   bhi   HardFault_Handler_Continue       \n"
      /* ----------------------------------------------------------------- */
      "HardFault_MoveSP:                         \n"
      "   mov   SP, r1                           \n"
      "   movs  r0, #0                           \n"
      "HardFault_Handler_Continue:               \n"
      "   ldr r3, =prvGetRegistersFromStack      \n"
      "   bx r3                                  \n"
      "   .align                                 \n");
}

/*!
 * \brief Main HardFault trap
 *
 * Then application crashes it calls to debugger an saves fault context to
 * analyze
 * \param pulFaultStackAddress Trapped context provided by cpu
 */
void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress) {
  // Call to debuger
  for (;;)
    __asm__("BKPT");
}
}

/*! @} */
