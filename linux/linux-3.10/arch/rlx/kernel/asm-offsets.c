/*
 * offset.c: Calculate pt_regs and task_struct offsets.
 *
 * Copyright (C) 1996 David S. Miller
 * Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003 Ralf Baechle
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 *
 * Kevin Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.
 */
#include <linux/compat.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/kbuild.h>
#include <linux/suspend.h>
#include <asm/ptrace.h>
#include <asm/processor.h>

#include <linux/kvm_host.h>

void output_ptreg_defines(void)
{
	COMMENT("MIPS pt_regs offsets.");
	OFFSET(PT_R0, pt_regs, regs[0]);
	OFFSET(PT_R1, pt_regs, regs[1]);
	OFFSET(PT_R2, pt_regs, regs[2]);
	OFFSET(PT_R3, pt_regs, regs[3]);
	OFFSET(PT_R4, pt_regs, regs[4]);
	OFFSET(PT_R5, pt_regs, regs[5]);
	OFFSET(PT_R6, pt_regs, regs[6]);
	OFFSET(PT_R7, pt_regs, regs[7]);
	OFFSET(PT_R8, pt_regs, regs[8]);
	OFFSET(PT_R9, pt_regs, regs[9]);
	OFFSET(PT_R10, pt_regs, regs[10]);
	OFFSET(PT_R11, pt_regs, regs[11]);
	OFFSET(PT_R12, pt_regs, regs[12]);
	OFFSET(PT_R13, pt_regs, regs[13]);
	OFFSET(PT_R14, pt_regs, regs[14]);
	OFFSET(PT_R15, pt_regs, regs[15]);
	OFFSET(PT_R16, pt_regs, regs[16]);
	OFFSET(PT_R17, pt_regs, regs[17]);
	OFFSET(PT_R18, pt_regs, regs[18]);
	OFFSET(PT_R19, pt_regs, regs[19]);
	OFFSET(PT_R20, pt_regs, regs[20]);
	OFFSET(PT_R21, pt_regs, regs[21]);
	OFFSET(PT_R22, pt_regs, regs[22]);
	OFFSET(PT_R23, pt_regs, regs[23]);
	OFFSET(PT_R24, pt_regs, regs[24]);
	OFFSET(PT_R25, pt_regs, regs[25]);
	OFFSET(PT_R26, pt_regs, regs[26]);
	OFFSET(PT_R27, pt_regs, regs[27]);
	OFFSET(PT_R28, pt_regs, regs[28]);
	OFFSET(PT_R29, pt_regs, regs[29]);
	OFFSET(PT_R30, pt_regs, regs[30]);
	OFFSET(PT_R31, pt_regs, regs[31]);
	OFFSET(PT_LO, pt_regs, lo);
	OFFSET(PT_HI, pt_regs, hi);
	OFFSET(PT_EPC, pt_regs, cp0_epc);
	OFFSET(PT_BVADDR, pt_regs, cp0_badvaddr);
	OFFSET(PT_STATUS, pt_regs, cp0_status);
	OFFSET(PT_CAUSE, pt_regs, cp0_cause);

	DEFINE(PT_SIZE, sizeof(struct pt_regs));
	BLANK();
}

void output_task_defines(void)
{
	COMMENT("MIPS task_struct offsets.");
	OFFSET(TASK_STATE, task_struct, state);
	OFFSET(TASK_THREAD_INFO, task_struct, stack);
	OFFSET(TASK_FLAGS, task_struct, flags);
	OFFSET(TASK_MM, task_struct, mm);
	OFFSET(TASK_PID, task_struct, pid);
	DEFINE(TASK_STRUCT_SIZE, sizeof(struct task_struct));
	BLANK();
}

void output_thread_info_defines(void)
{
	COMMENT("MIPS thread_info offsets.");
	OFFSET(TI_TASK, thread_info, task);
	OFFSET(TI_EXEC_DOMAIN, thread_info, exec_domain);
	OFFSET(TI_FLAGS, thread_info, flags);
	OFFSET(TI_TP_VALUE, thread_info, tp_value);
	OFFSET(TI_CPU, thread_info, cpu);
	OFFSET(TI_PRE_COUNT, thread_info, preempt_count);
	OFFSET(TI_ADDR_LIMIT, thread_info, addr_limit);
	OFFSET(TI_RESTART_BLOCK, thread_info, restart_block);
	OFFSET(TI_REGS, thread_info, regs);
	DEFINE(_THREAD_SIZE, THREAD_SIZE);
	DEFINE(_THREAD_MASK, THREAD_MASK);
	BLANK();
}

void output_thread_defines(void)
{
	COMMENT("MIPS specific thread_struct offsets.");
	OFFSET(THREAD_REG16, task_struct, thread.reg16);
	OFFSET(THREAD_REG17, task_struct, thread.reg17);
	OFFSET(THREAD_REG18, task_struct, thread.reg18);
	OFFSET(THREAD_REG19, task_struct, thread.reg19);
	OFFSET(THREAD_REG20, task_struct, thread.reg20);
	OFFSET(THREAD_REG21, task_struct, thread.reg21);
	OFFSET(THREAD_REG22, task_struct, thread.reg22);
	OFFSET(THREAD_REG23, task_struct, thread.reg23);
	OFFSET(THREAD_REG29, task_struct, thread.reg29);
	OFFSET(THREAD_REG30, task_struct, thread.reg30);
	OFFSET(THREAD_REG31, task_struct, thread.reg31);
	OFFSET(THREAD_STATUS, task_struct,
	       thread.cp0_status);
#ifdef CONFIG_CPU_HAS_FPU
	OFFSET(THREAD_FPU, task_struct, thread.fpu);
#endif
#ifdef CONFIG_CPU_HAS_RADIAX
	OFFSET(THREAD_RADIAX, task_struct, thread.radiax);
#endif

	OFFSET(THREAD_BVADDR, task_struct, \
	       thread.cp0_badvaddr);
	OFFSET(THREAD_BUADDR, task_struct, \
	       thread.cp0_baduaddr);
	OFFSET(THREAD_ECODE, task_struct, \
	       thread.error_code);
	BLANK();
}

#ifdef CONFIG_CPU_HAS_FPU
void output_thread_fpu_defines(void)
{
	OFFSET(THREAD_FPR0, task_struct, thread.fpu.fpr[0]);
	OFFSET(THREAD_FPR1, task_struct, thread.fpu.fpr[1]);
	OFFSET(THREAD_FPR2, task_struct, thread.fpu.fpr[2]);
	OFFSET(THREAD_FPR3, task_struct, thread.fpu.fpr[3]);
	OFFSET(THREAD_FPR4, task_struct, thread.fpu.fpr[4]);
	OFFSET(THREAD_FPR5, task_struct, thread.fpu.fpr[5]);
	OFFSET(THREAD_FPR6, task_struct, thread.fpu.fpr[6]);
	OFFSET(THREAD_FPR7, task_struct, thread.fpu.fpr[7]);
	OFFSET(THREAD_FPR8, task_struct, thread.fpu.fpr[8]);
	OFFSET(THREAD_FPR9, task_struct, thread.fpu.fpr[9]);
	OFFSET(THREAD_FPR10, task_struct, thread.fpu.fpr[10]);
	OFFSET(THREAD_FPR11, task_struct, thread.fpu.fpr[11]);
	OFFSET(THREAD_FPR12, task_struct, thread.fpu.fpr[12]);
	OFFSET(THREAD_FPR13, task_struct, thread.fpu.fpr[13]);
	OFFSET(THREAD_FPR14, task_struct, thread.fpu.fpr[14]);
	OFFSET(THREAD_FPR15, task_struct, thread.fpu.fpr[15]);
	OFFSET(THREAD_FPR16, task_struct, thread.fpu.fpr[16]);
	OFFSET(THREAD_FPR17, task_struct, thread.fpu.fpr[17]);
	OFFSET(THREAD_FPR18, task_struct, thread.fpu.fpr[18]);
	OFFSET(THREAD_FPR19, task_struct, thread.fpu.fpr[19]);
	OFFSET(THREAD_FPR20, task_struct, thread.fpu.fpr[20]);
	OFFSET(THREAD_FPR21, task_struct, thread.fpu.fpr[21]);
	OFFSET(THREAD_FPR22, task_struct, thread.fpu.fpr[22]);
	OFFSET(THREAD_FPR23, task_struct, thread.fpu.fpr[23]);
	OFFSET(THREAD_FPR24, task_struct, thread.fpu.fpr[24]);
	OFFSET(THREAD_FPR25, task_struct, thread.fpu.fpr[25]);
	OFFSET(THREAD_FPR26, task_struct, thread.fpu.fpr[26]);
	OFFSET(THREAD_FPR27, task_struct, thread.fpu.fpr[27]);
	OFFSET(THREAD_FPR28, task_struct, thread.fpu.fpr[28]);
	OFFSET(THREAD_FPR29, task_struct, thread.fpu.fpr[29]);
	OFFSET(THREAD_FPR30, task_struct, thread.fpu.fpr[30]);
	OFFSET(THREAD_FPR31, task_struct, thread.fpu.fpr[31]);

	OFFSET(THREAD_FCR31, task_struct, thread.fpu.fcr31);
	BLANK();
}
#endif

#ifdef CONFIG_CPU_HAS_RADIAX
void output_thread_radiax_defines(void)
{
	OFFSET(THREAD_CBS0, task_struct, thread.radiax.radiaxr[0]);
	OFFSET(THREAD_CBS1, task_struct, thread.radiax.radiaxr[1]);
	OFFSET(THREAD_CBS2, task_struct, thread.radiax.radiaxr[2]);
	OFFSET(THREAD_CBE0, task_struct, thread.radiax.radiaxr[3]);
	OFFSET(THREAD_CBE1, task_struct, thread.radiax.radiaxr[4]);
	OFFSET(THREAD_CBE2, task_struct, thread.radiax.radiaxr[5]);
	OFFSET(THREAD_LPS0, task_struct, thread.radiax.radiaxr[6]);
	OFFSET(THREAD_LPE0, task_struct, thread.radiax.radiaxr[7]);
	OFFSET(THREAD_LPC0, task_struct, thread.radiax.radiaxr[8]);
	OFFSET(THREAD_MMD , task_struct, thread.radiax.radiaxr[9]);
	OFFSET(THREAD_M0LL, task_struct, thread.radiax.radiaxr[10]);
	OFFSET(THREAD_M0LH, task_struct, thread.radiax.radiaxr[11]);
	OFFSET(THREAD_M0HL, task_struct, thread.radiax.radiaxr[12]);
	OFFSET(THREAD_M0HH, task_struct, thread.radiax.radiaxr[13]);
	OFFSET(THREAD_M1LL, task_struct, thread.radiax.radiaxr[14]);
	OFFSET(THREAD_M1LH, task_struct, thread.radiax.radiaxr[15]);
	OFFSET(THREAD_M1HL, task_struct, thread.radiax.radiaxr[16]);
	OFFSET(THREAD_M1HH, task_struct, thread.radiax.radiaxr[17]);
	OFFSET(THREAD_M2LL, task_struct, thread.radiax.radiaxr[18]);
	OFFSET(THREAD_M2LH, task_struct, thread.radiax.radiaxr[19]);
	OFFSET(THREAD_M2HL, task_struct, thread.radiax.radiaxr[20]);
	OFFSET(THREAD_M2HH, task_struct, thread.radiax.radiaxr[21]);
	OFFSET(THREAD_M3LL, task_struct, thread.radiax.radiaxr[22]);
	OFFSET(THREAD_M3LH, task_struct, thread.radiax.radiaxr[23]);
	OFFSET(THREAD_M3HL, task_struct, thread.radiax.radiaxr[24]);
	OFFSET(THREAD_M3HH, task_struct, thread.radiax.radiaxr[25]);
}
#endif

void output_mm_defines(void)
{
	COMMENT("Size of struct page");
	DEFINE(STRUCT_PAGE_SIZE, sizeof(struct page));
	BLANK();
	COMMENT("Linux mm_struct offsets.");
	OFFSET(MM_USERS, mm_struct, mm_users);
	OFFSET(MM_PGD, mm_struct, pgd);
	OFFSET(MM_CONTEXT, mm_struct, context);
	BLANK();
	DEFINE(_PGD_T_SIZE, sizeof(pgd_t));
	DEFINE(_PMD_T_SIZE, sizeof(pmd_t));
	DEFINE(_PTE_T_SIZE, sizeof(pte_t));
	BLANK();
	DEFINE(_PGD_T_LOG2, PGD_T_LOG2);
#ifndef __PAGETABLE_PMD_FOLDED
	DEFINE(_PMD_T_LOG2, PMD_T_LOG2);
#endif
	DEFINE(_PTE_T_LOG2, PTE_T_LOG2);
	BLANK();
	DEFINE(_PGD_ORDER, PGD_ORDER);
#ifndef __PAGETABLE_PMD_FOLDED
	DEFINE(_PMD_ORDER, PMD_ORDER);
#endif
	DEFINE(_PTE_ORDER, PTE_ORDER);
	BLANK();
	DEFINE(_PMD_SHIFT, PMD_SHIFT);
	DEFINE(_PGDIR_SHIFT, PGDIR_SHIFT);
	BLANK();
	DEFINE(_PTRS_PER_PGD, PTRS_PER_PGD);
	DEFINE(_PTRS_PER_PMD, PTRS_PER_PMD);
	DEFINE(_PTRS_PER_PTE, PTRS_PER_PTE);
	BLANK();
	DEFINE(_PAGE_SHIFT, PAGE_SHIFT);
	DEFINE(_PAGE_SIZE, PAGE_SIZE);
	BLANK();
}

void output_sc_defines(void)
{
	COMMENT("Linux sigcontext offsets.");
	OFFSET(SC_REGS, sigcontext, sc_regs);
#ifdef CONFIG_CPU_HAS_FPU
	OFFSET(SC_FPREGS, sigcontext, sc_fpregs);
#endif
	OFFSET(SC_MDHI, sigcontext, sc_mdhi);
	OFFSET(SC_MDLO, sigcontext, sc_mdlo);
	OFFSET(SC_PC, sigcontext, sc_pc);
#ifdef CONFIG_CPU_HAS_FPU
	OFFSET(SC_FPC_CSR, sigcontext, sc_fpc_csr);
	OFFSET(SC_FPC_EIR, sigcontext, sc_fpc_eir);
#endif
	BLANK();
}

void output_signal_defined(void)
{
	COMMENT("Linux signal numbers.");
	DEFINE(_SIGHUP, SIGHUP);
	DEFINE(_SIGINT, SIGINT);
	DEFINE(_SIGQUIT, SIGQUIT);
	DEFINE(_SIGILL, SIGILL);
	DEFINE(_SIGTRAP, SIGTRAP);
	DEFINE(_SIGIOT, SIGIOT);
	DEFINE(_SIGABRT, SIGABRT);
	DEFINE(_SIGEMT, SIGEMT);
	DEFINE(_SIGFPE, SIGFPE);
	DEFINE(_SIGKILL, SIGKILL);
	DEFINE(_SIGBUS, SIGBUS);
	DEFINE(_SIGSEGV, SIGSEGV);
	DEFINE(_SIGSYS, SIGSYS);
	DEFINE(_SIGPIPE, SIGPIPE);
	DEFINE(_SIGALRM, SIGALRM);
	DEFINE(_SIGTERM, SIGTERM);
	DEFINE(_SIGUSR1, SIGUSR1);
	DEFINE(_SIGUSR2, SIGUSR2);
	DEFINE(_SIGCHLD, SIGCHLD);
	DEFINE(_SIGPWR, SIGPWR);
	DEFINE(_SIGWINCH, SIGWINCH);
	DEFINE(_SIGURG, SIGURG);
	DEFINE(_SIGIO, SIGIO);
	DEFINE(_SIGSTOP, SIGSTOP);
	DEFINE(_SIGTSTP, SIGTSTP);
	DEFINE(_SIGCONT, SIGCONT);
	DEFINE(_SIGTTIN, SIGTTIN);
	DEFINE(_SIGTTOU, SIGTTOU);
	DEFINE(_SIGVTALRM, SIGVTALRM);
	DEFINE(_SIGPROF, SIGPROF);
	DEFINE(_SIGXCPU, SIGXCPU);
	DEFINE(_SIGXFSZ, SIGXFSZ);
	BLANK();
}

#ifdef CONFIG_HIBERNATION
void output_pbe_defines(void)
{
	COMMENT(" Linux struct pbe offsets. ");
	OFFSET(PBE_ADDRESS, pbe, address);
	OFFSET(PBE_ORIG_ADDRESS, pbe, orig_address);
	OFFSET(PBE_NEXT, pbe, next);
	DEFINE(PBE_SIZE, sizeof(struct pbe));
	BLANK();
}
#endif

#ifdef CONFIG_KVM
void output_kvm_defines(void)
{
	COMMENT(" KVM/MIPS Specfic offsets. ");
	DEFINE(VCPU_ARCH_SIZE, sizeof(struct kvm_vcpu_arch));
	OFFSET(VCPU_RUN, kvm_vcpu, run);
	OFFSET(VCPU_HOST_ARCH, kvm_vcpu, arch);

	OFFSET(VCPU_HOST_EBASE, kvm_vcpu_arch, host_ebase);
	OFFSET(VCPU_GUEST_EBASE, kvm_vcpu_arch, guest_ebase);

	OFFSET(VCPU_HOST_STACK, kvm_vcpu_arch, host_stack);
	OFFSET(VCPU_HOST_GP, kvm_vcpu_arch, host_gp);

	OFFSET(VCPU_HOST_CP0_BADVADDR, kvm_vcpu_arch, host_cp0_badvaddr);
	OFFSET(VCPU_HOST_CP0_CAUSE, kvm_vcpu_arch, host_cp0_cause);
	OFFSET(VCPU_HOST_EPC, kvm_vcpu_arch, host_cp0_epc);
	OFFSET(VCPU_HOST_ENTRYHI, kvm_vcpu_arch, host_cp0_entryhi);

	OFFSET(VCPU_GUEST_INST, kvm_vcpu_arch, guest_inst);

	OFFSET(VCPU_R0, kvm_vcpu_arch, gprs[0]);
	OFFSET(VCPU_R1, kvm_vcpu_arch, gprs[1]);
	OFFSET(VCPU_R2, kvm_vcpu_arch, gprs[2]);
	OFFSET(VCPU_R3, kvm_vcpu_arch, gprs[3]);
	OFFSET(VCPU_R4, kvm_vcpu_arch, gprs[4]);
	OFFSET(VCPU_R5, kvm_vcpu_arch, gprs[5]);
	OFFSET(VCPU_R6, kvm_vcpu_arch, gprs[6]);
	OFFSET(VCPU_R7, kvm_vcpu_arch, gprs[7]);
	OFFSET(VCPU_R8, kvm_vcpu_arch, gprs[8]);
	OFFSET(VCPU_R9, kvm_vcpu_arch, gprs[9]);
	OFFSET(VCPU_R10, kvm_vcpu_arch, gprs[10]);
	OFFSET(VCPU_R11, kvm_vcpu_arch, gprs[11]);
	OFFSET(VCPU_R12, kvm_vcpu_arch, gprs[12]);
	OFFSET(VCPU_R13, kvm_vcpu_arch, gprs[13]);
	OFFSET(VCPU_R14, kvm_vcpu_arch, gprs[14]);
	OFFSET(VCPU_R15, kvm_vcpu_arch, gprs[15]);
	OFFSET(VCPU_R16, kvm_vcpu_arch, gprs[16]);
	OFFSET(VCPU_R17, kvm_vcpu_arch, gprs[17]);
	OFFSET(VCPU_R18, kvm_vcpu_arch, gprs[18]);
	OFFSET(VCPU_R19, kvm_vcpu_arch, gprs[19]);
	OFFSET(VCPU_R20, kvm_vcpu_arch, gprs[20]);
	OFFSET(VCPU_R21, kvm_vcpu_arch, gprs[21]);
	OFFSET(VCPU_R22, kvm_vcpu_arch, gprs[22]);
	OFFSET(VCPU_R23, kvm_vcpu_arch, gprs[23]);
	OFFSET(VCPU_R24, kvm_vcpu_arch, gprs[24]);
	OFFSET(VCPU_R25, kvm_vcpu_arch, gprs[25]);
	OFFSET(VCPU_R26, kvm_vcpu_arch, gprs[26]);
	OFFSET(VCPU_R27, kvm_vcpu_arch, gprs[27]);
	OFFSET(VCPU_R28, kvm_vcpu_arch, gprs[28]);
	OFFSET(VCPU_R29, kvm_vcpu_arch, gprs[29]);
	OFFSET(VCPU_R30, kvm_vcpu_arch, gprs[30]);
	OFFSET(VCPU_R31, kvm_vcpu_arch, gprs[31]);
	OFFSET(VCPU_LO, kvm_vcpu_arch, lo);
	OFFSET(VCPU_HI, kvm_vcpu_arch, hi);
	OFFSET(VCPU_PC, kvm_vcpu_arch, pc);
	OFFSET(VCPU_COP0, kvm_vcpu_arch, cop0);
	OFFSET(VCPU_GUEST_KERNEL_ASID, kvm_vcpu_arch, guest_kernel_asid);
	OFFSET(VCPU_GUEST_USER_ASID, kvm_vcpu_arch, guest_user_asid);

	OFFSET(COP0_TLB_HI, mips_coproc, reg[MIPS_CP0_TLB_HI][0]);
	OFFSET(COP0_STATUS, mips_coproc, reg[MIPS_CP0_STATUS][0]);
	BLANK();
}
#endif
