# 1 "Middlewares/Third_Party/RealThread_RTOS_RT-Thread/libcpu/arm/cortex-m3/context_gcc.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "Middlewares/Third_Party/RealThread_RTOS_RT-Thread/libcpu/arm/cortex-m3/context_gcc.S"
# 16 "Middlewares/Third_Party/RealThread_RTOS_RT-Thread/libcpu/arm/cortex-m3/context_gcc.S"
    .cpu cortex-m3
    .fpu softvfp
    .syntax unified
    .thumb
    .text

    .equ SCB_VTOR, 0xE000ED08
    .equ ICSR, 0xE000ED04
    .equ PENDSVSET_BIT, 0x10000000

    .equ SHPR3, 0xE000ED20
    .equ PENDSV_PRI_LOWEST, 0x00FF0000




    .global rt_hw_interrupt_disable
    .type rt_hw_interrupt_disable, %function
rt_hw_interrupt_disable:
    MRS R0, PRIMASK
    CPSID I
    BX LR




    .global rt_hw_interrupt_enable
    .type rt_hw_interrupt_enable, %function
rt_hw_interrupt_enable:
    MSR PRIMASK, R0
    BX LR






    .global rt_hw_context_switch_interrupt
    .type rt_hw_context_switch_interrupt, %function
    .global rt_hw_context_switch
    .type rt_hw_context_switch, %function
rt_hw_context_switch_interrupt:
rt_hw_context_switch:

    LDR R2, =rt_thread_switch_interrupt_flag
    LDR R3, [R2]
    CMP R3, #1
    BEQ _reswitch
    MOV R3, #1
    STR R3, [R2]

    LDR R2, =rt_interrupt_from_thread
    STR R0, [R2]

_reswitch:
    LDR R2, =rt_interrupt_to_thread
    STR R1, [R2]

    LDR R0, =ICSR
    LDR R1, =PENDSVSET_BIT
    STR R1, [R0]
    BX LR





    .global PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:

    MRS R2, PRIMASK
    CPSID I


    LDR R0, =rt_thread_switch_interrupt_flag
    LDR R1, [R0]
    CBZ R1, pendsv_exit


    MOV R1, #0
    STR R1, [R0]

    LDR R0, =rt_interrupt_from_thread
    LDR R1, [R0]
    CBZ R1, switch_to_thread

    MRS R1, PSP
    STMFD R1!, {R4 - R11}
    LDR R0, [R0]
    STR R1, [R0]

switch_to_thread:
    LDR R1, =rt_interrupt_to_thread
    LDR R1, [R1]
    LDR R1, [R1]

    LDMFD R1!, {R4 - R11}
    MSR PSP, R1

pendsv_exit:

    MSR PRIMASK, R2

    ORR LR, LR, #0x04
    BX LR





    .global rt_hw_context_switch_to
    .type rt_hw_context_switch_to, %function
rt_hw_context_switch_to:
    LDR R1, =rt_interrupt_to_thread
    STR R0, [R1]


    LDR R1, =rt_interrupt_from_thread
    MOV R0, #0
    STR R0, [R1]


    LDR R1, =rt_thread_switch_interrupt_flag
    MOV R0, #1
    STR R0, [R1]


    LDR R0, =SHPR3
    LDR R1, =PENDSV_PRI_LOWEST
    LDR.W R2, [R0,#0]
    ORR R1, R1, R2
    STR R1, [R0]

    LDR R0, =ICSR
    LDR R1, =PENDSVSET_BIT
    STR R1, [R0]


    LDR r0, =SCB_VTOR
    LDR r0, [r0]
    LDR r0, [r0]
    NOP
    MSR msp, r0


    CPSIE F
    CPSIE I




    .global rt_hw_interrupt_thread_switch
    .type rt_hw_interrupt_thread_switch, %function
rt_hw_interrupt_thread_switch:
    BX LR
    NOP

    .global HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:

    MRS r0, msp
    TST lr, #0x04
    BEQ _get_sp_done
    MRS r0, psp
_get_sp_done:

    STMFD r0!, {r4 - r11}
    STMFD r0!, {lr}

    TST lr, #0x04
    BEQ _update_msp
    MSR psp, r0
    B _update_done
_update_msp:
    MSR msp, r0
_update_done:

    PUSH {LR}
    BL rt_hw_hard_fault_exception
    POP {LR}

    ORR LR, LR, #0x04
    BX LR





    .global rt_hw_interrupt_check
    .type rt_hw_interrupt_check, %function
rt_hw_interrupt_check:
    MRS R0, IPSR
    BX LR
