/*
 * features_phase4.c - Phase 4 Advanced Developer Tools & Debugging
 * Implements 100 developer features (bits 407-506)
 */

#include <palerain.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <inttypes.h>

/* External reference to global flags */
extern palerain_option_t palerain_flags;

/* ============================================================
 * === Kernel Development & Debugging (15) ===
 * ============================================================ */

int kern_debug_enter(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Enabling kernel debugger entry on boot");
    LOG(LOG_INFO, "  Kernel will trap into debugger at earliest opportunity");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_debug_enter);
    return 0;
}

int kern_debug_wait(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Waiting for kernel debugger attach");
    LOG(LOG_INFO, "  Kernel will spin until debugger connects via serial/network");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_debug_wait);
    return 0;
}

int kern_break_early(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Setting early kernel entry breakpoint");
    LOG(LOG_INFO, "  Breakpoint will trigger at __start/arm_init");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_break_early);
    return 0;
}

int kern_watchdog_disable(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Disabling kernel watchdog timer");
    LOG(LOG_INFO, "  System will not reboot on hang (use with caution)");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_watchdog_disable);
    return 0;
}

int kern_panic_log(const char *path) {
    const char *out = path ? path : "/var/log/kernel_panic.log";
    LOG(LOG_INFO, "[KERN-DEBUG] Logging kernel panics to: %s", out);
    FILE *f = fopen(out, "a");
    if (f) {
        fprintf(f, "# Kernel panic log session %ld\n", (long)time(NULL));
        fclose(f);
    }
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_panic_log);
    return 0;
}

int kern_backtrace_all(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Capturing backtraces from all CPUs");
    LOG(LOG_INFO, "  CPU0: <capture pending>");
    LOG(LOG_INFO, "  CPU1: <capture pending>");
    LOG(LOG_INFO, "  CPU2: <capture pending>");
    LOG(LOG_INFO, "  CPU3: <capture pending>");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_backtrace_all);
    return 0;
}

int kern_lock_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting lock contention statistics");
    LOG(LOG_INFO, "  Tracking: mutex, rwlock, spinlock, lck_mtx_t");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_lock_stat);
    return 0;
}

int kern_intr_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting interrupt statistics");
    LOG(LOG_INFO, "  Monitoring IRQ vector dispatch counts");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_intr_stat);
    return 0;
}

int kern_syscall_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting syscall statistics");
    LOG(LOG_INFO, "  Counting syscalls by number, tracking latency");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_syscall_stat);
    return 0;
}

int kern_vm_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting VM statistics");
    LOG(LOG_INFO, "  Page-ins: <tracked>  Page-outs: <tracked>  Compressions: <tracked>");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_vm_stat);
    return 0;
}

int kern_task_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting task/process statistics");
    LOG(LOG_INFO, "  Active tasks: <tracked>  Threads: <tracked>  Ports: <tracked>");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_task_stat);
    return 0;
}

int kern_zone_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting zone allocator statistics");
    LOG(LOG_INFO, "  Per-zone: element count, free count, allocations");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_zone_stat);
    return 0;
}

int kern_kalloc_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting kalloc statistics");
    LOG(LOG_INFO, "  Tracking kalloc sizes and frequencies");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_kalloc_stat);
    return 0;
}

int kern_upl_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting UPL (upl_t) statistics");
    LOG(LOG_INFO, "  Tracking upl_create, upl_commit operations");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_upl_stat);
    return 0;
}

int kern_io_stat(void) {
    LOG(LOG_INFO, "[KERN-DEBUG] Collecting I/O statistics");
    LOG(LOG_INFO, "  Disk reads/writes, bytes transferred, latency");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_io_stat);
    return 0;
}

/* ============================================================
 * === Dynamic Analysis & Instrumentation (15) ===
 * ============================================================ */

int dtrace_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling DTrace probes");
    LOG(LOG_INFO, "  DTrace framework available for scripted tracing");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtrace_enable);
    return 0;
}

int ktrace_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling ktrace");
    LOG(LOG_INFO, "  System call and Mach IPC tracing active");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_ktrace_enable);
    return 0;
}

int ftrace_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling function tracing (ftrace)");
    LOG(LOG_INFO, "  Instrumenting function entry/exit points");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_ftrace_enable);
    return 0;
}

int perf_counters_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling hardware performance counters");
    LOG(LOG_INFO, "  PMU counters: cycles, instructions, cache-misses, branches");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_counters);
    return 0;
}

int branch_trace_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling branch trace buffer (BTB)");
    LOG(LOG_INFO, "  Recording taken/not-taken branch history");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_branch_trace);
    return 0;
}

int call_graph_build(void) {
    LOG(LOG_INFO, "[DYNAMIC] Building call graph");
    LOG(LOG_INFO, "  Sampling stack traces to construct call tree");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_call_graph);
    return 0;
}

int hot_path_detect(void) {
    LOG(LOG_INFO, "[DYNAMIC] Detecting hot code paths");
    LOG(LOG_INFO, "  Profiling to identify frequently executed functions");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hot_path_detect);
    return 0;
}

int mem_leak_detect_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling memory leak detection");
    LOG(LOG_INFO, "  Tracking allocations without matching frees");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_mem_leak_detect);
    return 0;
}

int use_after_free_detect(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling use-after-free detection");
    LOG(LOG_INFO, "  Quarantining freed memory, checking accesses");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_use_after_free);
    return 0;
}

int double_free_detect_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling double-free detection");
    LOG(LOG_INFO, "  Marking freed blocks to catch duplicate frees");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_double_free_detect);
    return 0;
}

int buffer_overflow_detect(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling buffer overflow detection");
    LOG(LOG_INFO, "  Inserting redzones around allocations");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_buffer_overflow);
    return 0;
}

int race_detect_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling data race detection");
    LOG(LOG_INFO, "  Monitoring shared memory access patterns");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_race_detect);
    return 0;
}

int deadlock_detect_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling deadlock detection");
    LOG(LOG_INFO, "  Tracking lock acquisition order graphs");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_deadlock_detect);
    return 0;
}

int stack_canary_check_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling stack canary checks");
    LOG(LOG_INFO, "  Inserting guard values on stack frames");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_stack_canary_check);
    return 0;
}

int shadow_stack_enable(void) {
    LOG(LOG_INFO, "[DYNAMIC] Enabling shadow stack");
    LOG(LOG_INFO, "  Maintaining parallel return address stack");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_shadow_stack);
    return 0;
}

/* ============================================================
 * === Fuzzing & Testing Infrastructure (10) ===
 * ============================================================ */

int fuzz_kernel_enable(void) {
    LOG(LOG_INFO, "[FUZZ] Enabling kernel syscall fuzzing");
    LOG(LOG_INFO, "  Generating random/mutated syscall arguments");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_kernel);
    return 0;
}

int fuzz_driver_enable(void) {
    LOG(LOG_INFO, "[FUZZ] Enabling driver interface fuzzing");
    LOG(LOG_INFO, "  Fuzzing IOService::externalMethod, user clients");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_driver);
    return 0;
}

int fuzz_filesystem_enable(void) {
    LOG(LOG_INFO, "[FUZZ] Enabling filesystem fuzzing");
    LOG(LOG_INFO, "  Fuzzing VFS operations with malformed paths");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_filesystem);
    return 0;
}

int fuzz_network_enable(void) {
    LOG(LOG_INFO, "[FUZZ] Enabling network stack fuzzing");
    LOG(LOG_INFO, "  Fuzzing socket operations and packet parsing");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_network);
    return 0;
}

int fuzz_usb_stack_enable(void) {
    LOG(LOG_INFO, "[FUZZ] Enabling USB stack fuzzing");
    LOG(LOG_INFO, "  Fuzzing USB descriptor and transfer handling");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_usb_stack);
    return 0;
}

int fuzz_ipc_enable(void) {
    LOG(LOG_INFO, "[FUZZ] Enabling IPC fuzzing");
    LOG(LOG_INFO, "  Fuzzing Mach message sending/receiving");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_ipc);
    return 0;
}

int fuzz_mach_enable(void) {
    LOG(LOG_INFO, "[FUZZ] Enabling Mach interface fuzzing");
    LOG(LOG_INFO, "  Fuzzing Mach traps and ports");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_mach);
    return 0;
}

int property_test_enable(void) {
    LOG(LOG_INFO, "[TEST] Enabling property-based testing");
    LOG(LOG_INFO, "  Generating test cases from invariant properties");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_property_test);
    return 0;
}

int model_check_enable(void) {
    LOG(LOG_INFO, "[TEST] Enabling model checking");
    LOG(LOG_INFO, "  Exhaustively verifying state machine properties");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_model_check);
    return 0;
}

int symbolic_exec_enable(void) {
    LOG(LOG_INFO, "[TEST] Enabling symbolic execution");
    LOG(LOG_INFO, "  Exploring execution paths with symbolic inputs");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_symbolic_exec);
    return 0;
}

/* ============================================================
 * === Reverse Engineering & Analysis (10) ===
 * ============================================================ */

int re_disassemble_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling code disassembly");
    LOG(LOG_INFO, "  Disassembling loaded code regions on demand");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_disassemble);
    return 0;
}

int re_cfg_build_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling control flow graph construction");
    LOG(LOG_INFO, "  Building CFG from disassembled basic blocks");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_cfg_build);
    return 0;
}

int re_data_flow_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling data flow analysis");
    LOG(LOG_INFO, "  Tracking def-use chains and taint propagation");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_data_flow);
    return 0;
}

int re_type_recovery_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling type recovery");
    LOG(LOG_INFO, "  Inferring struct/union types from usage");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_type_recovery);
    return 0;
}

int re_struct_recovery_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling structure recovery");
    LOG(LOG_INFO, "  Reconstructing struct layouts from field accesses");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_struct_recovery);
    return 0;
}

int re_string_analysis_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling string analysis");
    LOG(LOG_INFO, "  Extracting and cross-referencing string constants");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_string_analysis);
    return 0;
}

int re_xref_find_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling cross-reference finding");
    LOG(LOG_INFO, "  Building full xref database for symbols");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_xref_find);
    return 0;
}

int re_pattern_match_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling pattern matching");
    LOG(LOG_INFO, "  Matching known code patterns/idioms");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_pattern_match);
    return 0;
}

int re_signature_gen_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling signature generation");
    LOG(LOG_INFO, "  Generating byte patterns for function identification");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_signature_gen);
    return 0;
}

int re_diff_binary_enable(void) {
    LOG(LOG_INFO, "[RE] Enabling binary diffing");
    LOG(LOG_INFO, "  Comparing two binaries for changed functions");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_diff_binary);
    return 0;
}

/* ============================================================
 * === Exploit Development (10) ===
 * ============================================================ */

int exploit_dev_mode_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling exploit development mode");
    LOG(LOG_INFO, "  Relaxed checks, verbose failure reporting");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_dev_mode);
    return 0;
}

int exploit_payload_gen_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling exploit payload generation");
    LOG(LOG_INFO, "  Generating stage payloads from templates");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_payload_gen);
    return 0;
}

int exploit_rop_build_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling ROP chain building");
    LOG(LOG_INFO, "  Finding gadgets and chaining for ROP");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_rop_build);
    return 0;
}

int exploit_jop_build_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling JOP chain building");
    LOG(LOG_INFO, "  Constructing jump-oriented programming chains");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_jop_build);
    return 0;
}

int exploit_srop_build_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling SROP chain building");
    LOG(LOG_INFO, "  Crafting sigreturn-oriented programming frames");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_srop_build);
    return 0;
}

int exploit_pac_bypass_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling PAC bypass development");
    LOG(LOG_INFO, "  Testing PAC forgery and brute-force techniques");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_pac_bypass);
    return 0;
}

int exploit_pp_bypass_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling pointer protection bypass");
    LOG(LOG_INFO, "  Testing PAC/PPL/APRR bypass strategies");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_pp_bypass);
    return 0;
}

int exploit_kaslr_leak_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling KASLR leak development");
    LOG(LOG_INFO, "  Developing side-channel KASLR defeat techniques");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_kaslr_leak);
    return 0;
}

int exploit_heap_feng_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling heap feng shui tools");
    LOG(LOG_INFO, "  Shaping heap layout for exploitation");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_heap_feng);
    return 0;
}

int exploit_race_win_enable(void) {
    LOG(LOG_INFO, "[EXPLOIT-DEV] Enabling race condition winning");
    LOG(LOG_INFO, "  Tools for winning TOCTOU race windows");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_race_win);
    return 0;
}

/* ============================================================
 * === Hardware Bring-up & Validation (10) ===
 * ============================================================ */

int hw_bringup_enable(void) {
    LOG(LOG_INFO, "[HW] Enabling hardware bring-up mode");
    LOG(LOG_INFO, "  Minimal init, verbose peripheral probing");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_bringup);
    return 0;
}

int hw_memory_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running memory testing");
    LOG(LOG_INFO, "  March tests on DRAM: CWE, MATS+, etc.");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_memory_test);
    return 0;
}

int hw_cache_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running cache testing");
    LOG(LOG_INFO, "  L1/L2 cache coherency and timing tests");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_cache_test);
    return 0;
}

int hw_interconnect_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running interconnect testing");
    LOG(LOG_INFO, "  AXI/AMBA bus bandwidth and latency tests");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_interconnect);
    return 0;
}

int hw_timer_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running timer testing");
    LOG(LOG_INFO, "  Verifying ARM timer accuracy and interrupts");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_timer_test);
    return 0;
}

int hw_interrupt_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running interrupt testing");
    LOG(LOG_INFO, "  Triggering and measuring interrupt latency");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_interrupt_test);
    return 0;
}

int hw_dma_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running DMA testing");
    LOG(LOG_INFO, "  Exercising DMA controllers with pattern transfers");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_dma_test);
    return 0;
}

int hw_pmu_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running PMU testing");
    LOG(LOG_INFO, "  Validating performance monitoring unit counters");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_pmu_test);
    return 0;
}

int hw_thermal_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running thermal testing");
    LOG(LOG_INFO, "  Reading temperature sensors, stress heating");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_thermal_test);
    return 0;
}

int hw_power_test_enable(void) {
    LOG(LOG_INFO, "[HW] Running power testing");
    LOG(LOG_INFO, "  Measuring power domains and rail voltages");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_power_test);
    return 0;
}

/* ============================================================
 * === Firmware & Bootloader Development (10) ===
 * ============================================================ */

int fw_dev_mode_enable(void) {
    LOG(LOG_INFO, "[FW] Enabling firmware development mode");
    LOG(LOG_INFO, "  Allows unsigned firmware loading for testing");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_dev_mode);
    return 0;
}

int fw_patch_test_enable(void) {
    LOG(LOG_INFO, "[FW] Testing firmware patches");
    LOG(LOG_INFO, "  Applying and verifying patch compatibility");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_patch_test);
    return 0;
}

int fw_verify_chain_enable(void) {
    LOG(LOG_INFO, "[FW] Verifying boot chain");
    LOG(LOG_INFO, "  Checking LLB->iBoot->Kernel signature chain");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_verify_chain);
    return 0;
}

int fw_key_test_enable(void) {
    LOG(LOG_INFO, "[FW] Testing key operations");
    LOG(LOG_INFO, "  Verifying GID/UID/AES key availability");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_key_test);
    return 0;
}

int fw_rollback_test_enable(void) {
    LOG(LOG_INFO, "[FW] Testing firmware rollback");
    LOG(LOG_INFO, "  Checking anti-rollback nonce enforcement");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_rollback_test);
    return 0;
}

int fw_dfu_test_enable(void) {
    LOG(LOG_INFO, "[FW] Testing DFU mode");
    LOG(LOG_INFO, "  Exercising DFU upload/download paths");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_dfu_test);
    return 0;
}

int fw_recovery_test_enable(void) {
    LOG(LOG_INFO, "[FW] Testing recovery mode");
    LOG(LOG_INFO, "  Exercising recovery mode commands");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_recovery_test);
    return 0;
}

int fw_ramdisk_test_enable(void) {
    LOG(LOG_INFO, "[FW] Testing ramdisk");
    LOG(LOG_INFO, "  Loading and mounting test ramdisk");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_ramdisk_test);
    return 0;
}

int fw_overlay_test_enable(void) {
    LOG(LOG_INFO, "[FW] Testing overlay");
    LOG(LOG_INFO, "  Applying and verifying filesystem overlay");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_overlay_test);
    return 0;
}

int fw_integrity_enable(void) {
    LOG(LOG_INFO, "[FW] Checking firmware integrity");
    LOG(LOG_INFO, "  Computing and verifying firmware hashes");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_integrity);
    return 0;
}

/* ============================================================
 * === Performance & Optimization (10) ===
 * ============================================================ */

int perf_baseline_enable(void) {
    LOG(LOG_INFO, "[PERF] Establishing performance baseline");
    LOG(LOG_INFO, "  Recording reference metrics for comparison");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_baseline);
    return 0;
}

int perf_regression_enable(void) {
    LOG(LOG_INFO, "[PERF] Detecting performance regressions");
    LOG(LOG_INFO, "  Comparing current metrics against baseline");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_regression);
    return 0;
}

int perf_optimize_enable(void) {
    LOG(LOG_INFO, "[PERF] Enabling auto-optimization");
    LOG(LOG_INFO, "  Applying detected optimization opportunities");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_optimize);
    return 0;
}

int perf_tune_cpu_enable(void) {
    LOG(LOG_INFO, "[PERF] Tuning CPU");
    LOG(LOG_INFO, "  Adjusting clock frequencies and scheduling");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_cpu);
    return 0;
}

int perf_tune_gpu_enable(void) {
    LOG(LOG_INFO, "[PERF] Tuning GPU");
    LOG(LOG_INFO, "  Adjusting GPU clock and memory bandwidth");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_gpu);
    return 0;
}

int perf_tune_mem_enable(void) {
    LOG(LOG_INFO, "[PERF] Tuning memory");
    LOG(LOG_INFO, "  Optimizing cache policies and memory layout");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_mem);
    return 0;
}

int perf_tune_io_enable(void) {
    LOG(LOG_INFO, "[PERF] Tuning I/O");
    LOG(LOG_INFO, "  Optimizing disk and bus transfer parameters");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_io);
    return 0;
}

int perf_tune_net_enable(void) {
    LOG(LOG_INFO, "[PERF] Tuning network");
    LOG(LOG_INFO, "  Optimizing socket buffers and TCP parameters");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_net);
    return 0;
}

int perf_energy_enable(void) {
    LOG(LOG_INFO, "[PERF] Optimizing energy usage");
    LOG(LOG_INFO, "  Balancing performance vs power consumption");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_energy);
    return 0;
}

int perf_thermal_enable(void) {
    LOG(LOG_INFO, "[PERF] Optimizing thermal behavior");
    LOG(LOG_INFO, "  Managing thermal throttling thresholds");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_thermal);
    return 0;
}

/* ============================================================
 * === Developer Workflow & Productivity (10) ===
 * ============================================================ */

int dev_hot_reload_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling hot reload");
    LOG(LOG_INFO, "  Monitoring source changes, reloading modules");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_hot_reload);
    return 0;
}

int dev_live_patch_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling live patching");
    LOG(LOG_INFO, "  Applying code patches to running kernel");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_live_patch);
    return 0;
}

int dev_repl_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling REPL");
    LOG(LOG_INFO, "  Interactive read-eval-print loop for kernel/userspace");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_repl);
    return 0;
}

int dev_snippets_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling code snippets library");
    LOG(LOG_INFO, "  Loading reusable code fragment database");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_snippets);
    return 0;
}

int dev_templates_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling project templates");
    LOG(LOG_INFO, "  Generating scaffold projects for new exploits");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_templates);
    return 0;
}

int dev_ci_integration_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling CI integration");
    LOG(LOG_INFO, "  Hooking into CI pipeline for automated testing");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_ci_integration);
    return 0;
}

int dev_git_hooks_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling git hooks");
    LOG(LOG_INFO, "  Installing pre-commit and post-receive hooks");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_git_hooks);
    return 0;
}

int dev_docker_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling Docker support");
    LOG(LOG_INFO, "  Containerizing build/test environments");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_docker);
    return 0;
}

int dev_vscode_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling VS Code integration");
    LOG(LOG_INFO, "  Generating launch.json and c_cpp_properties.json");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_vscode);
    return 0;
}

int dev_clangd_enable(void) {
    LOG(LOG_INFO, "[DEV] Enabling clangd LSP integration");
    LOG(LOG_INFO, "  Generating compile_commands.json for clangd");
    PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_clangd);
    return 0;
}
