#pragma once

#define AVR_UART_PUT_ASM_TMPL(delay,delay_rest)     \
  "  in  %[port_state], %[portx]        \n\t"        \
  "  com %[byte]                       \n\t"        \
  "  ldi %[bits],       10             \n\t"        \
  "1:cbr %[port_state], %[mask]        \n\t"        \
  "  brcs 2f                           \n\t"        \
  "  sbr %[port_state], %[mask]        \n\t"        \
  "2:out %[portx],      %[port_state]  \n\t"        \
  "  lsr %[byte]                       \n\t"        \
  delay                                             \
  delay_rest                                        \
  "  dec %[bits]                       \n\t"        \
  "  brne 1b                           \n\t" 

#define AVR_UART_PUT_OUT_OPS(delay)                               \
  : [byte] "+r" (byte),                                           \
    [port_state] "=d" (port_value),                               \
    [bits] "=d" (bits)                                            \
    delay
    
  
#define AVR_UART_PUT_IN_OPS(delay)                                \
  : [portx] "I" (TxPin::portx::io_addr()),                        \
    [mask] "i" (TxPin::bv())                                      \
    delay
    
#define AVR_UART_GET_ASM_TMPL(_1_5_delay_rest, delay, delay_rest) \
  "1:sbic %[pinx], %[rx_pin]                          \n\t"       \
  "  rjmp 1b                                          \n\t"       \
  "  ldi %[one_half_delay_cnt],  %[one_half_delay_b]  \n\t"       \
  "1:dec %[one_half_delay_cnt]                        \n\t"       \
  "  brne 1b                                          \n\t"       \
  _1_5_delay_rest                                                 \
  "  ldi  %[bits], 9                                  \n\t"       \
  "2:ror  %[byte]                                     \n\t"       \
  "  sbic %[pinx], %[rx_pin]                          \n\t"       \
  "  sec                                              \n\t"       \
  delay                                                           \
  delay_rest                                                      \
  "  dec %[bits]                                      \n\t"       \
  "  brne 2b                                          \n\t"

#define AVR_UART_GET_SEQ_ASM_TMPL(_1_5_delay_rest, delay, delay_rest, \
                                  delay_after_fst_bit, delay_after_fst_bit_rest) \
  "  ldi %[cnt], %[n_bytes] \n\t"                                       \
  "1:sbic %[pinx], %[rx_pin]                          \n\t"             \
  "  rjmp 1b                                          \n\t"             \
  "  ldi %[one_half_delay_cnt],  %[one_half_delay_b]  \n\t"             \
  "1:dec %[one_half_delay_cnt]                        \n\t"             \
  "  brne 1b                                          \n\t"             \
  _1_5_delay_rest                                                       \
  "  ldi  %[bits], 9                                  \n\t"             \
  "2:clc                                              \n\t"             \
  "  sbic %[pinx], %[rx_pin]                          \n\t"             \
  "  sec                                              \n\t"             \
  "  dec %[bits] \n\t"                                                  \
  "  breq 5f \n\t"                                                      \
  "  ror  %[byte]                                     \n\t"             \
  delay                                                                 \
  delay_rest                                                            \
  "  rjmp 2b                                          \n\t"             \
  "5:dec  %[cnt]                                      \n\t"             \
  "  breq 4f                                          \n\t"             \
  "1:sbic %[pinx], %[rx_pin]                          \n\t"             \
  "  rjmp 1b                                          \n\t"             \
  "  st   %a[values]+, %[byte]                        \n\t"             \
  "  clr  %[byte]                                     \n\t"             \
  "  ldi  %[bits], 9                                  \n\t"             \
  delay_after_fst_bit                                                   \
  delay_after_fst_bit_rest                                              \
  "  rjmp 2b                                          \n\t"             \
  "4:st   %a[values]+, %[byte]                        \n\t"
  
#define AVR_UART_GET_OUT_OPS(delay)                     \
  : [byte] "+r" (byte),                                 \
    [bits] "=d" (bits),                                 \
    [one_half_delay_cnt] "=d" (one_half_delay_cnt)      \
    delay
    
#define AVR_UART_GET_SEQ_OUT_OPS                        \
  , "=m" (pvalues)                                      \
  , [cnt] "=r" (cnt)                                    \
  , [values] "+e" (pvalues)                                  

#define AVR_UART_GET_IN_OPS(delay)                     \
  : [pinx] "I" (RxPin::pinx::io_addr()),               \
    [rx_pin] "I" (RxPin::value),                       \
    [one_half_delay_b] "M" (one_half_delay_b)          \
    delay

#define AVR_UART_GET_SEQ_IN_OPS                                         \
  , [n_bytes] "M" (N)                                                   \
  , [one_half_delay_after_fst_bit_b] "M" (one_half_delay_after_fst_bit_b)

#define AVR_UART_DELAY_1_CYCLE "nop    \n\t"

#define AVR_UART_DELAY_2_CYCLE "rjmp . \n\t"

#define AVR_UART_DELAY_3_CYCLE                  \
  "  ldi %[delay_cnt],  %[delay_b]     \n\t"    \
  "3:dec %[delay_cnt]                  \n\t"    \
  "  brne 3b                           \n\t"

#define AVR_UART_DELAY_3_CYCLE_GET_SEQ                                       \
  "  ldi  %[one_half_delay_cnt], %[one_half_delay_after_fst_bit_b]     \n\t" \
  "1:dec  %[one_half_delay_cnt]                                        \n\t" \
  "  brne 1b                                                           \n\t"

#define AVR_UART_OUT_OPS_DELAY                  \
    ,[delay_cnt] "=d" (delay_cnt)

#define AVR_UART_IN_OPS_DELAY                   \
    ,[delay_b] "M" (delay_b)
