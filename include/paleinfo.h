/*
 * jbinit - https://github.com/palera1n/jbinit
 *
 * This file is part of jbinit
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PALEINFO_H
#define PALEINFO_H


#include <stdint.h>

#define PALEINFO_VERSION 2U
#define PALERAIN_OPTION_WORDS 8  /* 8 * 64 = 512 bits for 400+ features */

typedef uint64_t palerain_option_t[PALERAIN_OPTION_WORDS];

#define PALERAIN_OPTION_BIT(x) (1ULL << ((x) % 64))
#define PALERAIN_OPTION_WORD(x) ((x) / 64)
#define PALERAIN_OPTION_SET(flags, x) ((flags)[PALERAIN_OPTION_WORD(x)] |= PALERAIN_OPTION_BIT(x))
#define PALERAIN_OPTION_CLEAR(flags, x) ((flags)[PALERAIN_OPTION_WORD(x)] &= ~PALERAIN_OPTION_BIT(x))
#define PALERAIN_OPTION_TEST(flags, x) ((flags)[PALERAIN_OPTION_WORD(x)] & PALERAIN_OPTION_BIT(x))
#define PALERAIN_OPTION_ZERO(flags) do { for (int i = 0; i < PALERAIN_OPTION_WORDS; i++) (flags)[i] = 0; } while(0)
#define PALERAIN_OPTION_COPY(dst, src) do { for (int i = 0; i < PALERAIN_OPTION_WORDS; i++) (dst)[i] = (src)[i]; } while(0)
#define PALERAIN_OPTION_OR(dst, src) do { for (int i = 0; i < PALERAIN_OPTION_WORDS; i++) (dst)[i] |= (src)[i]; } while(0)
#define PALERAIN_OPTION_AND(dst, src) do { for (int i = 0; i < PALERAIN_OPTION_WORDS; i++) (dst)[i] &= (src)[i]; } while(0)
#define PALERAIN_OPTION_ANY(flags) ( \
    (flags)[0] || (flags)[1] || (flags)[2] || (flags)[3] || (flags)[4] || (flags)[5] || (flags)[6] \
)

// unified palera1n options
#define palerain_option_rootful              0  /* rootful jailbreak */
#define palerain_option_rootless             1  /* rootless jailbreak */
#define palerain_option_setup_rootful        2  /* create fakefs */
// #define palerain_option_3                 3  /* Deprecated and removed */
#define palerain_option_setup_partial_root   4  /* fakefs creating should be partial */
#define palerain_option_checkrain_is_clone   5  /* supplied checkra1n is checkra1n clone */
#define palerain_option_rootless_livefs      6  /* mount root livefs on rootless */
#define palerain_option_ssv                  7  /* has signed system volume */
// #define palerain_option_8                 8  /* reserved */
#define palerain_option_clean_fakefs         9  /* clean fakefs, but does not delete it */
#define palerain_option_tui                  10  /* terminal user interface */
// #define palerain_option_gui               11  /* graphical user interface*/
#define palerain_option_dfuhelper_only       12  /* dfuhelper only */
#define palerain_option_pongo_exit           13  /* boot to clean pongo shell */
#define palerain_option_demote               14  /* Demote effective production fuse status */
#define palerain_option_pongo_full           15  /* Boot to pongo with default images and options */
#define palerain_option_palerain_version     16  /* Print version */
#define palerain_option_exit_recovery        17  /* Exit recovery mode */
#define palerain_option_reboot_device        18  /* Reboot device in normal mode */
#define palerain_option_enter_recovery       19  /* Enter recovery mode */
#define palerain_option_device_info          20  /* Print device info */
#define palerain_option_no_colors            21  /* no colors */
#define palerain_option_bind_mount           22  /* bind mounts should be used (always true iOS 15+)*/
#define palerain_option_overlay              23  /* there is an overlay (should always be true) */
#define palerain_option_force_revert         24  /* Unjailbreak */
#define palerain_option_safemode             25  /* Enter safe mode */
#define palerain_option_verbose_boot         26  /* verbose boot */
// #define palerain_option_sf_ssh            27  /* safe mode with bootstrap ssh */
// #define palerain_option_sf_launchdaemons  28  /* safe mode with launchdaemons */
// #define palerain_option_emerg_mode        29  /* emergency mode (early boot usb shell) */
#define palerain_option_jbinit_log_to_file   30  /* deprecated (kept for compatibility) */
#define palerain_option_cli                  31  /* CLI mode */
#define palerain_option_telnetd              32  /* Enable TELNET daemon */
#define palerain_option_pwned_dfu            33  /* Skip exploit, use pwned DFU mode */

#define palerain_option_verbose_boot_log    61  /* Verbose boot logging */
#define palerain_option_test1                62  /* Developer test option 1 */
#define palerain_option_test2                63  /* Developer test option 2 */
#define palerain_option_simulator_pwned_dfu 34  /* Skip exploit, use pwned DFU mode */
#define palerain_option_diagnostics         35  /* Diagnostics */
#define palerain_option_dry_run             36  /* Dry run - show what would be done without executing */
#define palerain_option_log_file            37  /* Log to file */

/* === Device Management Features (8) === */
#define palerain_option_device_list         38  /* List all connected devices */
#define palerain_option_device_filter       39  /* Filter devices by type */
#define palerain_option_device_group        40  /* Group devices by iOS version */
#define palerain_option_device_export       41  /* Export device list */
#define palerain_option_device_autodetect   42  /* Auto-detect device */
#define palerain_option_device_rename       43  /* Rename device */
#define palerain_option_device_notes        44  /* Add notes to device */
#define palerain_option_device_favorite     45  /* Mark device as favorite */

/* === Safety & Recovery Features (8) === */
#define palerain_option_backup_restore      46  /* Backup/Restore device data */
#define palerain_option_recovery_auto       47  /* Automate recovery mode */
#define palerain_option_dfu_auto            48  /* Automate DFU mode */
#define palerain_option_safe_boot           49  /* Boot in safe mode */
#define palerain_option_bootloop_detect     50  /* Detect and recover from bootloops */
#define palerain_option_auto_recover        51  /* Auto recovery mode */
#define palerain_option_data_protect        52  /* Protect user data during operations */
#define palerain_option_emergency_exit      53  /* Emergency exit handler */

/* === Exploit & Boot Features (8) === */
#define palerain_option_multi_exploit       54  /* Multi-exploit support */
#define palerain_option_exploit_chain       55  /* Exploit chaining */
#define palerain_option_custom_payload      56  /* Custom payload support */
#define palerain_option_boot_presets        57  /* Boot argument presets */
#define palerain_option_kern_patches        58  /* Kernel patch management */
#define palerain_option_ramdisk_cust        59  /* Ramdisk customization */
#define palerain_option_overlay_mgmt        60  /* Overlay management */

/* === User Experience Features (8) === */
#define palerain_option_interactive_wizard  64  /* Interactive wizard */
#define palerain_option_progress_anim       65  /* Progress animations */
#define palerain_option_colored_themes      66  /* Colored themes */
#define palerain_option_sound_notify        67  /* Sound notifications */
#define palerain_option_haptic_feedback     68  /* Haptic feedback */
#define palerain_option_multi_lang          69  /* Multi-language support */
#define palerain_option_accessibility       70  /* Accessibility mode */
#define palerain_option_quick_actions       71  /* Quick actions */

/* === Developer & Debug Features (8) === */
#define palerain_option_debug_console       72  /* Debug console */
#define palerain_option_mem_inspect         73  /* Memory inspector */
#define palerain_option_reg_viewer          74  /* Register viewer */
#define palerain_option_syscall_trace       75  /* Syscall tracer */
#define palerain_option_kern_debug          76  /* Kernel debugger */
#define palerain_option_perf_profile        77  /* Performance profiler */
#define palerain_option_crash_report        78  /* Crash reporter */
#define palerain_option_log_analyze         79  /* Log analyzer */

/* === Automation & Scripting Features (8) === */
#define palerain_option_batch_ops           80  /* Batch operations */
#define palerain_option_scheduled_tasks     81  /* Scheduled tasks */
#define palerain_option_script_engine       82  /* Script engine */
#define palerain_option_plugin_system       83  /* Plugin system */
#define palerain_option_api_server          84  /* API server */
#define palerain_option_webhook             85  /* Webhook support */
#define palerain_option_ci_cd               86  /* CI/CD integration */
#define palerain_option_config_templates    87  /* Config templates */

/* === Maintenance & Utility Features (8) === */
#define palerain_option_auto_update         88  /* Auto-update */
#define palerain_option_self_diagnostics    89  /* Self-diagnostics */
#define palerain_option_cache_manage        90  /* Cache management */
#define palerain_option_log_rotation        91  /* Log rotation */
#define palerain_option_disk_cleanup        92  /* Disk cleanup */
#define palerain_option_health_monitor      93  /* Health monitoring */
#define palerain_option_version_check       94  /* Version checker */
#define palerain_option_dep_manager         95  /* Dependency manager */

/* === Bonus Features (2) === */
#define palerain_option_benchmark           96  /* Device benchmarking */
#define palerain_option_persist_check       97  /* Jailbreak persistence check */

/* === Additional Features (8) === */
#define palerain_option_device_info_detailed 98  /* Detailed device info */
#define palerain_option_device_tree         99  /* Show device tree */
#define palerain_option_kernel_dump         100 /* Kernel memory dump */
#define palerain_option_usb_sniffer         101 /* USB traffic sniffer */
#define palerain_option_boot_chain          102 /* Boot chain analysis */
#define palerain_option_firmware_keys       103 /* Firmware key extraction */
#define palerain_option_pongo_shell         104 /* Interactive PongoOS shell */
#define palerain_option_recovery_mode_test  105 /* Recovery mode stress test */

/* === Ultra Verbose & Logging Features (10) === */
#define palerain_option_ultra_verbose       106 /* Ultra verbose output for all stages */
#define palerain_option_trace_usb           107 /* Trace all USB transactions */
#define palerain_option_trace_memory        108 /* Trace memory allocations/frees */
#define palerain_option_trace_exploit       109 /* Trace exploit execution step-by-step */
#define palerain_option_trace_pongo         110 /* Trace PongoOS command/response */
#define palerain_option_trace_kpf           111 /* Trace kernel patchfinder operations */
#define palerain_option_trace_ramdisk       112 /* Trace ramdisk upload/extract */
#define palerain_option_trace_overlay       113 /* Trace overlay operations */
#define palerain_option_trace_bootx         114 /* Trace bootx execution */
#define palerain_option_log_all_stages      115 /* Log every stage to separate files */

/* === PongoOS Stage Diagnostics (10) === */
#define palerain_option_pongo_verbose       116 /* Verbose PongoOS boot logging */
#define palerain_option_pongo_cmd_log       117 /* Log all PongoOS commands sent */
#define palerain_option_pongo_response_log  118 /* Log all PongoOS responses */
#define palerain_option_pongo_mem_map       119 /* Dump PongoOS memory map */
#define palerain_option_pongo_reg_dump      120 /* Dump PongoOS register state */
#define palerain_option_pongo_usb_log       121 /* Log PongoOS USB endpoint traffic */
#define palerain_option_pongo_exception_log 122 /* Log PongoOS exceptions/crashes */
#define palerain_option_pongo_task_list     123 /* List PongoOS tasks/threads */
#define palerain_option_pongo_heap_dump     124 /* Dump PongoOS heap */
#define palerain_option_pongo_symbols       125 /* Load/dump PongoOS symbol table */

/* === Exploit Stage Diagnostics (10) === */
#define palerain_option_exploit_verbose     126 /* Verbose exploit execution logging */
#define palerain_option_exploit_step_trace  127 /* Trace each exploit step */
#define palerain_option_exploit_mem_trace   128 /* Trace exploit memory operations */
#define palerain_option_exploit_reg_trace   129 /* Trace register changes during exploit */
#define palerain_option_exploit_timing      130 /* Measure exploit stage timing */
#define palerain_option_exploit_retry_log   131 /* Log exploit retry attempts */
#define palerain_option_exploit_payload_log 132 /* Log exploit payload bytes */
#define palerain_option_exploit_state_dump  133 /* Dump exploit state machine */
#define palerain_option_exploit_side_effects 134 /* Detect exploit side effects */
#define palerain_option_exploit_validation  135 /* Validate exploit success criteria */

/* === Kernel & Memory Diagnostics (10) === */
#define palerain_option_kernel_verbose      136 /* Verbose kernel boot logging */
#define palerain_option_kernel_mem_map      137 /* Dump kernel memory map */
#define palerain_option_kernel_slide_log    138 /* Log kernel slide calculation */
#define palerain_option_kernel_patch_log    139 /* Log each kernel patch applied */
#define palerain_option_kernel_symbols      140 /* Load kernel symbol table */
#define palerain_option_kernel_stack_trace  141 /* Capture kernel stack traces */
#define palerain_option_kernel_page_track   142 /* Track kernel page allocations */
#define palerain_option_kernel_vm_log       143 /* Log VM operations */
#define palerain_option_kernel_kalloc_log   144 /* Log kalloc/kfree operations */
#define palerain_option_kernel_zone_log     145 /* Log zone allocations */

/* === USB & Communication Diagnostics (10) === */
#define palerain_option_usb_verbose         146 /* Verbose USB communication logging */
#define palerain_option_usb_control_trace   147 /* Trace USB control transfers */
#define palerain_option_usb_bulk_trace      148 /* Trace USB bulk transfers */
#define palerain_option_usb_iso_trace       149 /* Trace USB isochronous transfers */
#define palerain_option_usb_device_tree     150 /* Log USB device tree */
#define palerain_option_usb_endpoint_log    151 /* Log USB endpoint info */
#define palerain_option_usb_speed_log       152 /* Log USB speed negotiation */
#define palerain_option_usb_power_log       153 /* Log USB power events */
#define palerain_option_usb_reset_trace     154 /* Trace USB bus resets */
#define palerain_option_usb_hub_log         155 /* Log USB hub topology */

/* === Device Tree & Firmware Analysis (10) === */
#define palerain_option_dtree_full_dump     156 /* Full device tree dump */
#define palerain_option_dtree_compare       157 /* Compare device trees */
#define palerain_option_dtree_property_log  158 /* Log all device tree properties */
#define palerain_option_dtree_compatible    159 /* Log compatible strings */
#define palerain_option_dtree_reg_dump      160 /* Dump device tree reg properties */
#define palerain_option_dtree_interrupts    161 /* Log interrupt mappings */
#define palerain_option_dtree_clocks        162 /* Log clock tree */
#define palerain_option_dtree_power         163 /* Log power domain tree */
#define palerain_option_dtree_gpio          164 /* Log GPIO configuration */
#define palerain_option_firmware_analyze    165 /* Analyze firmware components */

/* === Security & Cryptography Features (10) === */
#define palerain_option_aes_engine_trace    166 /* Trace AES engine operations */
#define palerain_option_sep_comm_log        167 /* Log SEP communication */
#define palerain_option_sep_firmware_dump   168 /* Dump SEP firmware */
#define palerain_option_sep_key_trace       169 /* Trace SEP key operations */
#define palerain_option_gid_uid_trace       170 /* Trace GID/UID key usage */
#define palerain_option_firmware_decrypt    171 /* Decrypt firmware components */
#define palerain_option_img4_parse_log      172 /* Log IMG4 parsing */
#define palerain_option_ap_ticket_log       173 /* Log APTicket validation */
#define palerain_option_sep_ticket_log      174 /* Log SEPTicket validation */
#define palerain_option_keybag_dump         175 /* Dump keybag contents */

/* === Advanced Jailbreak Features (10) === */
#define palerain_option_rootless_verbose    176 /* Verbose rootless boot logging */
#define palerain_option_fakefs_verbose      177 /* Verbose fakefs operations */
#define palerain_option_bind_mount_log      178 /* Log bind mount operations */
#define palerain_option_overlay_verbose     179 /* Verbose overlay operations */
#define palerain_option_sandbox_log         180 /* Log sandbox operations */
#define palerain_option_amfi_log            181 /* Log AMFI operations */
#define palerain_option_codesign_log        182 /* Log code signing ops */
#define palerain_option_entitlement_log     183 /* Log entitlement checks */
#define palerain_option_sip_log             184 /* Log SIP bypass operations */
#define palerain_option_tfp0_log            185 /* Log tfp0 operations */

/* === Automation & Testing Features (10) === */
#define palerain_option_auto_retry          186 /* Auto-retry failed stages */
#define palerain_option_max_retries         187 /* Set max retry count */
#define palerain_option_test_mode           188 /* Test mode (no device changes) */
#define palerain_option_stress_test         189 /* Stress test all stages */
#define palerain_option_fuzz_usb            190 /* Fuzz USB communication */
#define palerain_option_fuzz_exploit        191 /* Fuzz exploit parameters */
#define palerain_option_fuzz_pongo          192 /* Fuzz PongoOS commands */
#define palerain_option_ci_mode             193 /* CI/CD optimized mode */
#define palerain_option_benchmark_all       194 /* Benchmark all stages */
#define palerain_option_perf_report         195 /* Generate performance report */

/* === Developer & Research Features (10) === */
#define palerain_option_research_mode       196 /* Research mode - extra checks */
#define palerain_option_debug_breakpoints   197 /* Enable debug breakpoints */
#define palerain_option_memory_watchpoints  198 /* Memory watchpoints */
#define palerain_option_instruction_trace   199 /* Instruction-level tracing */
#define palerain_option_coverage_tracking   200 /* Code coverage tracking */
#define palerain_option_custom_hooks        201 /* Custom hook support */
#define palerain_option_plugin_debug        202 /* Plugin system debug */
#define palerain_option_api_debug           203 /* API server debug */
#define palerain_option_script_debug        204 /* Script engine debug */
#define palerain_option_experimental        205 /* Enable experimental features */

/* === Phase 2: Advanced Diagnostics & Analysis (100 features, bits 206-305) === */

/* === Hardware & Peripheral Diagnostics (10) === */
#define palerain_option_gpio_trace          206 /* Trace GPIO operations */
#define palerain_option_i2c_trace           207 /* Trace I2C bus transactions */
#define palerain_option_spi_trace           208 /* Trace SPI bus transactions */
#define palerain_option_uart_trace          209 /* Trace UART communication */
#define palerain_option_pmic_trace          210 /* Trace PMIC operations */
#define palerain_option_display_trace       211 /* Trace display controller */
#define palerain_option_touch_trace         212 /* Trace touchscreen controller */
#define palerain_option_audio_trace         213 /* Trace audio subsystem */
#define palerain_option_camera_trace        214 /* Trace camera subsystem */
#define palerain_option_sensor_trace        215 /* Trace sensor hub */

/* === Power & Thermal Management (10) === */
#define palerain_option_power_domains       216 /* Dump power domain tree */
#define palerain_option_thermal_zones       217 /* Log thermal zone data */
#define palerain_option_battery_log         218 /* Log battery status */
#define palerain_option_charging_log        219 /* Log charging events */
#define palerain_option_dvfs_log            220 /* Log DVFS transitions */
#define palerain_option_idle_states         221 /* Log CPU idle states */
#define palerain_option_suspend_log         222 /* Log suspend/resume */
#define palerain_option_wakeup_sources      223 /* Log wakeup sources */
#define palerain_option_power_stats         224 /* Collect power statistics */
#define palerain_option_energy_model        225 /* Energy model analysis */

/* === Filesystem & Storage Diagnostics (10) === */
#define palerain_option_apfs_verbose        226 /* Verbose APFS logging */
#define palerain_option_apfs_snapshots      227 /* List APFS snapshots */
#define palerain_option_apfs_clones         228 /* Log APFS clone operations */
#define palerain_option_apfs_encryption     229 /* Trace APFS encryption */
#define palerain_option_nand_trace          230 /* Trace NAND operations */
#define palerain_option_nvme_trace          231 /* Trace NVMe commands */
#define palerain_option_storage_stats       232 /* Storage I/O statistics */
#define palerain_option_trim_log            233 /* Log TRIM operations */
#define palerain_option_wear_leveling       234 /* Log wear leveling */
#define palerain_option_bad_block_log       235 /* Log bad block management */

/* === Network & Connectivity Diagnostics (10) === */
#define palerain_option_wifi_trace          236 /* Trace WiFi operations */
#define palerain_option_bluetooth_trace     237 /* Trace Bluetooth operations */
#define palerain_option_cellular_trace      238 /* Trace cellular modem */
#define palerain_option_nfc_trace           239 /* Trace NFC operations */
#define palerain_option_tcpip_trace         240 /* Trace TCP/IP stack */
#define palerain_option_dns_trace           241 /* Trace DNS resolution */
#define palerain_option_tls_trace           242 /* Trace TLS handshakes */
#define palerain_option_vpn_trace           243 /* Trace VPN operations */
#define palerain_option_airdrop_trace       244 /* Trace AirDrop */
#define palerain_option_continuity_trace    245 /* Trace Continuity */

/* === Graphics & Display Diagnostics (10) === */
#define palerain_option_gpu_trace           246 /* Trace GPU commands */
#define palerain_option_metal_trace         247 /* Trace Metal API calls */
#define palerain_option_iosurface_trace     248 /* Trace IOSurface operations */
#define palerain_option_cvms_trace          249 /* Trace CVMS server */
#define palerain_option_compositor_trace    250 /* Trace window compositor */
#define palerain_option_display_link        251 /* Trace display link */
#define palerain_option_vsync_trace         252 /* Trace VSync events */
#define palerain_option_layer_trace         253 /* Trace layer operations */
#define palerain_option_color_trace         254 /* Trace color management */
#define palerain_option_hdr_trace           255 /* Trace HDR operations */

/* === Security & Enclave Diagnostics (10) === */
#define palerain_option_sep_verbose         256 /* Verbose SEP logging */
#define palerain_option_sep_mailbox         257 /* Trace SEP mailbox */
#define palerain_option_sep_memory          258 /* Dump SEP memory map */
#define palerain_option_sep_firmware_log    259 /* Log SEP firmware events */
#define palerain_option_tz_verbose          260 /* Verbose TrustZone logging */
#define palerain_option_tz_memory           261 /* Dump TZ memory map */
#define palerain_option_tz_smc_trace        262 /* Trace SMC calls */
#define palerain_option_secure_boot_log     263 /* Log secure boot process */
#define palerain_option_keybag_trace        264 /* Trace keybag operations */
#define palerain_option_entanglement_log    265 /* Log entanglement events */

/* === Memory Management Diagnostics (10) === */
#define palerain_option_vm_map_dump         266 /* Dump VM maps */
#define palerain_option_vm_fault_trace      267 /* Trace VM faults */
#define palerain_option_physmem_log         268 /* Log physical memory */
#define palerain_option_page_reclaim        269 /* Log page reclaim */
#define palerain_option_compression_log     270 /* Log memory compression */
#define palerain_option_swap_trace          271 /* Trace swap operations */
#define palerain_option_jemalloc_trace      272 /* Trace jemalloc ops */
#define palerain_option_zone_trace          273 /* Trace zone allocator */
#define palerain_option_vm_pressure         274 /* Log VM pressure events */
#define palerain_option_memory_tags         275 /* Track memory tags */

/* === Scheduler & Process Diagnostics (10) === */
#define palerain_option_sched_trace         276 /* Trace scheduler decisions */
#define palerain_option_thread_trace        277 /* Trace thread operations */
#define palerain_option_workqueue_trace     278 /* Trace workqueue ops */
#define palerain_option_timer_trace         279 /* Trace timer operations */
#define palerain_option_ipc_trace           280 /* Trace IPC messages */
#define palerain_option_mach_trace          281 /* Trace Mach calls */
#define palerain_option_exception_trace     282 /* Trace exceptions */
#define palerain_option_signal_trace        283 /* Trace signal delivery */
#define palerain_option_proc_trace          284 /* Trace process lifecycle */
#define palerain_option_task_trace          285 /* Trace task operations */

/* === IOKit & Driver Diagnostics (10) === */
#define palerain_option_ioregistry_dump     286 /* Dump IORegistry */
#define palerain_option_iokit_trace         287 /* Trace IOKit calls */
#define palerain_option_driver_match        289 /* Log driver matching */
#define palerain_option_property_trace      290 /* Trace property changes */
#define palerain_option_service_trace       291 /* Trace service events */
#define palerain_option_notification_trace  292 /* Trace notifications */
#define palerain_option_powerman_trace      293 /* Trace power management */
#define palerain_option_thermal_trace       294 /* Trace thermal management */
#define palerain_option_pci_trace           295 /* Trace PCIe operations */
#define palerain_option_thunderbolt_trace   296 /* Trace Thunderbolt */

/* === Boot & Startup Diagnostics (10) === */
#define palerain_option_iboot_verbose       297 /* Verbose iBoot logging */
#define palerain_option_iboot_env           298 /* Dump iBoot environment */
#define palerain_option_iboot_commands      299 /* Log iBoot commands */
#define palerain_option_diags_log           300 /* Log diagnostics */
#define palerain_option_recovery_log        301 /* Log recovery mode */
#define palerain_option_dfu_log             302 /* Log DFU mode */
#define palerain_option_ramdisk_log         303 /* Log ramdisk boot */
#define palerain_option_kernelcache_log     304 /* Log kernelcache load */
#define palerain_option_prelink_log         305 /* Log prelink process */
#define palerain_option_boot_progress       306 /* Log boot progress */

/* === PongoOS Stage Diagnostics (20) === */
#define palerain_option_pongo_stage_verbose 307 /* Verbose PongoOS stage logging */
#define palerain_option_pongo_stage_log     308 /* Log PongoOS stage transitions */
#define palerain_option_pongo_mem_log       309 /* Log PongoOS memory usage */
#define palerain_option_pongo_heap_log      310 /* Log PongoOS heap allocations */
#define palerain_option_pongo_task_log      311 /* Log PongoOS task scheduling */
#define palerain_option_pongo_irq_log       312 /* Log PongoOS IRQ handling */
#define palerain_option_pongo_timer_log     313 /* Log PongoOS timer events */
#define palerain_option_pongo_uart_log      314 /* Log PongoOS UART I/O */
#define palerain_option_pongo_usb_event_log 315 /* Log PongoOS USB events */
#define palerain_option_pongo_wdt_log       316 /* Log PongoOS watchdog */
#define palerain_option_pongo_crash_log     317 /* Log PongoOS crashes */
#define palerain_option_pongo_syscall_log   318 /* Log PongoOS syscalls */
#define palerain_option_pongo_ipc_log       319 /* Log PongoOS IPC */
#define palerain_option_pongo_sem_log       320 /* Log PongoOS semaphores */
#define palerain_option_pongo_mutex_log     321 /* Log PongoOS mutexes */
#define palerain_option_pongo_cond_log      322 /* Log PongoOS condition variables */
#define palerain_option_pongo_queue_log     323 /* Log PongoOS message queues */
#define palerain_option_pongo_pool_log      324 /* Log PongoOS memory pools */
#define palerain_option_pongo_dma_log       325 /* Log PongoOS DMA operations */
#define palerain_option_pongo_pmu_log       326 /* Log PongoOS PMU events */

/* === Kernel Exploitation Diagnostics (10) === */
#define palerain_option_exploit_diag_verbose 327 /* Verbose exploit diagnostics logging */
#define palerain_option_exploit_stage_log   328 /* Log exploit stage transitions */
#define palerain_option_exploit_heap_log    329 /* Log exploit heap operations */
#define palerain_option_exploit_rop_log     330 /* Log ROP chain construction */
#define palerain_option_exploit_kcall_log   331 /* Log kcall operations */
#define palerain_option_exploit_kread_log   332 /* Log kernel reads */
#define palerain_option_exploit_kwrite_log  333 /* Log kernel writes */
#define palerain_option_exploit_pac_log     334 /* Log PAC bypass attempts */
#define palerain_option_exploit_pp_log      335 /* Log pointer protection bypass */
#define palerain_option_exploit_slide_log   336 /* Log kernel slide discovery */

/* === Checkra1n Integration Diagnostics (10) === */
#define palerain_option_checkra1n_verbose   337 /* Verbose checkra1n logging */
#define palerain_option_checkra1n_stage_log 338 /* Log checkra1n stages */
#define palerain_option_checkra1n_pongo_log 339 /* Log checkra1n PongoOS handshake */
#define palerain_option_checkra1n_boot_log  340 /* Log checkra1n boot process */
#define palerain_option_checkra1n_ramdisk_log 341 /* Log checkra1n ramdisk */
#define palerain_option_checkra1n_kernel_log 342 /* Log checkra1n kernel patching */
#define palerain_option_checkra1n_hook_log  343 /* Log checkra1n hook installation */
#define palerain_option_checkra1n_shell_log 344 /* Log checkra1n shell */
#define palerain_option_checkra1n_ssh_log   345 /* Log checkra1n SSH */
#define palerain_option_checkra1n_cleanup_log 346 /* Log checkra1n cleanup */

/* === Advanced USB Diagnostics (10) === */
#define palerain_option_usb_diag_verbose    347 /* Verbose USB diagnostics logging */
#define palerain_option_usb_ep_diag_log     348 /* Log USB endpoint diagnostics */
#define palerain_option_usb_transfer_log    349 /* Log USB transfers */
#define palerain_option_usb_control_log     350 /* Log USB control transfers */
#define palerain_option_usb_bulk_log        351 /* Log USB bulk transfers */
#define palerain_option_usb_interrupt_log   352 /* Log USB interrupt transfers */
#define palerain_option_usb_isochronous_log 353 /* Log USB isochronous transfers */
#define palerain_option_usb_device_log      354 /* Log USB device events */
#define palerain_option_usb_interface_log   355 /* Log USB interface events */
#define palerain_option_usb_config_log      356 /* Log USB configuration events */

/* === DFU Mode Diagnostics (10) === */
#define palerain_option_dfu_verbose         357 /* Verbose DFU logging */
#define palerain_option_dfu_state_log       358 /* Log DFU state transitions */
#define palerain_option_dfu_command_log     359 /* Log DFU commands */
#define palerain_option_dfu_upload_log      360 /* Log DFU uploads */
#define palerain_option_dfu_download_log    361 /* Log DFU downloads */
#define palerain_option_dfu_getstatus_log   362 /* Log DFU getstatus */
#define palerain_option_dfu_clrstatus_log   363 /* Log DFU clrstatus */
#define palerain_option_dfu_abort_log       364 /* Log DFU abort */
#define palerain_option_dfu_detach_log      365 /* Log DFU detach */
#define palerain_option_dfu_reset_log       366 /* Log DFU reset */

/* === Recovery Mode Diagnostics (10) === */
#define palerain_option_recovery_verbose    367 /* Verbose recovery logging */
#define palerain_option_recovery_state_log  368 /* Log recovery state transitions */
#define palerain_option_recovery_command_log 369 /* Log recovery commands */
#define palerain_option_recovery_upload_log 370 /* Log recovery uploads */
#define palerain_option_recovery_download_log 371 /* Log recovery downloads */
#define palerain_option_recovery_nonce_log  372 /* Log recovery nonce handling */
#define palerain_option_recovery_apnonce_log 373 /* Log recovery apnonce */
#define palerain_option_recovery_sepnonce_log 374 /* Log recovery sepnonce */
#define palerain_option_recovery_ticket_log 375 /* Log recovery ticket */
#define palerain_option_recovery_reset_log  376 /* Log recovery reset */

/* === Bootloader Diagnostics (10) === */
#define palerain_option_bootloader_verbose  377 /* Verbose bootloader logging */
#define palerain_option_bootloader_stage_log 378 /* Log bootloader stages */
#define palerain_option_bootloader_iboot_log 379 /* Log iBoot operations */
#define palerain_option_bootloader_ibec_log 380 /* Log iBEC operations */
#define palerain_option_bootloader_ibss_log 381 /* Log iBSS operations */
#define palerain_option_bootloader_lowlevel_log 382 /* Log low-level bootloader */
#define palerain_option_bootloader_mem_log  383 /* Log bootloader memory */
#define palerain_option_bootloader_reg_log  384 /* Log bootloader registers */
#define palerain_option_bootloader_dev_log  385 /* Log bootloader devices */
#define palerain_option_bootloader_cmd_log  386 /* Log bootloader commands */

/* === Filesystem Diagnostics (10) === */
#define palerain_option_fs_verbose          387 /* Verbose filesystem logging */
#define palerain_option_fs_mount_log        388 /* Log filesystem mounts */
#define palerain_option_fs_unmount_log      389 /* Log filesystem unmounts */
#define palerain_option_fs_read_log         390 /* Log filesystem reads */
#define palerain_option_fs_write_log        391 /* Log filesystem writes */
#define palerain_option_fs_create_log       392 /* Log filesystem creates */
#define palerain_option_fs_delete_log       393 /* Log filesystem deletes */
#define palerain_option_fs_rename_log       394 /* Log filesystem renames */
#define palerain_option_fs_stat_log         395 /* Log filesystem stats */
#define palerain_option_fs_xattr_log        396 /* Log filesystem xattrs */

/* === Network Diagnostics (10) === */
#define palerain_option_net_verbose         397 /* Verbose network logging */
#define palerain_option_net_packet_log      398 /* Log network packets */
#define palerain_option_net_socket_log      399 /* Log socket operations */
#define palerain_option_net_route_log       400 /* Log routing operations */
#define palerain_option_net_arp_log         401 /* Log ARP operations */
#define palerain_option_net_ndp_log         402 /* Log NDP operations */
#define palerain_option_net_dhcp_log        403 /* Log DHCP operations */
#define palerain_option_net_dns_log         404 /* Log DNS operations */
#define palerain_option_net_tls_log         405 /* Log TLS operations */
#define palerain_option_net_http_log        406 /* Log HTTP operations */

/* === Phase 4: Advanced Developer Tools & Debugging (100 features, bits 407-506) === */

/* === Kernel Development & Debugging (15) === */
#define palerain_option_kern_debug_enter    407 /* Enter kernel debugger on boot */
#define palerain_option_kern_debug_wait     408 /* Wait for kernel debugger attach */
#define palerain_option_kern_break_early    409 /* Break at earliest kernel entry */
#define palerain_option_kern_watchdog_disable 410 /* Disable kernel watchdog */
#define palerain_option_kern_panic_log      411 /* Log kernel panics to file */
#define palerain_option_kern_backtrace_all  412 /* Capture all CPU backtraces */
#define palerain_option_kern_lock_stat      413 /* Log lock contention stats */
#define palerain_option_kern_intr_stat      414 /* Log interrupt statistics */
#define palerain_option_kern_syscall_stat   415 /* Log syscall statistics */
#define palerain_option_kern_vm_stat        416 /* Log VM statistics */
#define palerain_option_kern_task_stat      417 /* Log task/process statistics */
#define palerain_option_kern_zone_stat      418 /* Log zone allocator statistics */
#define palerain_option_kern_kalloc_stat    419 /* Log kalloc statistics */
#define palerain_option_kern_upl_stat       420 /* Log UPL statistics */
#define palerain_option_kern_io_stat        421 /* Log I/O statistics */

/* === Dynamic Analysis & Instrumentation (15) === */
#define palerain_option_dtrace_enable       422 /* Enable DTrace probes */
#define palerain_option_ktrace_enable       423 /* Enable ktrace */
#define palerain_option_ftrace_enable       424 /* Enable function tracing */
#define palerain_option_perf_counters       425 /* Enable hardware perf counters */
#define palerain_option_branch_trace        426 /* Branch trace buffer */
#define palerain_option_call_graph          427 /* Build call graph */
#define palerain_option_hot_path_detect     428 /* Detect hot code paths */
#define palerain_option_mem_leak_detect     429 /* Detect memory leaks */
#define palerain_option_use_after_free      430 /* Detect use-after-free */
#define palerain_option_double_free_detect  431 /* Detect double free */
#define palerain_option_buffer_overflow     432 /* Detect buffer overflows */
#define palerain_option_race_detect         433 /* Detect data races */
#define palerain_option_deadlock_detect     434 /* Detect deadlocks */
#define palerain_option_stack_canary_check  435 /* Check stack canaries */
#define palerain_option_shadow_stack        436 /* Enable shadow stack */

/* === Fuzzing & Testing Infrastructure (10) === */
#define palerain_option_fuzz_kernel         437 /* Fuzz kernel syscalls */
#define palerain_option_fuzz_driver         438 /* Fuzz driver interfaces */
#define palerain_option_fuzz_filesystem     439 /* Fuzz filesystem operations */
#define palerain_option_fuzz_network        440 /* Fuzz network stack */
#define palerain_option_fuzz_usb_stack      441 /* Fuzz USB stack */
#define palerain_option_fuzz_ipc            442 /* Fuzz IPC mechanisms */
#define palerain_option_fuzz_mach           443 /* Fuzz Mach interfaces */
#define palerain_option_property_test       444 /* Property-based testing */
#define palerain_option_model_check         445 /* Model checking */
#define palerain_option_symbolic_exec       446 /* Symbolic execution */

/* === Reverse Engineering & Analysis (10) === */
#define palerain_option_re_disassemble      447 /* Disassemble code regions */
#define palerain_option_re_cfg_build        448 /* Build control flow graph */
#define palerain_option_re_data_flow        449 /* Data flow analysis */
#define palerain_option_re_type_recovery    450 /* Type recovery */
#define palerain_option_re_struct_recovery  451 /* Structure recovery */
#define palerain_option_re_string_analysis  452 /* String analysis */
#define palerain_option_re_xref_find        453 /* Cross-reference finding */
#define palerain_option_re_pattern_match    454 /* Pattern matching */
#define palerain_option_re_signature_gen    455 /* Generate signatures */
#define palerain_option_re_diff_binary      456 /* Binary diffing */

/* === Exploit Development (10) === */
#define palerain_option_exploit_dev_mode    457 /* Exploit development mode */
#define palerain_option_exploit_payload_gen 458 /* Generate exploit payloads */
#define palerain_option_exploit_rop_build   459 /* Build ROP chains */
#define palerain_option_exploit_jop_build   460 /* Build JOP chains */
#define palerain_option_exploit_srop_build  461 /* Build SROP chains */
#define palerain_option_exploit_pac_bypass  462 /* PAC bypass development */
#define palerain_option_exploit_pp_bypass   463 /* Pointer protection bypass */
#define palerain_option_exploit_kaslr_leak  464 /* KASLR leak development */
#define palerain_option_exploit_heap_feng   465 /* Heap feng shui tools */
#define palerain_option_exploit_race_win    466 /* Race condition winning */

/* === Hardware Bring-up & Validation (10) === */
#define palerain_option_hw_bringup          467 /* Hardware bring-up mode */
#define palerain_option_hw_memory_test      468 /* Memory testing */
#define palerain_option_hw_cache_test       469 /* Cache testing */
#define palerain_option_hw_interconnect     470 /* Interconnect testing */
#define palerain_option_hw_timer_test       471 /* Timer testing */
#define palerain_option_hw_interrupt_test   472 /* Interrupt testing */
#define palerain_option_hw_dma_test         473 /* DMA testing */
#define palerain_option_hw_pmu_test         474 /* PMU testing */
#define palerain_option_hw_thermal_test     475 /* Thermal testing */
#define palerain_option_hw_power_test       476 /* Power testing */

/* === Firmware & Bootloader Development (10) === */
#define palerain_option_fw_dev_mode         477 /* Firmware development mode */
#define palerain_option_fw_patch_test       478 /* Test firmware patches */
#define palerain_option_fw_verify_chain     479 /* Verify boot chain */
#define palerain_option_fw_key_test         480 /* Test key operations */
#define palerain_option_fw_rollback_test    481 /* Test rollback */
#define palerain_option_fw_dfu_test         482 /* Test DFU mode */
#define palerain_option_fw_recovery_test    483 /* Test recovery mode */
#define palerain_option_fw_ramdisk_test     484 /* Test ramdisk */
#define palerain_option_fw_overlay_test     485 /* Test overlay */
#define palerain_option_fw_integrity        486 /* Firmware integrity check */

/* === Performance & Optimization (10) === */
#define palerain_option_perf_baseline       487 /* Performance baseline */
#define palerain_option_perf_regression     488 /* Regression detection */
#define palerain_option_perf_optimize       489 /* Auto-optimization */
#define palerain_option_perf_tune_cpu       490 /* CPU tuning */
#define palerain_option_perf_tune_gpu       491 /* GPU tuning */
#define palerain_option_perf_tune_mem       492 /* Memory tuning */
#define palerain_option_perf_tune_io        493 /* I/O tuning */
#define palerain_option_perf_tune_net       494 /* Network tuning */
#define palerain_option_perf_energy         495 /* Energy optimization */
#define palerain_option_perf_thermal        496 /* Thermal optimization */

/* === Developer Workflow & Productivity (10) === */
#define palerain_option_dev_hot_reload      497 /* Hot reload for development */
#define palerain_option_dev_live_patch      498 /* Live patching */
#define palerain_option_dev_repl            499 /* REPL for kernel/userspace */
#define palerain_option_dev_snippets        500 /* Code snippets library */
#define palerain_option_dev_templates       501 /* Project templates */
#define palerain_option_dev_ci_integration  502 /* CI integration */
#define palerain_option_dev_git_hooks       503 /* Git hooks */
#define palerain_option_dev_docker          504 /* Docker support */
#define palerain_option_dev_vscode          505 /* VS Code integration */
#define palerain_option_dev_clangd          506 /* clangd LSP integration */

#define palerain_option_case_log_file  400

#define PALEINFO_MAGIC 'PLSH'

#pragma clang diagnostic push
#pragma clang diagnostic warning "-Wpadded"

/* paleinfo version 2, appended to ramdisk */
struct paleinfo {
    uint32_t magic; /* 'PLSH' */
    uint32_t version; /* 2 */
    uint64_t kbase; /* kernel base */
    uint64_t kslide; /* kernel slide */
    palerain_option_t flags; /* unified palera1n flags */
    char rootdev[0x10]; /* ex. disk0s1s8 */
};

#pragma clang diagnostic pop

#endif
