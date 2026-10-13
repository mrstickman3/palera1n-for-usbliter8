#include <simulator.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include <palerain.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <sys/stat.h>
#ifdef TUI
#include <tui.h>
#endif

/* External reference */
extern palerain_option_t palerain_flags;

palerain_option_t* palerain_flags_p = &palerain_flags;
char* gOverrideLibcheckra1nHelper = NULL;
/* dry_run removed - use palerain_option_dry_run flag instead */

static struct option longopts[] = {
	{"setup-partial-fakefs", no_argument, NULL, 'B'},
	{"setup-fakefs", no_argument, NULL, 'c'},
	{"clean-fakefs", no_argument, NULL, 'C'},
	{"dfuhelper", no_argument, NULL, 'D'},
	{"help", no_argument, NULL, 'h'},
	{"pongo-shell", no_argument, NULL, 'p'},
	{"pongo-full", no_argument, NULL, 'P'},
	{"debug-logging", no_argument, NULL, 'v'},
	{"verbose-boot", no_argument, NULL, 'V'},
	{"boot-args", required_argument, NULL, 'e'},
	{"fakefs", no_argument, NULL, 'f'},
	{"rootless", no_argument, NULL, 'l'},
	{"demote", no_argument, NULL, 'd'},
	{"force-revert", no_argument, NULL, palerain_option_case_force_revert},
	{"no-colors", no_argument, NULL, 'S'},
	{"safe-mode", no_argument, NULL, 's'},
	{"telnetd", no_argument, NULL, 'T'},
	{"simulator", required_argument, 0, 'm'},
	{"dry-run", no_argument, 0, 'y'},
	{"simulator-fast", required_argument, 0, 'M'},
	{"log-file", required_argument, NULL, palerain_option_case_log_file},
	{"config-file", required_argument, NULL, palerain_option_case_config_file},

	/* === Device Management Features === */
	{"device-list", no_argument, NULL, palerain_option_case_device_list},
	{"device-filter", required_argument, NULL, palerain_option_case_device_filter},
	{"device-group", no_argument, NULL, palerain_option_case_device_group},
	{"device-export", required_argument, NULL, palerain_option_case_device_export},
	{"device-autodetect", no_argument, NULL, palerain_option_case_device_autodetect},
	{"device-rename", required_argument, NULL, palerain_option_case_device_rename},
	{"device-note", required_argument, NULL, palerain_option_case_device_notes},
	{"device-favorite", no_argument, NULL, palerain_option_case_device_favorite},

	/* === Safety & Recovery Features === */
	{"backup-restore", required_argument, NULL, palerain_option_case_backup_restore},
	{"recovery-auto", no_argument, NULL, palerain_option_case_recovery_auto},
	{"dfu-auto", no_argument, NULL, palerain_option_case_dfu_auto},
	{"safe-boot", no_argument, NULL, palerain_option_case_safe_boot},
	{"bootloop-detect", no_argument, NULL, palerain_option_case_bootloop_detect},
	{"auto-recover", no_argument, NULL, palerain_option_case_auto_recover},
	{"data-protect", no_argument, NULL, palerain_option_case_data_protect},
	{"emergency-exit", no_argument, NULL, palerain_option_case_emergency_exit},

	/* === Exploit & Boot Features === */
	{"multi-exploit", required_argument, NULL, palerain_option_case_multi_exploit},
	{"exploit-chain", required_argument, NULL, palerain_option_case_exploit_chain},
	{"custom-payload", required_argument, NULL, palerain_option_case_custom_payload},
	{"boot-presets", required_argument, NULL, palerain_option_case_boot_presets},
	{"kern-patches", required_argument, NULL, palerain_option_case_kern_patches},
	{"ramdisk-cust", required_argument, NULL, palerain_option_case_ramdisk_cust},
	{"overlay-mgmt", required_argument, NULL, palerain_option_case_overlay_mgmt},
	{"verbose-boot-log", required_argument, NULL, palerain_option_case_verbose_boot_log},

	/* === User Experience Features === */
	{"interactive-wizard", no_argument, NULL, palerain_option_case_wizard},
	{"progress-anim", no_argument, NULL, palerain_option_case_progress_anim},
	{"theme", required_argument, NULL, palerain_option_case_theme},
	{"sound-notify", required_argument, NULL, palerain_option_case_sound_notify},
	{"haptic-feedback", required_argument, NULL, palerain_option_case_haptic_feedback},
	{"language", required_argument, NULL, palerain_option_case_language},
	{"accessibility", no_argument, NULL, palerain_option_case_accessibility},
	{"quick-actions", no_argument, NULL, palerain_option_case_quick_actions},

	/* === Developer & Debug Features === */
	{"debug-console", required_argument, NULL, palerain_option_case_debug_console},
	{"mem-inspect", required_argument, NULL, palerain_option_case_mem_inspect},
	{"reg-viewer", no_argument, NULL, palerain_option_case_reg_viewer},
	{"syscall-trace", no_argument, NULL, palerain_option_case_syscall_trace},
	{"kern-debug", no_argument, NULL, palerain_option_case_kern_debug},
	{"perf-profile", no_argument, NULL, palerain_option_case_perf_profile},
	{"crash-report", required_argument, NULL, palerain_option_case_crash_report},
	{"log-analyze", required_argument, NULL, palerain_option_case_log_analyze},

	/* === Automation & Scripting Features === */
	{"batch", required_argument, NULL, palerain_option_case_batch},
	{"schedule", required_argument, NULL, palerain_option_case_schedule},
	{"script", required_argument, NULL, palerain_option_case_script},
	{"plugin", required_argument, NULL, palerain_option_case_plugin},
	{"api-server", required_argument, NULL, palerain_option_case_api_server},
	{"webhook", required_argument, NULL, palerain_option_case_webhook},
	{"ci-cd", required_argument, NULL, palerain_option_case_ci_cd},
	{"config-template", required_argument, NULL, palerain_option_case_config_template},

	/* === Maintenance & Utility Features === */
	{"auto-update", no_argument, NULL, palerain_option_case_auto_update},
	{"self-diagnostics", no_argument, NULL, palerain_option_case_health_check},
	{"cache-manage", required_argument, NULL, palerain_option_case_cache_clean},
	{"log-rotate", required_argument, NULL, palerain_option_case_log_rotate},
	{"disk-cleanup", no_argument, NULL, palerain_option_case_disk_cleanup},
	{"health-monitor", required_argument, NULL, palerain_option_case_health_monitor},
	{"version-check", no_argument, NULL, palerain_option_case_version_check},
	{"dep-manager", no_argument, NULL, palerain_option_case_dep_manager},

	/* === Bonus Features === */
	{"benchmark", no_argument, NULL, palerain_option_case_benchmark},
	{"persist-check", no_argument, NULL, palerain_option_case_persist_check},

	/* === Additional Features === */
	{"device-info-detailed", no_argument, NULL, palerain_option_case_device_info_detailed},
	{"device-tree", no_argument, NULL, palerain_option_case_device_tree},
	{"kernel-dump", required_argument, NULL, palerain_option_case_kernel_dump},
	{"usb-sniffer", no_argument, NULL, palerain_option_case_usb_sniffer},
	{"boot-chain", no_argument, NULL, palerain_option_case_boot_chain},
	{"firmware-keys", required_argument, NULL, palerain_option_case_firmware_keys},
	{"pongo-shell", no_argument, NULL, palerain_option_case_pongo_shell},
	{"recovery-test", required_argument, NULL, palerain_option_case_recovery_test},

	/* === Ultra Verbose & Logging Features (10) === */
	{"ultra-verbose", no_argument, NULL, palerain_option_case_ultra_verbose},
	{"trace-usb", no_argument, NULL, palerain_option_case_trace_usb},
	{"trace-memory", no_argument, NULL, palerain_option_case_trace_memory},
	{"trace-exploit", no_argument, NULL, palerain_option_case_trace_exploit},
	{"trace-pongo", no_argument, NULL, palerain_option_case_trace_pongo},
	{"trace-kpf", no_argument, NULL, palerain_option_case_trace_kpf},
	{"trace-ramdisk", no_argument, NULL, palerain_option_case_trace_ramdisk},
	{"trace-overlay", no_argument, NULL, palerain_option_case_trace_overlay},
	{"trace-bootx", no_argument, NULL, palerain_option_case_trace_bootx},
	{"log-all-stages", no_argument, NULL, palerain_option_case_log_all_stages},

	/* === PongoOS Stage Diagnostics (10) === */
	{"pongo-verbose", no_argument, NULL, palerain_option_case_pongo_verbose},
	{"pongo-cmd-log", no_argument, NULL, palerain_option_case_pongo_cmd_log},
	{"pongo-response-log", no_argument, NULL, palerain_option_case_pongo_response_log},
	{"pongo-mem-map", no_argument, NULL, palerain_option_case_pongo_mem_map},
	{"pongo-reg-dump", no_argument, NULL, palerain_option_case_pongo_reg_dump},
	{"pongo-usb-log", no_argument, NULL, palerain_option_case_pongo_usb_log},
	{"pongo-exception-log", no_argument, NULL, palerain_option_case_pongo_exception_log},
	{"pongo-task-list", no_argument, NULL, palerain_option_case_pongo_task_list},
	{"pongo-heap-dump", no_argument, NULL, palerain_option_case_pongo_heap_dump},
	{"pongo-symbols", no_argument, NULL, palerain_option_case_pongo_symbols},

	/* === Exploit Stage Diagnostics (10) === */
	{"exploit-verbose", no_argument, NULL, palerain_option_case_exploit_verbose},
	{"exploit-step-trace", no_argument, NULL, palerain_option_case_exploit_step_trace},
	{"exploit-mem-trace", no_argument, NULL, palerain_option_case_exploit_mem_trace},
	{"exploit-reg-trace", no_argument, NULL, palerain_option_case_exploit_reg_trace},
	{"exploit-timing", no_argument, NULL, palerain_option_case_exploit_timing},
	{"exploit-retry-log", no_argument, NULL, palerain_option_case_exploit_retry_log},
	{"exploit-payload-log", no_argument, NULL, palerain_option_case_exploit_payload_log},
	{"exploit-state-dump", no_argument, NULL, palerain_option_case_exploit_state_dump},
	{"exploit-side-effects", no_argument, NULL, palerain_option_case_exploit_side_effects},
	{"exploit-validation", no_argument, NULL, palerain_option_case_exploit_validation},

	/* === Kernel & Memory Diagnostics (10) === */
	{"kernel-verbose", no_argument, NULL, palerain_option_case_kernel_verbose},
	{"kernel-mem-map", no_argument, NULL, palerain_option_case_kernel_mem_map},
	{"kernel-slide-log", no_argument, NULL, palerain_option_case_kernel_slide_log},
	{"kernel-patch-log", no_argument, NULL, palerain_option_case_kernel_patch_log},
	{"kernel-symbols", no_argument, NULL, palerain_option_case_kernel_symbols},
	{"kernel-stack-trace", no_argument, NULL, palerain_option_case_kernel_stack_trace},
	{"kernel-page-track", no_argument, NULL, palerain_option_case_kernel_page_track},
	{"kernel-vm-log", no_argument, NULL, palerain_option_case_kernel_vm_log},
	{"kernel-kalloc-log", no_argument, NULL, palerain_option_case_kernel_kalloc_log},
	{"kernel-zone-log", no_argument, NULL, palerain_option_case_kernel_zone_log},

	/* === USB & Communication Diagnostics (10) === */
	{"usb-verbose", no_argument, NULL, palerain_option_case_usb_verbose},
	{"usb-control-trace", no_argument, NULL, palerain_option_case_usb_control_trace},
	{"usb-bulk-trace", no_argument, NULL, palerain_option_case_usb_bulk_trace},
	{"usb-iso-trace", no_argument, NULL, palerain_option_case_usb_iso_trace},
	{"usb-device-tree", no_argument, NULL, palerain_option_case_usb_device_tree},
	{"usb-endpoint-log", no_argument, NULL, palerain_option_case_usb_endpoint_log},
	{"usb-speed-log", no_argument, NULL, palerain_option_case_usb_speed_log},
	{"usb-power-log", no_argument, NULL, palerain_option_case_usb_power_log},
	{"usb-reset-trace", no_argument, NULL, palerain_option_case_usb_reset_trace},
	{"usb-hub-log", no_argument, NULL, palerain_option_case_usb_hub_log},

	/* === Device Tree & Firmware Analysis (10) === */
	{"dtree-full-dump", no_argument, NULL, palerain_option_case_dtree_full_dump},
	{"dtree-compare", no_argument, NULL, palerain_option_case_dtree_compare},
	{"dtree-property-log", no_argument, NULL, palerain_option_case_dtree_property_log},
	{"dtree-compatible", no_argument, NULL, palerain_option_case_dtree_compatible},
	{"dtree-reg-dump", no_argument, NULL, palerain_option_case_dtree_reg_dump},
	{"dtree-interrupts", no_argument, NULL, palerain_option_case_dtree_interrupts},
	{"dtree-clocks", no_argument, NULL, palerain_option_case_dtree_clocks},
	{"dtree-power", no_argument, NULL, palerain_option_case_dtree_power},
	{"dtree-gpio", no_argument, NULL, palerain_option_case_dtree_gpio},
	{"firmware-analyze", no_argument, NULL, palerain_option_case_firmware_analyze},

	/* === Security & Cryptography Features (10) === */
	{"aes-engine-trace", no_argument, NULL, palerain_option_case_aes_engine_trace},
	{"sep-comm-log", no_argument, NULL, palerain_option_case_sep_comm_log},
	{"sep-firmware-dump", no_argument, NULL, palerain_option_case_sep_firmware_dump},
	{"sep-key-trace", no_argument, NULL, palerain_option_case_sep_key_trace},
	{"gid-uid-trace", no_argument, NULL, palerain_option_case_gid_uid_trace},
	{"firmware-decrypt", no_argument, NULL, palerain_option_case_firmware_decrypt},
	{"img4-parse-log", no_argument, NULL, palerain_option_case_img4_parse_log},
	{"ap-ticket-log", no_argument, NULL, palerain_option_case_ap_ticket_log},
	{"sep-ticket-log", no_argument, NULL, palerain_option_case_sep_ticket_log},
	{"keybag-dump", no_argument, NULL, palerain_option_case_keybag_dump},

	/* === Advanced Jailbreak Features (10) === */
	{"rootless-verbose", no_argument, NULL, palerain_option_case_rootless_verbose},
	{"fakefs-verbose", no_argument, NULL, palerain_option_case_fakefs_verbose},
	{"bind-mount-log", no_argument, NULL, palerain_option_case_bind_mount_log},
	{"overlay-verbose", no_argument, NULL, palerain_option_case_overlay_verbose},
	{"sandbox-log", no_argument, NULL, palerain_option_case_sandbox_log},
	{"amfi-log", no_argument, NULL, palerain_option_case_amfi_log},
	{"codesign-log", no_argument, NULL, palerain_option_case_codesign_log},
	{"entitlement-log", no_argument, NULL, palerain_option_case_entitlement_log},
	{"sip-log", no_argument, NULL, palerain_option_case_sip_log},
	{"tfp0-log", no_argument, NULL, palerain_option_case_tfp0_log},

	/* === Automation & Testing Features (10) === */
	{"auto-retry", no_argument, NULL, palerain_option_case_auto_retry},
	{"max-retries", required_argument, NULL, palerain_option_case_max_retries},
	{"test-mode", no_argument, NULL, palerain_option_case_test_mode},
	{"stress-test", no_argument, NULL, palerain_option_case_stress_test},
	{"fuzz-usb", no_argument, NULL, palerain_option_case_fuzz_usb},
	{"fuzz-exploit", no_argument, NULL, palerain_option_case_fuzz_exploit},
	{"fuzz-pongo", no_argument, NULL, palerain_option_case_fuzz_pongo},
	{"ci-mode", no_argument, NULL, palerain_option_case_ci_mode},
	{"benchmark-all", no_argument, NULL, palerain_option_case_benchmark_all},
	{"perf-report", required_argument, NULL, palerain_option_case_perf_report},

	/* === Developer & Research Features (10) === */
	{"research-mode", no_argument, NULL, palerain_option_case_research_mode},
	{"debug-breakpoints", no_argument, NULL, palerain_option_case_debug_breakpoints},
	{"memory-watchpoints", no_argument, NULL, palerain_option_case_memory_watchpoints},
	{"instruction-trace", no_argument, NULL, palerain_option_case_instruction_trace},
	{"coverage-tracking", no_argument, NULL, palerain_option_case_coverage_tracking},
	{"custom-hooks", no_argument, NULL, palerain_option_case_custom_hooks},
	{"plugin-debug", no_argument, NULL, palerain_option_case_plugin_debug},
	{"api-debug", no_argument, NULL, palerain_option_case_api_debug},
	{"script-debug", no_argument, NULL, palerain_option_case_script_debug},
	{"experimental", no_argument, NULL, palerain_option_case_experimental},

	/* === Phase 2: Advanced Diagnostics & Analysis (100 features) === */

	/* === Hardware & Peripheral Diagnostics (10) === */
	{"gpio-trace", no_argument, NULL, palerain_option_case_gpio_trace},
	{"i2c-trace", no_argument, NULL, palerain_option_case_i2c_trace},
	{"spi-trace", no_argument, NULL, palerain_option_case_spi_trace},
	{"uart-trace", no_argument, NULL, palerain_option_case_uart_trace},
	{"pmic-trace", no_argument, NULL, palerain_option_case_pmic_trace},
	{"display-trace", no_argument, NULL, palerain_option_case_display_trace},
	{"touch-trace", no_argument, NULL, palerain_option_case_touch_trace},
	{"audio-trace", no_argument, NULL, palerain_option_case_audio_trace},
	{"camera-trace", no_argument, NULL, palerain_option_case_camera_trace},
	{"sensor-trace", no_argument, NULL, palerain_option_case_sensor_trace},

	/* === Power & Thermal Management (10) === */
	{"power-domains", no_argument, NULL, palerain_option_case_power_domains},
	{"thermal-zones", no_argument, NULL, palerain_option_case_thermal_zones},
	{"battery-log", no_argument, NULL, palerain_option_case_battery_log},
	{"charging-log", no_argument, NULL, palerain_option_case_charging_log},
	{"dvfs-log", no_argument, NULL, palerain_option_case_dvfs_log},
	{"idle-states", no_argument, NULL, palerain_option_case_idle_states},
	{"suspend-log", no_argument, NULL, palerain_option_case_suspend_log},
	{"wakeup-sources", no_argument, NULL, palerain_option_case_wakeup_sources},
	{"power-stats", no_argument, NULL, palerain_option_case_power_stats},
	{"energy-model", no_argument, NULL, palerain_option_case_energy_model},

	/* === Filesystem & Storage Diagnostics (10) === */
	{"apfs-verbose", no_argument, NULL, palerain_option_case_apfs_verbose},
	{"apfs-snapshots", no_argument, NULL, palerain_option_case_apfs_snapshots},
	{"apfs-clones", no_argument, NULL, palerain_option_case_apfs_clones},
	{"apfs-encryption", no_argument, NULL, palerain_option_case_apfs_encryption},
	{"nand-trace", no_argument, NULL, palerain_option_case_nand_trace},
	{"nvme-trace", no_argument, NULL, palerain_option_case_nvme_trace},
	{"storage-stats", no_argument, NULL, palerain_option_case_storage_stats},
	{"trim-log", no_argument, NULL, palerain_option_case_trim_log},
	{"wear-leveling", no_argument, NULL, palerain_option_case_wear_leveling},
	{"bad-block-log", no_argument, NULL, palerain_option_case_bad_block_log},

	/* === Network & Connectivity Diagnostics (10) === */
	{"wifi-trace", no_argument, NULL, palerain_option_case_wifi_trace},
	{"bluetooth-trace", no_argument, NULL, palerain_option_case_bluetooth_trace},
	{"cellular-trace", no_argument, NULL, palerain_option_case_cellular_trace},
	{"nfc-trace", no_argument, NULL, palerain_option_case_nfc_trace},
	{"tcpip-trace", no_argument, NULL, palerain_option_case_tcpip_trace},
	{"dns-trace", no_argument, NULL, palerain_option_case_dns_trace},
	{"tls-trace", no_argument, NULL, palerain_option_case_tls_trace},
	{"vpn-trace", no_argument, NULL, palerain_option_case_vpn_trace},
	{"airdrop-trace", no_argument, NULL, palerain_option_case_airdrop_trace},
	{"continuity-trace", no_argument, NULL, palerain_option_case_continuity_trace},

	/* === Graphics & Display Diagnostics (10) === */
	{"gpu-trace", no_argument, NULL, palerain_option_case_gpu_trace},
	{"metal-trace", no_argument, NULL, palerain_option_case_metal_trace},
	{"iosurface-trace", no_argument, NULL, palerain_option_case_iosurface_trace},
	{"cvms-trace", no_argument, NULL, palerain_option_case_cvms_trace},
	{"compositor-trace", no_argument, NULL, palerain_option_case_compositor_trace},
	{"display-link", no_argument, NULL, palerain_option_case_display_link},
	{"vsync-trace", no_argument, NULL, palerain_option_case_vsync_trace},
	{"layer-trace", no_argument, NULL, palerain_option_case_layer_trace},
	{"color-trace", no_argument, NULL, palerain_option_case_color_trace},
	{"hdr-trace", no_argument, NULL, palerain_option_case_hdr_trace},

	/* === Security & Enclave Diagnostics (10) === */
	{"sep-verbose", no_argument, NULL, palerain_option_case_sep_verbose},
	{"sep-mailbox", no_argument, NULL, palerain_option_case_sep_mailbox},
	{"sep-memory", no_argument, NULL, palerain_option_case_sep_memory},
	{"sep-firmware-log", no_argument, NULL, palerain_option_case_sep_firmware_log},
	{"tz-verbose", no_argument, NULL, palerain_option_case_tz_verbose},
	{"tz-memory", no_argument, NULL, palerain_option_case_tz_memory},
	{"tz-smc-trace", no_argument, NULL, palerain_option_case_tz_smc_trace},
	{"secure-boot-log", no_argument, NULL, palerain_option_case_secure_boot_log},
	{"keybag-trace", no_argument, NULL, palerain_option_case_keybag_trace},
	{"entanglement-log", no_argument, NULL, palerain_option_case_entanglement_log},

	/* === Memory Management Diagnostics (10) === */
	{"vm-map-dump", no_argument, NULL, palerain_option_case_vm_map_dump},
	{"vm-fault-trace", no_argument, NULL, palerain_option_case_vm_fault_trace},
	{"physmem-log", no_argument, NULL, palerain_option_case_physmem_log},
	{"page-reclaim", no_argument, NULL, palerain_option_case_page_reclaim},
	{"compression-log", no_argument, NULL, palerain_option_case_compression_log},
	{"swap-trace", no_argument, NULL, palerain_option_case_swap_trace},
	{"jemalloc-trace", no_argument, NULL, palerain_option_case_jemalloc_trace},
	{"zone-trace", no_argument, NULL, palerain_option_case_zone_trace},
	{"vm-pressure", no_argument, NULL, palerain_option_case_vm_pressure},
	{"memory-tags", no_argument, NULL, palerain_option_case_memory_tags},

	/* === Scheduler & Process Diagnostics (10) === */
	{"sched-trace", no_argument, NULL, palerain_option_case_sched_trace},
	{"thread-trace", no_argument, NULL, palerain_option_case_thread_trace},
	{"workqueue-trace", no_argument, NULL, palerain_option_case_workqueue_trace},
	{"timer-trace", no_argument, NULL, palerain_option_case_timer_trace},
	{"ipc-trace", no_argument, NULL, palerain_option_case_ipc_trace},
	{"mach-trace", no_argument, NULL, palerain_option_case_mach_trace},
	{"exception-trace", no_argument, NULL, palerain_option_case_exception_trace},
	{"signal-trace", no_argument, NULL, palerain_option_case_signal_trace},
	{"proc-trace", no_argument, NULL, palerain_option_case_proc_trace},
	{"task-trace", no_argument, NULL, palerain_option_case_task_trace},

	/* === IOKit & Driver Diagnostics (10) === */
	{"ioregistry-dump", no_argument, NULL, palerain_option_case_ioregistry_dump},
	{"iokit-trace", no_argument, NULL, palerain_option_case_iokit_trace},
	{"driver-match", no_argument, NULL, palerain_option_case_driver_match},
	{"property-trace", no_argument, NULL, palerain_option_case_property_trace},
	{"service-trace", no_argument, NULL, palerain_option_case_service_trace},
	{"notification-trace", no_argument, NULL, palerain_option_case_notification_trace},
	{"powerman-trace", no_argument, NULL, palerain_option_case_powerman_trace},
	{"thermal-trace", no_argument, NULL, palerain_option_case_thermal_trace},
	{"pci-trace", no_argument, NULL, palerain_option_case_pci_trace},
	{"thunderbolt-trace", no_argument, NULL, palerain_option_case_thunderbolt_trace},

	/* === Boot & Startup Diagnostics (10) === */
	{"iboot-verbose", no_argument, NULL, palerain_option_case_iboot_verbose},
	{"iboot-env", no_argument, NULL, palerain_option_case_iboot_env},
	{"iboot-commands", no_argument, NULL, palerain_option_case_iboot_commands},
	{"diags-log", no_argument, NULL, palerain_option_case_diags_log},
	{"recovery-log", no_argument, NULL, palerain_option_case_recovery_log},
	{"dfu-log", no_argument, NULL, palerain_option_case_dfu_log},
	{"ramdisk-log", no_argument, NULL, palerain_option_case_ramdisk_log},
	{"kernelcache-log", no_argument, NULL, palerain_option_case_kernelcache_log},
	{"prelink-log", no_argument, NULL, palerain_option_case_prelink_log},
	{"boot-progress", no_argument, NULL, palerain_option_case_boot_progress},

	/* === Phase 3: PongoOS Stage Diagnostics & Advanced Features (100 features) === */

	/* === PongoOS Stage Diagnostics (20) === */
	{"pongo-stage-verbose", no_argument, NULL, palerain_option_case_pongo_stage_verbose},
	{"pongo-stage-log", no_argument, NULL, palerain_option_case_pongo_stage_log},
	{"pongo-mem-log", no_argument, NULL, palerain_option_case_pongo_mem_log},
	{"pongo-heap-log", no_argument, NULL, palerain_option_case_pongo_heap_log},
	{"pongo-task-log", no_argument, NULL, palerain_option_case_pongo_task_log},
	{"pongo-irq-log", no_argument, NULL, palerain_option_case_pongo_irq_log},
	{"pongo-timer-log", no_argument, NULL, palerain_option_case_pongo_timer_log},
	{"pongo-uart-log", no_argument, NULL, palerain_option_case_pongo_uart_log},
	{"pongo-usb-event-log", no_argument, NULL, palerain_option_case_pongo_usb_event_log},
	{"pongo-wdt-log", no_argument, NULL, palerain_option_case_pongo_wdt_log},
	{"pongo-crash-log", no_argument, NULL, palerain_option_case_pongo_crash_log},
	{"pongo-syscall-log", no_argument, NULL, palerain_option_case_pongo_syscall_log},
	{"pongo-ipc-log", no_argument, NULL, palerain_option_case_pongo_ipc_log},
	{"pongo-sem-log", no_argument, NULL, palerain_option_case_pongo_sem_log},
	{"pongo-mutex-log", no_argument, NULL, palerain_option_case_pongo_mutex_log},
	{"pongo-cond-log", no_argument, NULL, palerain_option_case_pongo_cond_log},
	{"pongo-queue-log", no_argument, NULL, palerain_option_case_pongo_queue_log},
	{"pongo-pool-log", no_argument, NULL, palerain_option_case_pongo_pool_log},
	{"pongo-dma-log", no_argument, NULL, palerain_option_case_pongo_dma_log},
	{"pongo-pmu-log", no_argument, NULL, palerain_option_case_pongo_pmu_log},

	/* === Kernel Exploitation Diagnostics (10) === */
	{"exploit-diag-verbose", no_argument, NULL, palerain_option_case_exploit_diag_verbose},
	{"exploit-stage-log", no_argument, NULL, palerain_option_case_exploit_stage_log},
	{"exploit-heap-log", no_argument, NULL, palerain_option_case_exploit_heap_log},
	{"exploit-rop-log", no_argument, NULL, palerain_option_case_exploit_rop_log},
	{"exploit-kcall-log", no_argument, NULL, palerain_option_case_exploit_kcall_log},
	{"exploit-kread-log", no_argument, NULL, palerain_option_case_exploit_kread_log},
	{"exploit-kwrite-log", no_argument, NULL, palerain_option_case_exploit_kwrite_log},
	{"exploit-pac-log", no_argument, NULL, palerain_option_case_exploit_pac_log},
	{"exploit-pp-log", no_argument, NULL, palerain_option_case_exploit_pp_log},
	{"exploit-slide-log", no_argument, NULL, palerain_option_case_exploit_slide_log},

	/* === Checkra1n Integration Diagnostics (10) === */
	{"checkra1n-verbose", no_argument, NULL, palerain_option_case_checkra1n_verbose},
	{"checkra1n-stage-log", no_argument, NULL, palerain_option_case_checkra1n_stage_log},
	{"checkra1n-pongo-log", no_argument, NULL, palerain_option_case_checkra1n_pongo_log},
	{"checkra1n-boot-log", no_argument, NULL, palerain_option_case_checkra1n_boot_log},
	{"checkra1n-ramdisk-log", no_argument, NULL, palerain_option_case_checkra1n_ramdisk_log},
	{"checkra1n-kernel-log", no_argument, NULL, palerain_option_case_checkra1n_kernel_log},
	{"checkra1n-hook-log", no_argument, NULL, palerain_option_case_checkra1n_hook_log},
	{"checkra1n-shell-log", no_argument, NULL, palerain_option_case_checkra1n_shell_log},
	{"checkra1n-ssh-log", no_argument, NULL, palerain_option_case_checkra1n_ssh_log},
	{"checkra1n-cleanup-log", no_argument, NULL, palerain_option_case_checkra1n_cleanup_log},

	/* === Advanced USB Diagnostics (10) === */
	{"usb-diag-verbose", no_argument, NULL, palerain_option_case_usb_diag_verbose},
	{"usb-ep-diag-log", no_argument, NULL, palerain_option_case_usb_ep_diag_log},
	{"usb-transfer-log", no_argument, NULL, palerain_option_case_usb_transfer_log},
	{"usb-control-log", no_argument, NULL, palerain_option_case_usb_control_log},
	{"usb-bulk-log", no_argument, NULL, palerain_option_case_usb_bulk_log},
	{"usb-interrupt-log", no_argument, NULL, palerain_option_case_usb_interrupt_log},
	{"usb-isochronous-log", no_argument, NULL, palerain_option_case_usb_isochronous_log},
	{"usb-device-log", no_argument, NULL, palerain_option_case_usb_device_log},
	{"usb-interface-log", no_argument, NULL, palerain_option_case_usb_interface_log},
	{"usb-config-log", no_argument, NULL, palerain_option_case_usb_config_log},

	/* === DFU Mode Diagnostics (10) === */
	{"dfu-verbose", no_argument, NULL, palerain_option_case_dfu_verbose},
	{"dfu-state-log", no_argument, NULL, palerain_option_case_dfu_state_log},
	{"dfu-command-log", no_argument, NULL, palerain_option_case_dfu_command_log},
	{"dfu-upload-log", no_argument, NULL, palerain_option_case_dfu_upload_log},
	{"dfu-download-log", no_argument, NULL, palerain_option_case_dfu_download_log},
	{"dfu-getstatus-log", no_argument, NULL, palerain_option_case_dfu_getstatus_log},
	{"dfu-clrstatus-log", no_argument, NULL, palerain_option_case_dfu_clrstatus_log},
	{"dfu-abort-log", no_argument, NULL, palerain_option_case_dfu_abort_log},
	{"dfu-detach-log", no_argument, NULL, palerain_option_case_dfu_detach_log},
	{"dfu-reset-log", no_argument, NULL, palerain_option_case_dfu_reset_log},

	/* === Recovery Mode Diagnostics (10) === */
	{"recovery-verbose", no_argument, NULL, palerain_option_case_recovery_verbose},
	{"recovery-state-log", no_argument, NULL, palerain_option_case_recovery_state_log},
	{"recovery-command-log", no_argument, NULL, palerain_option_case_recovery_command_log},
	{"recovery-upload-log", no_argument, NULL, palerain_option_case_recovery_upload_log},
	{"recovery-download-log", no_argument, NULL, palerain_option_case_recovery_download_log},
	{"recovery-nonce-log", no_argument, NULL, palerain_option_case_recovery_nonce_log},
	{"recovery-apnonce-log", no_argument, NULL, palerain_option_case_recovery_apnonce_log},
	{"recovery-sepnonce-log", no_argument, NULL, palerain_option_case_recovery_sepnonce_log},
	{"recovery-ticket-log", no_argument, NULL, palerain_option_case_recovery_ticket_log},
	{"recovery-reset-log", no_argument, NULL, palerain_option_case_recovery_reset_log},

	/* === Bootloader Diagnostics (10) === */
	{"bootloader-verbose", no_argument, NULL, palerain_option_case_bootloader_verbose},
	{"bootloader-stage-log", no_argument, NULL, palerain_option_case_bootloader_stage_log},
	{"bootloader-iboot-log", no_argument, NULL, palerain_option_case_bootloader_iboot_log},
	{"bootloader-ibec-log", no_argument, NULL, palerain_option_case_bootloader_ibec_log},
	{"bootloader-ibss-log", no_argument, NULL, palerain_option_case_bootloader_ibss_log},
	{"bootloader-lowlevel-log", no_argument, NULL, palerain_option_case_bootloader_lowlevel_log},
	{"bootloader-mem-log", no_argument, NULL, palerain_option_case_bootloader_mem_log},
	{"bootloader-reg-log", no_argument, NULL, palerain_option_case_bootloader_reg_log},
	{"bootloader-dev-log", no_argument, NULL, palerain_option_case_bootloader_dev_log},
	{"bootloader-cmd-log", no_argument, NULL, palerain_option_case_bootloader_cmd_log},

	/* === Filesystem Diagnostics (10) === */
	{"fs-verbose", no_argument, NULL, palerain_option_case_fs_verbose},
	{"fs-mount-log", no_argument, NULL, palerain_option_case_fs_mount_log},
	{"fs-unmount-log", no_argument, NULL, palerain_option_case_fs_unmount_log},
	{"fs-read-log", no_argument, NULL, palerain_option_case_fs_read_log},
	{"fs-write-log", no_argument, NULL, palerain_option_case_fs_write_log},
	{"fs-create-log", no_argument, NULL, palerain_option_case_fs_create_log},
	{"fs-delete-log", no_argument, NULL, palerain_option_case_fs_delete_log},
	{"fs-rename-log", no_argument, NULL, palerain_option_case_fs_rename_log},
	{"fs-stat-log", no_argument, NULL, palerain_option_case_fs_stat_log},
	{"fs-xattr-log", no_argument, NULL, palerain_option_case_fs_xattr_log},

	/* === Network Diagnostics (10) === */
	{"net-verbose", no_argument, NULL, palerain_option_case_net_verbose},
	{"net-packet-log", no_argument, NULL, palerain_option_case_net_packet_log},
	{"net-socket-log", no_argument, NULL, palerain_option_case_net_socket_log},
	{"net-route-log", no_argument, NULL, palerain_option_case_net_route_log},
	{"net-arp-log", no_argument, NULL, palerain_option_case_net_arp_log},
	{"net-ndp-log", no_argument, NULL, palerain_option_case_net_ndp_log},
	{"net-dhcp-log", no_argument, NULL, palerain_option_case_net_dhcp_log},
	{"net-dns-log", no_argument, NULL, palerain_option_case_net_dns_log},
	{"net-tls-log", no_argument, NULL, palerain_option_case_net_tls_log},
	{"net-http-log", no_argument, NULL, palerain_option_case_net_http_log},

	/* === Phase 4: Advanced Developer Tools & Debugging (100 features) === */

	/* === Kernel Development & Debugging (15) === */
	{"kern-debug-enter", no_argument, NULL, palerain_option_case_kern_debug_enter},
	{"kern-debug-wait", no_argument, NULL, palerain_option_case_kern_debug_wait},
	{"kern-break-early", no_argument, NULL, palerain_option_case_kern_break_early},
	{"kern-watchdog-disable", no_argument, NULL, palerain_option_case_kern_watchdog_disable},
	{"kern-panic-log", no_argument, NULL, palerain_option_case_kern_panic_log},
	{"kern-backtrace-all", no_argument, NULL, palerain_option_case_kern_backtrace_all},
	{"kern-lock-stat", no_argument, NULL, palerain_option_case_kern_lock_stat},
	{"kern-intr-stat", no_argument, NULL, palerain_option_case_kern_intr_stat},
	{"kern-syscall-stat", no_argument, NULL, palerain_option_case_kern_syscall_stat},
	{"kern-vm-stat", no_argument, NULL, palerain_option_case_kern_vm_stat},
	{"kern-task-stat", no_argument, NULL, palerain_option_case_kern_task_stat},
	{"kern-zone-stat", no_argument, NULL, palerain_option_case_kern_zone_stat},
	{"kern-kalloc-stat", no_argument, NULL, palerain_option_case_kern_kalloc_stat},
	{"kern-upl-stat", no_argument, NULL, palerain_option_case_kern_upl_stat},
	{"kern-io-stat", no_argument, NULL, palerain_option_case_kern_io_stat},

	/* === Dynamic Analysis & Instrumentation (15) === */
	{"dtrace-enable", no_argument, NULL, palerain_option_case_dtrace_enable},
	{"ktrace-enable", no_argument, NULL, palerain_option_case_ktrace_enable},
	{"ftrace-enable", no_argument, NULL, palerain_option_case_ftrace_enable},
	{"perf-counters", no_argument, NULL, palerain_option_case_perf_counters},
	{"branch-trace", no_argument, NULL, palerain_option_case_branch_trace},
	{"call-graph", no_argument, NULL, palerain_option_case_call_graph},
	{"hot-path-detect", no_argument, NULL, palerain_option_case_hot_path_detect},
	{"mem-leak-detect", no_argument, NULL, palerain_option_case_mem_leak_detect},
	{"use-after-free", no_argument, NULL, palerain_option_case_use_after_free},
	{"double-free-detect", no_argument, NULL, palerain_option_case_double_free_detect},
	{"buffer-overflow", no_argument, NULL, palerain_option_case_buffer_overflow},
	{"race-detect", no_argument, NULL, palerain_option_case_race_detect},
	{"deadlock-detect", no_argument, NULL, palerain_option_case_deadlock_detect},
	{"stack-canary-check", no_argument, NULL, palerain_option_case_stack_canary_check},
	{"shadow-stack", no_argument, NULL, palerain_option_case_shadow_stack},

	/* === Fuzzing & Testing Infrastructure (10) === */
	{"fuzz-kernel", no_argument, NULL, palerain_option_case_fuzz_kernel},
	{"fuzz-driver", no_argument, NULL, palerain_option_case_fuzz_driver},
	{"fuzz-filesystem", no_argument, NULL, palerain_option_case_fuzz_filesystem},
	{"fuzz-network", no_argument, NULL, palerain_option_case_fuzz_network},
	{"fuzz-usb-stack", no_argument, NULL, palerain_option_case_fuzz_usb_stack},
	{"fuzz-ipc", no_argument, NULL, palerain_option_case_fuzz_ipc},
	{"fuzz-mach", no_argument, NULL, palerain_option_case_fuzz_mach},
	{"property-test", no_argument, NULL, palerain_option_case_property_test},
	{"model-check", no_argument, NULL, palerain_option_case_model_check},
	{"symbolic-exec", no_argument, NULL, palerain_option_case_symbolic_exec},

	/* === Reverse Engineering & Analysis (10) === */
	{"re-disassemble", no_argument, NULL, palerain_option_case_re_disassemble},
	{"re-cfg-build", no_argument, NULL, palerain_option_case_re_cfg_build},
	{"re-data-flow", no_argument, NULL, palerain_option_case_re_data_flow},
	{"re-type-recovery", no_argument, NULL, palerain_option_case_re_type_recovery},
	{"re-struct-recovery", no_argument, NULL, palerain_option_case_re_struct_recovery},
	{"re-string-analysis", no_argument, NULL, palerain_option_case_re_string_analysis},
	{"re-xref-find", no_argument, NULL, palerain_option_case_re_xref_find},
	{"re-pattern-match", no_argument, NULL, palerain_option_case_re_pattern_match},
	{"re-signature-gen", no_argument, NULL, palerain_option_case_re_signature_gen},
	{"re-diff-binary", no_argument, NULL, palerain_option_case_re_diff_binary},

	/* === Exploit Development (10) === */
	{"exploit-dev-mode", no_argument, NULL, palerain_option_case_exploit_dev_mode},
	{"exploit-payload-gen", no_argument, NULL, palerain_option_case_exploit_payload_gen},
	{"exploit-rop-build", no_argument, NULL, palerain_option_case_exploit_rop_build},
	{"exploit-jop-build", no_argument, NULL, palerain_option_case_exploit_jop_build},
	{"exploit-srop-build", no_argument, NULL, palerain_option_case_exploit_srop_build},
	{"exploit-pac-bypass", no_argument, NULL, palerain_option_case_exploit_pac_bypass},
	{"exploit-pp-bypass", no_argument, NULL, palerain_option_case_exploit_pp_bypass},
	{"exploit-kaslr-leak", no_argument, NULL, palerain_option_case_exploit_kaslr_leak},
	{"exploit-heap-feng", no_argument, NULL, palerain_option_case_exploit_heap_feng},
	{"exploit-race-win", no_argument, NULL, palerain_option_case_exploit_race_win},

	/* === Hardware Bring-up & Validation (10) === */
	{"hw-bringup", no_argument, NULL, palerain_option_case_hw_bringup},
	{"hw-memory-test", no_argument, NULL, palerain_option_case_hw_memory_test},
	{"hw-cache-test", no_argument, NULL, palerain_option_case_hw_cache_test},
	{"hw-interconnect", no_argument, NULL, palerain_option_case_hw_interconnect},
	{"hw-timer-test", no_argument, NULL, palerain_option_case_hw_timer_test},
	{"hw-interrupt-test", no_argument, NULL, palerain_option_case_hw_interrupt_test},
	{"hw-dma-test", no_argument, NULL, palerain_option_case_hw_dma_test},
	{"hw-pmu-test", no_argument, NULL, palerain_option_case_hw_pmu_test},
	{"hw-thermal-test", no_argument, NULL, palerain_option_case_hw_thermal_test},
	{"hw-power-test", no_argument, NULL, palerain_option_case_hw_power_test},

	/* === Firmware & Bootloader Development (10) === */
	{"fw-dev-mode", no_argument, NULL, palerain_option_case_fw_dev_mode},
	{"fw-patch-test", no_argument, NULL, palerain_option_case_fw_patch_test},
	{"fw-verify-chain", no_argument, NULL, palerain_option_case_fw_verify_chain},
	{"fw-key-test", no_argument, NULL, palerain_option_case_fw_key_test},
	{"fw-rollback-test", no_argument, NULL, palerain_option_case_fw_rollback_test},
	{"fw-dfu-test", no_argument, NULL, palerain_option_case_fw_dfu_test},
	{"fw-recovery-test", no_argument, NULL, palerain_option_case_fw_recovery_test},
	{"fw-ramdisk-test", no_argument, NULL, palerain_option_case_fw_ramdisk_test},
	{"fw-overlay-test", no_argument, NULL, palerain_option_case_fw_overlay_test},
	{"fw-integrity", no_argument, NULL, palerain_option_case_fw_integrity},

	/* === Performance & Optimization (10) === */
	{"perf-baseline", no_argument, NULL, palerain_option_case_perf_baseline},
	{"perf-regression", no_argument, NULL, palerain_option_case_perf_regression},
	{"perf-optimize", no_argument, NULL, palerain_option_case_perf_optimize},
	{"perf-tune-cpu", no_argument, NULL, palerain_option_case_perf_tune_cpu},
	{"perf-tune-gpu", no_argument, NULL, palerain_option_case_perf_tune_gpu},
	{"perf-tune-mem", no_argument, NULL, palerain_option_case_perf_tune_mem},
	{"perf-tune-io", no_argument, NULL, palerain_option_case_perf_tune_io},
	{"perf-tune-net", no_argument, NULL, palerain_option_case_perf_tune_net},
	{"perf-energy", no_argument, NULL, palerain_option_case_perf_energy},
	{"perf-thermal", no_argument, NULL, palerain_option_case_perf_thermal},

	/* === Developer Workflow & Productivity (10) === */
	{"dev-hot-reload", no_argument, NULL, palerain_option_case_dev_hot_reload},
	{"dev-live-patch", no_argument, NULL, palerain_option_case_dev_live_patch},
	{"dev-repl", no_argument, NULL, palerain_option_case_dev_repl},
	{"dev-snippets", no_argument, NULL, palerain_option_case_dev_snippets},
	{"dev-templates", no_argument, NULL, palerain_option_case_dev_templates},
	{"dev-ci-integration", no_argument, NULL, palerain_option_case_dev_ci_integration},
	{"dev-git-hooks", no_argument, NULL, palerain_option_case_dev_git_hooks},
	{"dev-docker", no_argument, NULL, palerain_option_case_dev_docker},
	{"dev-vscode", no_argument, NULL, palerain_option_case_dev_vscode},
	{"dev-clangd", no_argument, NULL, palerain_option_case_dev_clangd},

	{"version", no_argument, NULL, palerain_option_case_version},
	{"override-libcheckra1nhelper", required_argument, NULL, palerain_option_case_libcheckra1nhelper_path},
	{"override-pongo", required_argument, NULL, 'k'},
	{"override-overlay", required_argument, NULL, 'o'},
	{"override-ramdisk", required_argument, NULL, 'r'},
	{"override-kpf", required_argument, NULL, 'K'},
	{"override-checkra1n", required_argument, NULL, 'i'},
	{"reboot-device", no_argument, NULL, 'R'},
	{"exit-recovery", no_argument, NULL, 'n'},
	{"enter-recovery", no_argument, NULL, 'E'},
	{"device-info", no_argument, NULL, 'I'},
#ifdef DEV_BUILD
	{"test1", no_argument, NULL, '1'},
	{"test2", no_argument, NULL, '2'},
#endif
#ifdef TUI
	{"tui", no_argument, NULL, 't'},
#endif
	{"pwned-dfu", no_argument, NULL, palerain_option_case_pwned_dfu},
	{"cli", no_argument, NULL, palerain_option_case_cli},
	{NULL, 0, NULL, 0}
};

static int usage(int e, char* prog_name)
{
	fprintf(stderr,
	"Usage: %s [-"
	"DEhpvVldsSTLRnPI"
#ifdef DEV_BUILD
			"12"
#endif
#ifdef ROOTFUL
			"cCfB"
#endif
#ifdef TUI
			"t"
#endif
			"] [-e boot arguments] [-k Pongo image] [-o overlay file] "
			"[-r ramdisk file] [-K KPF file] [-i checkra1n file]\n"
			"Copyright (C) 2024, palera1n team, All Rights Reserved.\n\n"
			"iOS/iPadOS/tvOS 15-26, bridgeOS 5-10 arm64 jailbreaking tool\n\n"
			"\t--version\t\t\t\tPrint version\n"
			"\t--force-revert\t\t\t\tRemove jailbreak\n"
#ifdef DEV_BUILD
			"\t-1, --test1\t\t\t\tSet palerain_option_test1\n"
			"\t-2, --test2\t\t\t\tSet palerain_option_test2\n"
#endif
#ifdef ROOTFUL
			"\t-B, --setup-partial-fakefs\t\tSetup partial fakefs\n"
			"\t-c, --setup-fakefs\t\t\tSetup fakefs\n"
			"\t-C, --clean-fakefs\t\t\tClean fakefs\n"
#endif
			"\t-d, --demote\t\t\t\tDemote\n"
			"\t-D, --dfuhelper\t\t\t\tExit after entering DFU\n"
			"\t-e, --boot-args <boot arguments>\tXNU boot arguments\n"
			"\t-E, --enter-recovery\t\t\tEnter recovery mode\n"
#ifdef ROOTFUL
			"\t-f, --fakefs \t\t\t\tBoots fakefs\n"
#endif
			"\t-h, --help\t\t\t\tShow this help\n"
			"\t-i, --override-checkra1n <file>\t\tOverride checkra1n\n"
			"\t-k, --override-pongo <file>\t\tOverride Pongo image\n"
			"\t-K, --override-kpf <file>\t\tOverride kernel patchfinder\n"
#ifdef ROOTFUL
			"\t-l, --rootless\t\t\t\tBoots rootless. This is the default\n"
#endif
			"\t-L, --jbinit-log-to-file\t\tMake jbinit log to /cores/jbinit.log "
			"(can be read from sandbox while jailbroken)\n"
			"\t-n, --exit-recovery\t\t\tExit recovery mode\n"
			"\t-I, --device-info\t\t\tPrint info about the connected device\n"
			"\t-o, --override-overlay <file>\t\tOverride overlay\n"
			"\t-p, --pongo-shell\t\t\tBoots to PongoOS shell\n"
			"\t-P, --pongo-full\t\t\tBoots to a PongoOS shell with default "
			"images already uploaded\n"
			"\t-r, --override-ramdisk <file>\t\tOverride ramdisk\n"
			"\t-R, --reboot-device\t\t\tReboot connected device in normal mode\n"
			"\t-s, --safe-mode\t\t\t\tEnter safe mode\n"
			"\t-S, --no-colors\t\t\t\tDisable colors on the command line\n"
			"\t-T, --telnetd\t\t\t\tEnable TELNET daemon on port 46 (insecure)\n"
			"\t-v, --debug-logging\t\t\tEnable debug logging\n"
			"\t\tThis option can be repeated for extra verbosity.\n"
			"\t-V, --verbose-boot\t\t\tVerbose boot\n"
			"\t--log-file <path>\t\t\tLog to file (default: palera1n.log)\n"
			"\t--config-file <path>\t\t\tLoad configuration from file\n"
			/* === Device Management Features === */
			"\t--device-list\t\t\t\tList all connected devices\n"
			"\t--device-filter <type>\t\t\tFilter devices by type\n"
			"\t--device-group\t\t\t\tGroup devices by iOS version\n"
			"\t--device-export <path>\t\t\tExport device list to file\n"
			"\t--device-autodetect\t\t\tAuto-detect connected device\n"
			"\t--device-rename <name>\t\t\tRename device\n"
			"\t--device-note <note>\t\t\tAdd note to device\n"
			"\t--device-favorite\t\t\tMark device as favorite\n"
			/* === Safety & Recovery Features === */
			"\t--backup-restore <path>\t\t\tBackup/Restore device data\n"
			"\t--recovery-auto\t\t\t\tAutomate recovery mode entry\n"
			"\t--dfu-auto\t\t\t\tAutomate DFU mode entry\n"
			"\t--safe-boot\t\t\t\tBoot in safe mode\n"
			"\t--bootloop-detect\t\t\tDetect and recover from bootloops\n"
			"\t--auto-recover\t\t\t\tAuto recovery mode\n"
			"\t--data-protect\t\t\t\tProtect user data during operations\n"
			"\t--emergency-exit\t\t\tEmergency exit handler\n"
			/* === Exploit & Boot Features === */
			"\t--multi-exploit <type>\t\t\tMulti-exploit support\n"
			"\t--exploit-chain <id>\t\t\tExploit chaining\n"
			"\t--custom-payload <path>\t\t\tCustom payload support\n"
			"\t--boot-presets <id>\t\t\tBoot argument presets\n"
			"\t--kern-patches <path>\t\t\tKernel patch management\n"
			"\t--ramdisk-cust <path>\t\t\tRamdisk customization\n"
			"\t--overlay-mgmt <path>\t\t\tOverlay management\n"
			"\t--verbose-boot-log <level>\t\tVerbose boot logging\n"
			/* === User Experience Features === */
			"\t--interactive-wizard\t\t\tInteractive wizard\n"
			"\t--progress-anim\t\t\t\tProgress animations\n"
			"\t--theme <name>\t\t\t\tColored themes\n"
			"\t--sound-notify <id>\t\t\tSound notifications\n"
			"\t--haptic-feedback <pattern>\t\tHaptic feedback\n"
			"\t--language <code>\t\t\tMulti-language support\n"
			"\t--accessibility\t\t\t\tAccessibility mode\n"
			"\t--quick-actions\t\t\t\tQuick actions\n"
			/* === Developer & Debug Features === */
			"\t--debug-console <port>\t\t\tDebug console\n"
			"\t--mem-inspect <addr:size>\t\tMemory inspector\n"
			"\t--reg-viewer\t\t\t\tRegister viewer\n"
			"\t--syscall-trace\t\t\t\tSyscall tracer\n"
			"\t--kern-debug\t\t\t\tKernel debugger\n"
			"\t--perf-profile\t\t\t\tPerformance profiler\n"
			"\t--crash-report <path>\t\t\tCrash reporter\n"
			"\t--log-analyze <path>\t\t\tLog analyzer\n"
	);
	fprintf(stderr,
			/* === Automation & Scripting Features === */
			"\t--batch <config>\t\t\t\tBatch operations\n"
			"\t--schedule <cron:cmd>\t\t\tScheduled tasks\n"
			"\t--script <path>\t\t\t\tScript engine\n"
			"\t--plugin <path>\t\t\t\tPlugin system\n"
			"\t--api-server <port>\t\t\tAPI server\n"
			"\t--webhook <url:event>\t\t\tWebhook support\n"
			"\t--ci-cd <config>\t\t\t\tCI/CD integration\n"
			"\t--config-template <path>\t\t\tConfig templates\n"
			/* === Maintenance & Utility Features === */
			"\t--auto-update\t\t\t\tAuto-update check\n"
			"\t--self-diagnostics\t\t\tSelf-diagnostics\n"
			"\t--cache-manage <action>\t\t\tCache management\n"
			"\t--log-rotate <size:files>\t\tLog rotation\n"
			"\t--disk-cleanup\t\t\t\tDisk cleanup\n"
			"\t--health-monitor <interval>\t\tHealth monitoring\n"
			"\t--version-check\t\t\t\tVersion checker\n"
			"\t--dep-manager\t\t\t\tDependency manager\n"
			/* === Bonus Features === */
			"\t--benchmark\t\t\t\tDevice benchmarking\n"
			"\t--persist-check\t\t\t\tJailbreak persistence check\n"
			/* === Additional Features === */
			"\t--device-info-detailed\t\tDetailed device info\n"
			"\t--device-tree\t\t\t\tShow device tree\n"
			"\t--kernel-dump <path>\t\t\tKernel memory dump\n"
			"\t--usb-sniffer\t\t\t\tUSB traffic sniffer\n"
			"\t--boot-chain\t\t\t\tBoot chain analysis\n"
			"\t--firmware-keys <path>\t\t\tFirmware key extraction\n"
			"\t--pongo-shell\t\t\t\tInteractive PongoOS shell\n"
			"\t--recovery-test <iterations>\tRecovery mode stress test\n"

			/* === Ultra Verbose & Logging Features === */
			"\t--ultra-verbose\t\t\t\tUltra verbose output for all stages\n"
			"\t--trace-usb\t\t\t\tTrace all USB transactions\n"
			"\t--trace-memory\t\t\t\tTrace memory allocations/frees\n"
			"\t--trace-exploit\t\t\t\tTrace exploit execution step-by-step\n"
			"\t--trace-pongo\t\t\t\tTrace PongoOS command/response\n"
			"\t--trace-kpf\t\t\t\tTrace kernel patchfinder operations\n"
			"\t--trace-ramdisk\t\t\t\tTrace ramdisk upload/extract\n"
			"\t--trace-overlay\t\t\t\tTrace overlay operations\n"
			"\t--trace-bootx\t\t\t\tTrace bootx execution\n"
			"\t--log-all-stages\t\t\t\tLog every stage to separate files\n"

			/* === PongoOS Stage Diagnostics === */
			"\t--pongo-verbose\t\t\t\tVerbose PongoOS boot logging\n"
			"\t--pongo-cmd-log\t\t\t\tLog all PongoOS commands sent\n"
			"\t--pongo-response-log\t\t\tLog all PongoOS responses\n"
			"\t--pongo-mem-map\t\t\t\tDump PongoOS memory map\n"
			"\t--pongo-reg-dump\t\t\t\tDump PongoOS register state\n"
			"\t--pongo-usb-log\t\t\t\tLog PongoOS USB endpoint traffic\n"
			"\t--pongo-exception-log\t\t\tLog PongoOS exceptions/crashes\n"
			"\t--pongo-task-list\t\t\t\tList PongoOS tasks/threads\n"
			"\t--pongo-heap-dump\t\t\t\tDump PongoOS heap\n"
			"\t--pongo-symbols\t\t\t\tLoad/dump PongoOS symbol table\n"
		);
		fprintf(stderr,
			/* === Exploit Stage Diagnostics === */
			"\t--exploit-verbose\t\t\tVerbose exploit execution logging\n"
			"\t--exploit-step-trace\t\t\tTrace each exploit step\n"
			"\t--exploit-mem-trace\t\t\tTrace exploit memory operations\n"
			"\t--exploit-reg-trace\t\t\tTrace register changes during exploit\n"
			"\t--exploit-timing\t\t\t\tMeasure exploit stage timing\n"
			"\t--exploit-retry-log\t\t\tLog exploit retry attempts\n"
			"\t--exploit-payload-log\t\t\tLog exploit payload bytes\n"
			"\t--exploit-state-dump\t\t\tDump exploit state machine\n"
			"\t--exploit-side-effects\t\t\tDetect exploit side effects\n"
			"\t--exploit-validation\t\t\tValidate exploit success criteria\n"

			/* === Kernel & Memory Diagnostics === */
			"\t--kernel-verbose\t\t\tVerbose kernel boot logging\n"
			"\t--kernel-mem-map\t\t\tDump kernel memory map\n"
			"\t--kernel-slide-log\t\t\tLog kernel slide calculation\n"
			"\t--kernel-patch-log\t\t\tLog each kernel patch applied\n"
			"\t--kernel-symbols\t\t\tLoad kernel symbol table\n"
			"\t--kernel-stack-trace\t\t\tCapture kernel stack traces\n"
			"\t--kernel-page-track\t\t\tTrack kernel page allocations\n"
			"\t--kernel-vm-log\t\t\t\tLog VM operations\n"
			"\t--kernel-kalloc-log\t\t\tLog kalloc/kfree operations\n"
			"\t--kernel-zone-log\t\t\t\tLog zone allocations\n"

			/* === USB & Communication Diagnostics === */
			"\t--usb-verbose\t\t\t\tVerbose USB communication logging\n"
			"\t--usb-control-trace\t\t\tTrace USB control transfers\n"
			"\t--usb-bulk-trace\t\t\tTrace USB bulk transfers\n"
			"\t--usb-iso-trace\t\t\t\tTrace USB isochronous transfers\n"
			"\t--usb-device-tree\t\t\tLog USB device tree\n"
			"\t--usb-endpoint-log\t\t\tLog USB endpoint info\n"
			"\t--usb-speed-log\t\t\t\tLog USB speed negotiation\n"
			"\t--usb-power-log\t\t\t\tLog USB power events\n"
			"\t--usb-reset-trace\t\t\tTrace USB bus resets\n"
			"\t--usb-hub-log\t\t\t\tLog USB hub topology\n"

			/* === Device Tree & Firmware Analysis === */
			"\t--dtree-full-dump\t\t\tFull device tree dump\n"
			"\t--dtree-compare\t\t\t\tCompare device trees\n"
			"\t--dtree-property-log\t\t\tLog all device tree properties\n"
			"\t--dtree-compatible\t\t\tLog compatible strings\n"
			"\t--dtree-reg-dump\t\t\t\tDump device tree reg properties\n"
			"\t--dtree-interrupts\t\t\tLog interrupt mappings\n"
			"\t--dtree-clocks\t\t\t\tLog clock tree\n"
			"\t--dtree-power\t\t\t\tLog power domain tree\n"
			"\t--dtree-gpio\t\t\t\tLog GPIO configuration\n"
			"\t--firmware-analyze\t\t\tAnalyze firmware components\n"
		);
		fprintf(stderr,
			/* === Security & Cryptography Features === */
			"\t--aes-engine-trace\t\t\tTrace AES engine operations\n"
			"\t--sep-comm-log\t\t\t\tLog SEP communication\n"
			"\t--sep-firmware-dump\t\t\tDump SEP firmware\n"
			"\t--sep-key-trace\t\t\t\tTrace SEP key operations\n"
			"\t--gid-uid-trace\t\t\t\tTrace GID/UID key usage\n"
			"\t--firmware-decrypt\t\t\tDecrypt firmware components\n"
			"\t--img4-parse-log\t\t\tLog IMG4 parsing\n"
			"\t--ap-ticket-log\t\t\t\tLog APTicket validation\n"
			"\t--sep-ticket-log\t\t\t\tLog SEPTicket validation\n"
			"\t--keybag-dump\t\t\t\tDump keybag contents\n"

			/* === Advanced Jailbreak Features === */
			"\t--rootless-verbose\t\t\tVerbose rootless boot logging\n"
			"\t--fakefs-verbose\t\t\tVerbose fakefs operations\n"
			"\t--bind-mount-log\t\t\tLog bind mount operations\n"
			"\t--overlay-verbose\t\t\tVerbose overlay operations\n"
			"\t--sandbox-log\t\t\t\tLog sandbox operations\n"
			"\t--amfi-log\t\t\t\tLog AMFI operations\n"
			"\t--codesign-log\t\t\t\tLog code signing ops\n"
			"\t--entitlement-log\t\t\t\tLog entitlement checks\n"
			"\t--sip-log\t\t\t\tLog SIP bypass operations\n"
			"\t--tfp0-log\t\t\t\tLog tfp0 operations\n"

			/* === Automation & Testing Features === */
			"\t--auto-retry\t\t\t\tAuto-retry failed stages\n"
			"\t--max-retries <n>\t\t\tSet max retry count\n"
			"\t--test-mode\t\t\t\tTest mode (no device changes)\n"
			"\t--stress-test\t\t\t\tStress test all stages\n"
			"\t--fuzz-usb\t\t\t\tFuzz USB communication\n"
			"\t--fuzz-exploit\t\t\t\tFuzz exploit parameters\n"
			"\t--fuzz-pongo\t\t\t\tFuzz PongoOS commands\n"
			"\t--ci-mode\t\t\t\tCI/CD optimized mode\n"
			"\t--benchmark-all\t\t\t\tBenchmark all stages\n"
			"\t--perf-report <path>\t\t\tGenerate performance report\n"

			/* === Developer & Research Features === */
			"\t--research-mode\t\t\t\tResearch mode - extra checks\n"
			"\t--debug-breakpoints\t\t\tEnable debug breakpoints\n"
			"\t--memory-watchpoints\t\t\tMemory watchpoints\n"
			"\t--instruction-trace\t\t\tInstruction-level tracing\n"
			"\t--coverage-tracking\t\t\tCode coverage tracking\n"
			"\t--custom-hooks\t\t\t\tCustom hook support\n"
			"\t--plugin-debug\t\t\t\tPlugin system debug\n"
			"\t--api-debug\t\t\t\tAPI server debug\n"
			"\t--script-debug\t\t\t\tScript engine debug\n"
			"\t--experimental\t\t\t\tEnable experimental features\n"

			/* === Phase 2: Advanced Diagnostics & Analysis === */

			/* === Hardware & Peripheral Diagnostics === */
			"\t--gpio-trace\t\t\t\tTrace GPIO operations\n"
			"\t--i2c-trace\t\t\t\tTrace I2C bus transactions\n"
			"\t--spi-trace\t\t\t\tTrace SPI bus transactions\n"
			"\t--uart-trace\t\t\t\tTrace UART communication\n"
			"\t--pmic-trace\t\t\t\tTrace PMIC operations\n"
			"\t--display-trace\t\t\t\tTrace display controller\n"
			"\t--touch-trace\t\t\t\tTrace touchscreen controller\n"
			"\t--audio-trace\t\t\t\tTrace audio subsystem\n"
			"\t--camera-trace\t\t\t\tTrace camera subsystem\n"
			"\t--sensor-trace\t\t\t\tTrace sensor hub\n"
		);
		fprintf(stderr,
			/* === Power & Thermal Management === */
			"\t--power-domains\t\t\t\tDump power domain tree\n"
			"\t--thermal-zones\t\t\t\tLog thermal zone data\n"
			"\t--battery-log\t\t\t\tLog battery status\n"
			"\t--charging-log\t\t\t\tLog charging events\n"
			"\t--dvfs-log\t\t\t\tLog DVFS transitions\n"
			"\t--idle-states\t\t\t\tLog CPU idle states\n"
			"\t--suspend-log\t\t\t\tLog suspend/resume\n"
			"\t--wakeup-sources\t\t\t\tLog wakeup sources\n"
			"\t--power-stats\t\t\t\tCollect power statistics\n"
			"\t--energy-model\t\t\t\tEnergy model analysis\n"

			/* === Filesystem & Storage Diagnostics === */
			"\t--apfs-verbose\t\t\t\tVerbose APFS logging\n"
			"\t--apfs-snapshots\t\t\tList APFS snapshots\n"
			"\t--apfs-clones\t\t\t\tLog APFS clone operations\n"
			"\t--apfs-encryption\t\t\tTrace APFS encryption\n"
			"\t--nand-trace\t\t\t\tTrace NAND operations\n"
			"\t--nvme-trace\t\t\t\tTrace NVMe commands\n"
			"\t--storage-stats\t\t\t\tStorage I/O statistics\n"
			"\t--trim-log\t\t\t\tLog TRIM operations\n"
			"\t--wear-leveling\t\t\t\tLog wear leveling\n"
			"\t--bad-block-log\t\t\t\tLog bad block management\n"

			/* === Network & Connectivity Diagnostics === */
			"\t--wifi-trace\t\t\t\tTrace WiFi operations\n"
			"\t--bluetooth-trace\t\t\tTrace Bluetooth operations\n"
			"\t--cellular-trace\t\t\tTrace cellular modem\n"
			"\t--nfc-trace\t\t\t\tTrace NFC operations\n"
			"\t--tcpip-trace\t\t\t\tTrace TCP/IP stack\n"
			"\t--dns-trace\t\t\t\tTrace DNS resolution\n"
			"\t--tls-trace\t\t\t\tTrace TLS handshakes\n"
			"\t--vpn-trace\t\t\t\tTrace VPN operations\n"
			"\t--airdrop-trace\t\t\t\tTrace AirDrop\n"
			"\t--continuity-trace\t\t\tTrace Continuity\n"
		);
		fprintf(stderr,
			/* === Graphics & Display Diagnostics === */
			"\t--gpu-trace\t\t\t\tTrace GPU commands\n"
			"\t--metal-trace\t\t\t\tTrace Metal API calls\n"
			"\t--iosurface-trace\t\t\tTrace IOSurface operations\n"
			"\t--cvms-trace\t\t\t\tTrace CVMS server\n"
			"\t--compositor-trace\t\t\tTrace window compositor\n"
			"\t--display-link\t\t\t\tTrace display link\n"
			"\t--vsync-trace\t\t\t\tTrace VSync events\n"
			"\t--layer-trace\t\t\t\tTrace layer operations\n"
			"\t--color-trace\t\t\t\tTrace color management\n"
			"\t--hdr-trace\t\t\t\tTrace HDR operations\n"

			/* === Security & Enclave Diagnostics === */
			"\t--sep-verbose\t\t\t\tVerbose SEP logging\n"
			"\t--sep-mailbox\t\t\t\tTrace SEP mailbox\n"
			"\t--sep-memory\t\t\t\tDump SEP memory map\n"
			"\t--sep-firmware-log\t\t\tLog SEP firmware events\n"
			"\t--tz-verbose\t\t\t\tVerbose TrustZone logging\n"
			"\t--tz-memory\t\t\t\tDump TZ memory map\n"
			"\t--tz-smc-trace\t\t\tTrace SMC calls\n"
			"\t--secure-boot-log\t\t\tLog secure boot process\n"
			"\t--keybag-trace\t\t\tTrace keybag operations\n"
			"\t--entanglement-log\t\t\tLog entanglement events\n"

			/* === Memory Management Diagnostics === */
			"\t--vm-map-dump\t\t\t\tDump VM maps\n"
			"\t--vm-fault-trace\t\t\tTrace VM faults\n"
			"\t--physmem-log\t\t\t\tLog physical memory\n"
			"\t--page-reclaim\t\t\t\tLog page reclaim\n"
			"\t--compression-log\t\t\tLog memory compression\n"
			"\t--swap-trace\t\t\t\tTrace swap operations\n"
			"\t--jemalloc-trace\t\t\tTrace jemalloc ops\n"
			"\t--zone-trace\t\t\t\tTrace zone allocator\n"
			"\t--vm-pressure\t\t\t\tLog VM pressure events\n"
			"\t--memory-tags\t\t\t\tTrack memory tags\n"
		);
		fprintf(stderr,
			/* === Scheduler & Process Diagnostics === */
			"\t--sched-trace\t\t\t\tTrace scheduler decisions\n"
			"\t--thread-trace\t\t\t\tTrace thread operations\n"
			"\t--workqueue-trace\t\t\tTrace workqueue ops\n"
			"\t--timer-trace\t\t\t\tTrace timer operations\n"
			"\t--ipc-trace\t\t\t\tTrace IPC messages\n"
			"\t--mach-trace\t\t\t\tTrace Mach calls\n"
			"\t--exception-trace\t\t\tTrace exceptions\n"
			"\t--signal-trace\t\t\t\tTrace signal delivery\n"
			"\t--proc-trace\t\t\t\tTrace process lifecycle\n"
			"\t--task-trace\t\t\t\tTrace task operations\n"

			/* === IOKit & Driver Diagnostics === */
			"\t--ioregistry-dump\t\t\tDump IORegistry\n"
			"\t--iokit-trace\t\t\t\tTrace IOKit calls\n"
			"\t--driver-match\t\t\t\tLog driver matching\n"
			"\t--property-trace\t\t\tTrace property changes\n"
			"\t--service-trace\t\t\tTrace service events\n"
			"\t--notification-trace\t\t\tTrace notifications\n"
			"\t--powerman-trace\t\t\tTrace power management\n"
			"\t--thermal-trace\t\t\tTrace thermal management\n"
			"\t--pci-trace\t\t\t\tTrace PCIe operations\n"
			"\t--thunderbolt-trace\t\t\tTrace Thunderbolt\n"

			/* === Boot & Startup Diagnostics === */
			"\t--iboot-verbose\t\t\t\tVerbose iBoot logging\n"
			"\t--iboot-env\t\t\t\tDump iBoot environment\n"
			"\t--iboot-commands\t\t\tLog iBoot commands\n"
			"\t--diags-log\t\t\t\tLog diagnostics\n"
			"\t--recovery-log\t\t\t\tLog recovery mode\n"
			"\t--dfu-log\t\t\t\tLog DFU mode\n"
			"\t--ramdisk-log\t\t\t\tLog ramdisk boot\n"
			"\t--kernelcache-log\t\t\tLog kernelcache load\n"
			"\t--prelink-log\t\t\t\tLog prelink process\n"
			"\t--boot-progress\t\t\t\tLog boot progress\n"

			/* === Phase 3: PongoOS Stage Diagnostics & Advanced Features === */

			/* === PongoOS Stage Diagnostics (20) === */
			"\t--pongo-stage-verbose\t\tVerbose PongoOS stage logging\n"
			"\t--pongo-stage-log\t\t\tLog PongoOS stage transitions\n"
			"\t--pongo-mem-log\t\t\t\tLog PongoOS memory usage\n"
			"\t--pongo-heap-log\t\t\tLog PongoOS heap allocations\n"
			"\t--pongo-task-log\t\t\tLog PongoOS task scheduling\n"
			"\t--pongo-irq-log\t\t\t\tLog PongoOS IRQ handling\n"
			"\t--pongo-timer-log\t\t\tLog PongoOS timer events\n"
			"\t--pongo-uart-log\t\t\tLog PongoOS UART I/O\n"
			"\t--pongo-usb-event-log\t\tLog PongoOS USB events\n"
			"\t--pongo-wdt-log\t\t\t\tLog PongoOS watchdog\n"
			"\t--pongo-crash-log\t\t\tLog PongoOS crashes\n"
			"\t--pongo-syscall-log\t\t\tLog PongoOS syscalls\n"
			"\t--pongo-ipc-log\t\t\t\tLog PongoOS IPC\n"
			"\t--pongo-sem-log\t\t\t\tLog PongoOS semaphores\n"
			"\t--pongo-mutex-log\t\t\tLog PongoOS mutexes\n"
			"\t--pongo-cond-log\t\t\tLog PongoOS condition variables\n"
			"\t--pongo-queue-log\t\t\tLog PongoOS message queues\n"
			"\t--pongo-pool-log\t\t\tLog PongoOS memory pools\n"
			"\t--pongo-dma-log\t\t\t\tLog PongoOS DMA operations\n"
			"\t--pongo-pmu-log\t\t\t\tLog PongoOS PMU events\n"
		);
		fprintf(stderr,
			/* === Kernel Exploitation Diagnostics (10) === */
			"\t--exploit-diag-verbose\t\tVerbose exploit diagnostics logging\n"
			"\t--exploit-stage-log\t\t\tLog exploit stage transitions\n"
			"\t--exploit-heap-log\t\t\tLog exploit heap operations\n"
			"\t--exploit-rop-log\t\t\tLog ROP chain construction\n"
			"\t--exploit-kcall-log\t\t\tLog kcall operations\n"
			"\t--exploit-kread-log\t\t\tLog kernel reads\n"
			"\t--exploit-kwrite-log\t\t\tLog kernel writes\n"
			"\t--exploit-pac-log\t\t\tLog PAC bypass attempts\n"
			"\t--exploit-pp-log\t\t\tLog pointer protection bypass\n"
			"\t--exploit-slide-log\t\t\tLog kernel slide discovery\n"

			/* === Checkra1n Integration Diagnostics (10) === */
			"\t--checkra1n-verbose\t\t\tVerbose checkra1n logging\n"
			"\t--checkra1n-stage-log\t\tLog checkra1n stages\n"
			"\t--checkra1n-pongo-log\t\tLog checkra1n PongoOS handshake\n"
			"\t--checkra1n-boot-log\t\t\tLog checkra1n boot process\n"
			"\t--checkra1n-ramdisk-log\t\tLog checkra1n ramdisk\n"
			"\t--checkra1n-kernel-log\t\tLog checkra1n kernel patching\n"
			"\t--checkra1n-hook-log\t\t\tLog checkra1n hook installation\n"
			"\t--checkra1n-shell-log\t\tLog checkra1n shell\n"
			"\t--checkra1n-ssh-log\t\t\tLog checkra1n SSH\n"
			"\t--checkra1n-cleanup-log\t\tLog checkra1n cleanup\n"

			/* === Advanced USB Diagnostics (10) === */
			"\t--usb-diag-verbose\t\t\tVerbose USB diagnostics logging\n"
			"\t--usb-ep-diag-log\t\t\tLog USB endpoint diagnostics\n"
			"\t--usb-transfer-log\t\t\tLog USB transfers\n"
			"\t--usb-control-log\t\t\tLog USB control transfers\n"
			"\t--usb-bulk-log\t\t\t\tLog USB bulk transfers\n"
			"\t--usb-interrupt-log\t\t\tLog USB interrupt transfers\n"
			"\t--usb-isochronous-log\t\tLog USB isochronous transfers\n"
			"\t--usb-device-log\t\t\tLog USB device events\n"
			"\t--usb-interface-log\t\t\tLog USB interface events\n"
			"\t--usb-config-log\t\t\tLog USB configuration events\n"
		);
		fprintf(stderr,
			/* === DFU Mode Diagnostics (10) === */
			"\t--dfu-verbose\t\t\t\tVerbose DFU logging\n"
			"\t--dfu-state-log\t\t\t\tLog DFU state transitions\n"
			"\t--dfu-command-log\t\t\tLog DFU commands\n"
			"\t--dfu-upload-log\t\t\tLog DFU uploads\n"
			"\t--dfu-download-log\t\t\tLog DFU downloads\n"
			"\t--dfu-getstatus-log\t\t\tLog DFU getstatus\n"
			"\t--dfu-clrstatus-log\t\t\tLog DFU clrstatus\n"
			"\t--dfu-abort-log\t\t\t\tLog DFU abort\n"
			"\t--dfu-detach-log\t\t\tLog DFU detach\n"
			"\t--dfu-reset-log\t\t\t\tLog DFU reset\n"

			/* === Recovery Mode Diagnostics (10) === */
			"\t--recovery-verbose\t\t\tVerbose recovery logging\n"
			"\t--recovery-state-log\t\tLog recovery state transitions\n"
			"\t--recovery-command-log\t\tLog recovery commands\n"
			"\t--recovery-upload-log\t\tLog recovery uploads\n"
			"\t--recovery-download-log\t\tLog recovery downloads\n"
			"\t--recovery-nonce-log\t\tLog recovery nonce handling\n"
			"\t--recovery-apnonce-log\t\tLog recovery apnonce\n"
			"\t--recovery-sepnonce-log\t\tLog recovery sepnonce\n"
			"\t--recovery-ticket-log\t\tLog recovery ticket\n"
			"\t--recovery-reset-log\t\t\tLog recovery reset\n"
		);
		fprintf(stderr,
			/* === Bootloader Diagnostics (10) === */
			"\t--bootloader-verbose\t\tVerbose bootloader logging\n"
			"\t--bootloader-stage-log\t\tLog bootloader stages\n"
			"\t--bootloader-iboot-log\t\tLog iBoot operations\n"
			"\t--bootloader-ibec-log\t\tLog iBEC operations\n"
			"\t--bootloader-ibss-log\t\tLog iBSS operations\n"
			"\t--bootloader-lowlevel-log\tLog low-level bootloader\n"
			"\t--bootloader-mem-log\t\tLog bootloader memory\n"
			"\t--bootloader-reg-log\t\t\tLog bootloader registers\n"
			"\t--bootloader-dev-log\t\t\tLog bootloader devices\n"
			"\t--bootloader-cmd-log\t\t\tLog bootloader commands\n"

			/* === Filesystem Diagnostics (10) === */
			"\t--fs-verbose\t\t\t\tVerbose filesystem logging\n"
			"\t--fs-mount-log\t\t\t\tLog filesystem mounts\n"
			"\t--fs-unmount-log\t\t\tLog filesystem unmounts\n"
			"\t--fs-read-log\t\t\t\tLog filesystem reads\n"
			"\t--fs-write-log\t\t\t\tLog filesystem writes\n"
			"\t--fs-create-log\t\t\t\tLog filesystem creates\n"
			"\t--fs-delete-log\t\t\t\tLog filesystem deletes\n"
			"\t--fs-rename-log\t\t\t\tLog filesystem renames\n"
			"\t--fs-stat-log\t\t\t\tLog filesystem stats\n"
			"\t--fs-xattr-log\t\t\t\tLog filesystem xattrs\n"
		);
		fprintf(stderr,
			/* === Network Diagnostics (10) === */
			"\t--net-verbose\t\t\t\tVerbose network logging\n"
			"\t--net-packet-log\t\t\tLog network packets\n"
			"\t--net-socket-log\t\t\tLog socket operations\n"
			"\t--net-route-log\t\t\t\tLog routing operations\n"
			"\t--net-arp-log\t\t\t\tLog ARP operations\n"
			"\t--net-ndp-log\t\t\t\tLog NDP operations\n"
			"\t--net-dhcp-log\t\t\t\tLog DHCP operations\n"
			"\t--net-dns-log\t\t\t\tLog DNS operations\n"
			"\t--net-tls-log\t\t\t\tLog TLS operations\n"
			"\t--net-http-log\t\t\t\tLog HTTP operations\n"
		);
		fprintf(stderr,
			/* === Phase 4: Advanced Developer Tools & Debugging (100 features) === */

			/* === Kernel Development & Debugging (15) === */
			"\t--kern-debug-enter\t\tEnter kernel debugger on boot\n"
			"\t--kern-debug-wait\t\tWait for kernel debugger attach\n"
			"\t--kern-break-early\t\tBreak at earliest kernel entry\n"
			"\t--kern-watchdog-disable\tDisable kernel watchdog\n"
			"\t--kern-panic-log\t\tLog kernel panics to file\n"
			"\t--kern-backtrace-all\t\tCapture all CPU backtraces\n"
			"\t--kern-lock-stat\t\tLog lock contention stats\n"
			"\t--kern-intr-stat\t\tLog interrupt statistics\n"
			"\t--kern-syscall-stat\t\tLog syscall statistics\n"
			"\t--kern-vm-stat\t\t\tLog VM statistics\n"
			"\t--kern-task-stat\t\tLog task/process statistics\n"
			"\t--kern-zone-stat\t\tLog zone allocator statistics\n"
			"\t--kern-kalloc-stat\t\tLog kalloc statistics\n"
			"\t--kern-upl-stat\t\t\tLog UPL statistics\n"
			"\t--kern-io-stat\t\t\tLog I/O statistics\n"

			/* === Dynamic Analysis & Instrumentation (15) === */
			"\t--dtrace-enable\t\t\tEnable DTrace probes\n"
			"\t--ktrace-enable\t\t\tEnable ktrace\n"
			"\t--ftrace-enable\t\t\tEnable function tracing\n"
			"\t--perf-counters\t\t\tEnable hardware perf counters\n"
			"\t--branch-trace\t\t\tBranch trace buffer\n"
			"\t--call-graph\t\t\tBuild call graph\n"
			"\t--hot-path-detect\t\tDetect hot code paths\n"
			"\t--mem-leak-detect\t\tDetect memory leaks\n"
			"\t--use-after-free\t\tDetect use-after-free\n"
			"\t--double-free-detect\t\tDetect double free\n"
			"\t--buffer-overflow\t\tDetect buffer overflows\n"
			"\t--race-detect\t\t\tDetect data races\n"
			"\t--deadlock-detect\t\tDetect deadlocks\n"
			"\t--stack-canary-check\t\tCheck stack canaries\n"
			"\t--shadow-stack\t\t\tEnable shadow stack\n"

			/* === Fuzzing & Testing Infrastructure (10) === */
			"\t--fuzz-kernel\t\t\tFuzz kernel syscalls\n"
			"\t--fuzz-driver\t\t\tFuzz driver interfaces\n"
			"\t--fuzz-filesystem\t\tFuzz filesystem operations\n"
			"\t--fuzz-network\t\t\tFuzz network stack\n"
			"\t--fuzz-usb-stack\t\tFuzz USB stack\n"
			"\t--fuzz-ipc\t\t\tFuzz IPC mechanisms\n"
			"\t--fuzz-mach\t\t\tFuzz Mach interfaces\n"
			"\t--property-test\t\t\tProperty-based testing\n"
			"\t--model-check\t\t\tModel checking\n"
			"\t--symbolic-exec\t\t\tSymbolic execution\n"
		);
		fprintf(stderr,
			/* === Reverse Engineering & Analysis (10) === */
			"\t--re-disassemble\t\tDisassemble code regions\n"
			"\t--re-cfg-build\t\t\tBuild control flow graph\n"
			"\t--re-data-flow\t\t\tData flow analysis\n"
			"\t--re-type-recovery\t\tType recovery\n"
			"\t--re-struct-recovery\t\tStructure recovery\n"
			"\t--re-string-analysis\t\tString analysis\n"
			"\t--re-xref-find\t\t\tCross-reference finding\n"
			"\t--re-pattern-match\t\tPattern matching\n"
			"\t--re-signature-gen\t\tGenerate signatures\n"
			"\t--re-diff-binary\t\tBinary diffing\n"

			/* === Exploit Development (10) === */
			"\t--exploit-dev-mode\t\tExploit development mode\n"
			"\t--exploit-payload-gen\t\tGenerate exploit payloads\n"
			"\t--exploit-rop-build\t\tBuild ROP chains\n"
			"\t--exploit-jop-build\t\tBuild JOP chains\n"
			"\t--exploit-srop-build\t\tBuild SROP chains\n"
			"\t--exploit-pac-bypass\t\tPAC bypass development\n"
			"\t--exploit-pp-bypass\t\tPointer protection bypass\n"
			"\t--exploit-kaslr-leak\t\tKASLR leak development\n"
			"\t--exploit-heap-feng\t\tHeap feng shui tools\n"
			"\t--exploit-race-win\t\tRace condition winning\n"

			/* === Hardware Bring-up & Validation (10) === */
			"\t--hw-bringup\t\t\tHardware bring-up mode\n"
			"\t--hw-memory-test\t\tMemory testing\n"
			"\t--hw-cache-test\t\t\tCache testing\n"
			"\t--hw-interconnect\t\tInterconnect testing\n"
			"\t--hw-timer-test\t\t\tTimer testing\n"
			"\t--hw-interrupt-test\t\tInterrupt testing\n"
			"\t--hw-dma-test\t\t\tDMA testing\n"
			"\t--hw-pmu-test\t\t\tPMU testing\n"
			"\t--hw-thermal-test\t\tThermal testing\n"
			"\t--hw-power-test\t\t\tPower testing\n"

			/* === Firmware & Bootloader Development (10) === */
			"\t--fw-dev-mode\t\t\tFirmware development mode\n"
			"\t--fw-patch-test\t\t\tTest firmware patches\n"
			"\t--fw-verify-chain\t\tVerify boot chain\n"
			"\t--fw-key-test\t\t\tTest key operations\n"
			"\t--fw-rollback-test\t\tTest rollback\n"
			"\t--fw-dfu-test\t\t\tTest DFU mode\n"
			"\t--fw-recovery-test\t\tTest recovery mode\n"
			"\t--fw-ramdisk-test\t\tTest ramdisk\n"
			"\t--fw-overlay-test\t\tTest overlay\n"
			"\t--fw-integrity\t\t\tFirmware integrity check\n"
		);
		fprintf(stderr,
			/* === Performance & Optimization (10) === */
			"\t--perf-baseline\t\t\tPerformance baseline\n"
			"\t--perf-regression\t\tRegression detection\n"
			"\t--perf-optimize\t\t\tAuto-optimization\n"
			"\t--perf-tune-cpu\t\t\tCPU tuning\n"
			"\t--perf-tune-gpu\t\t\tGPU tuning\n"
			"\t--perf-tune-mem\t\t\tMemory tuning\n"
			"\t--perf-tune-io\t\t\tI/O tuning\n"
			"\t--perf-tune-net\t\t\tNetwork tuning\n"
			"\t--perf-energy\t\t\tEnergy optimization\n"
			"\t--perf-thermal\t\t\tThermal optimization\n"

			/* === Developer Workflow & Productivity (10) === */
			"\t--dev-hot-reload\t\tHot reload for development\n"
			"\t--dev-live-patch\t\tLive patching\n"
			"\t--dev-repl\t\t\tREPL for kernel/userspace\n"
			"\t--dev-snippets\t\t\tCode snippets library\n"
			"\t--dev-templates\t\t\tProject templates\n"
			"\t--dev-ci-integration\t\tCI integration\n"
			"\t--dev-git-hooks\t\t\tGit hooks\n"
			"\t--dev-docker\t\t\tDocker support\n"
			"\t--dev-vscode\t\t\tVS Code integration\n"
			"\t--dev-clangd\t\t\tclangd LSP integration\n"
#ifdef TUI
			"\t-t, --tui\t\t\t\tForce interactive TUI\n"
			"\t--cli\t\t\t\t\tForce cli mode\n"
#endif
#if defined(__APPLE__)
			"\t--override-libcheckra1nhelper <file>\tCustom checkra1n hook dylib\n"
#endif
			"\nEnvironmental variables:\n"
				,
			prog_name);
	exit(e);
}

int optparse(int argc, char* argv[]) {
	int opt;
	int index;
	while ((opt = getopt_long(argc, argv,
        "DEhpvVlLdsSTtRnPIe:o:r:K:k:i:m:y:D"
#ifdef DEV_BUILD
	"12"
#endif
#ifdef ROOTFUL
	"fCcB"
#endif
	,longopts, NULL)) != -1)
	{
		switch (opt) {
#ifdef ROOTFUL
		case 'B':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_setup_partial_root);
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_setup_rootful);
			break;
		case 'c':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_setup_rootful);
			break;
		case 'C':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_clean_fakefs);
			break;
#endif
		case 'p':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_exit);
			break;
		case 'P':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_full);
			break;
		case 'D':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfuhelper_only);
			break;
		case 'h':
			usage(0, argv[0]);
			assert(0);
		case 'v':
			verbose++;
			break;
		case 'V':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_verbose_boot);
#ifdef TUI
			tui_options_verbose_boot = true;
#endif
			break;
		case 'e':
			if (strlen(optarg) > (sizeof(xargs_cmd) - 0x20)) {
                LOG(LOG_FATAL, "Boot arguments too long");
                return -1;
            }
			snprintf(xargs_cmd, sizeof(xargs_cmd), "xargs %s", optarg);
#ifdef TUI
			snprintf(tui_options_boot_args, sizeof(tui_options_boot_args), "%s", optarg);
#endif
			break;
#ifdef ROOTFUL
		case 'f':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_rootful);
			PALERAIN_OPTION_CLEAR(palerain_flags, palerain_option_rootless);
			break;
#endif
		case 'l':
			PALERAIN_OPTION_CLEAR(palerain_flags, palerain_option_rootful);
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_rootless);
			break;
		case 'L':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_jbinit_log_to_file);
			break;
		case 'd':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_demote);
			break;
		case 'E':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_enter_recovery);
			break;
		case 's':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_safemode);
#ifdef TUI
			tui_options_safe_mode = true;
#endif
			break;
		case 'T':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_telnetd);
			break;
		case 'k':
			if (!optarg) {
				LOG(LOG_FATAL, "Missing pongo file path");
				return -1;
			}
			if (access(optarg, F_OK) != 0) {
				LOG(LOG_FATAL, "Cannot access pongo file at %s: %d (%s)", optarg, errno, strerror(errno));
				return -1;
			}
			pongo_path = malloc(strlen(optarg) + 1);
			if (pongo_path == NULL) {
				LOG(LOG_FATAL, "memory allocation failed");
				return -1;
			}
			snprintf(pongo_path, strlen(optarg) + 1, "%s", optarg);
			break;
		case 'o':
			if (override_file(&override_overlay, overlay_to_upload, &binpack_dmg_len, optarg))
				return 1;
			break;
		case 'r':
			if (override_file(&override_ramdisk, ramdisk_to_upload, &ramdisk_dmg_lzma_len, optarg))
				return 1;
			break;
		case 'K':
			if (override_file(&override_kpf, kpf_to_upload, &checkra1n_kpf_pongo_lzma_len, optarg))
				return 1;
			struct mach_header_64* hdr = (struct mach_header_64*)override_kpf.ptr;
			if (hdr->magic != MH_MAGIC_64 && hdr->magic != MH_CIGAM_64) {
				LOG(LOG_FATAL, "Invalid kernel patchfinder: Not thin 64-bit Mach-O");
				return -1;
			} else if (hdr->filetype != MH_KEXT_BUNDLE) {
				LOG(LOG_FATAL, "Invalid kernel patchfinder: Not a kext bundle");
				return -1;
			} else if (hdr->cputype != CPU_TYPE_ARM64) {
				LOG(LOG_FATAL, "Invalid kernel patchfinder: CPU type is not arm64");
				return -1;
			}
			break;
		case 'i': {};
			struct stat st;
			if (stat(optarg, &st) != 0) {
				LOG(LOG_FATAL, "cannot stat external checkra1n file: %d (%s)", errno, strerror(errno));
				return -1;
			} else if (!(st.st_mode & S_IXUSR) && !(st.st_mode & S_IXGRP) && !(st.st_mode & S_IXOTH)) {
				LOG(LOG_FATAL, "%s is not executable", optarg);
				return -1;
			} else if (!S_ISREG(st.st_mode)) {
				LOG(LOG_FATAL, "%s is not a regular file", optarg);
				return -1;
			}
			if (st.st_size < (UCHAR_MAX+1)) {
				LOG(LOG_FATAL, "%s too small", optarg);
				return -1;
			}
			int checkra1n_fd = open(optarg, O_RDONLY);
			if (checkra1n_fd == -1) {
				LOG(LOG_FATAL, "Cannot open %s: %d (%s)", optarg, errno, strerror(errno));
				return -1;
			}
			void* addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, checkra1n_fd, 0);
			if (addr == MAP_FAILED) {
				LOG(LOG_ERROR, "Failed to map file %s: %d (%s)", optarg, errno, strerror(errno));
				return -1;
			}
			if (boyermoore_horspool_memmem(addr, st.st_size, (const unsigned char*)"[ra1npoc15-part] thanks to", strlen("[ra1npoc15-part] thanks to")) != NULL) 
				{
					PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkrain_is_clone);
					LOG(LOG_VERBOSE3, "%s is checkra1n clone", optarg);
				}
			else
			{
				PALERAIN_OPTION_CLEAR(palerain_flags, palerain_option_checkrain_is_clone);
				LOG(LOG_VERBOSE3, "%s is checkra1n", optarg);
			}
			munmap(addr, st.st_size);
			close(checkra1n_fd);
			ext_checkra1n = calloc(1, strlen(optarg) + 1);
			snprintf(ext_checkra1n, strlen(optarg) + 1, "%s", optarg);
			break;
		case 'M':
			if (!optarg) {
				LOG(LOG_FATAL, "Missing simulator device argument");
				return -1;
			}
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_simulator_pwned_dfu);
			simulator_fast = 1;
			simulator_init(strcmp(optarg,"a13")==0?SIM_DEVICE_A13:SIM_DEVICE_A12,SIM_MODE_PWNED_DFU);
			break;

		case 'y':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dry_run);
			break;

			PALERAIN_OPTION_SET(palerain_flags, palerain_option_diagnostics);
			break;

		case 'm':
			if (!optarg) {
				LOG(LOG_FATAL, "Missing simulator device argument");
				return -1;
			}
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_simulator_pwned_dfu);
			simulator_init(strcmp(optarg,"a13")==0?SIM_DEVICE_A13:strcmp(optarg,"a12")==0?SIM_DEVICE_A12:strcmp(optarg,"a11")==0?11:strcmp(optarg,"a10")==0?10:strcmp(optarg,"a9")==0?9:strcmp(optarg,"a8")==0?8:SIM_DEVICE_A12,SIM_MODE_PWNED_DFU);
			break;

		case 'R':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_reboot_device);
			break;
		case 'n':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exit_recovery);
			break;
		case 'I':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_info);
			break;
		case 'S':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_no_colors);
			break;
#ifdef TUI
		case 't':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_tui);
			break;
#endif
		case palerain_option_case_cli:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_cli);
			break;
		case palerain_option_case_pwned_dfu:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pwned_dfu);
			break;
#ifdef DEV_BUILD
		case '1':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_test1);
			break;
		case '2':
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_test2);
			break;
#endif
		case palerain_option_case_force_revert:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_force_revert);
#ifdef TUI
			tui_options_force_revert = true;
#endif
			break;
		case palerain_option_case_version:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_palerain_version);
			break;
		case palerain_option_case_libcheckra1nhelper_path:
			gOverrideLibcheckra1nHelper = calloc(1, strlen(optarg) + 1);
			if (!gOverrideLibcheckra1nHelper) {
				return -1;
			}
			snprintf(gOverrideLibcheckra1nHelper, strlen(optarg) + 1, "%s", optarg);
			break;
		case palerain_option_case_config_file:
			if (load_config_file(optarg) != 0) {
				LOG(LOG_WARNING, "Failed to load config file: %s", optarg);
			}
			break;

		/* === Device Management Features === */
		case palerain_option_case_device_list:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_list);
			break;
		case palerain_option_case_device_filter:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_filter);
			break;
		case palerain_option_case_device_group:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_group);
			break;
		case palerain_option_case_device_export:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_export);
			break;
		case palerain_option_case_device_autodetect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_autodetect);
			break;
		case palerain_option_case_device_rename:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_rename);
			break;
		case palerain_option_case_device_notes:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_notes);
			break;
		case palerain_option_case_device_favorite:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_favorite);
			break;

		/* === Safety & Recovery Features === */
		case palerain_option_case_backup_restore:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_backup_restore);
			break;
		case palerain_option_case_recovery_auto:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_auto);
			break;
		case palerain_option_case_dfu_auto:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_auto);
			break;
		case palerain_option_case_safe_boot:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_safe_boot);
			break;
		case palerain_option_case_bootloop_detect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloop_detect);
			break;
		case palerain_option_case_auto_recover:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_auto_recover);
			break;
		case palerain_option_case_data_protect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_data_protect);
			break;
		case palerain_option_case_emergency_exit:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_emergency_exit);
			break;

		/* === Exploit & Boot Features === */
		case palerain_option_case_multi_exploit:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_multi_exploit);
			break;
		case palerain_option_case_exploit_chain:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_chain);
			break;
		case palerain_option_case_custom_payload:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_custom_payload);
			break;
		case palerain_option_case_boot_presets:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_boot_presets);
			break;
		case palerain_option_case_kern_patches:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_patches);
			break;
		case palerain_option_case_ramdisk_cust:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ramdisk_cust);
			break;
		case palerain_option_case_overlay_mgmt:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_overlay_mgmt);
			break;
		case palerain_option_case_verbose_boot_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_verbose_boot_log);
			break;

		/* === User Experience Features === */
		case palerain_option_case_wizard:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_interactive_wizard);
			break;
		case palerain_option_case_progress_anim:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_progress_anim);
			break;
		case palerain_option_case_theme:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_colored_themes);
			break;
		case palerain_option_case_sound_notify:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sound_notify);
			break;
		case palerain_option_case_haptic_feedback:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_haptic_feedback);
			break;
		case palerain_option_case_language:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_multi_lang);
			break;
		case palerain_option_case_accessibility:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_accessibility);
			break;
		case palerain_option_case_quick_actions:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_quick_actions);
			break;

		/* === Developer & Debug Features === */
		case palerain_option_case_debug_console:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_debug_console);
			break;
		case palerain_option_case_mem_inspect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_mem_inspect);
			break;
		case palerain_option_case_reg_viewer:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_reg_viewer);
			break;
		case palerain_option_case_syscall_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_syscall_trace);
			break;
		case palerain_option_case_kern_debug:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_debug);
			break;
		case palerain_option_case_perf_profile:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_profile);
			break;
		case palerain_option_case_crash_report:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_crash_report);
			break;
		case palerain_option_case_log_analyze:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_log_analyze);
			break;

		/* === Automation & Scripting Features === */
		case palerain_option_case_batch:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_batch_ops);
			break;
		case palerain_option_case_schedule:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_scheduled_tasks);
			break;
		case palerain_option_case_script:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_script_engine);
			break;
		case palerain_option_case_plugin:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_plugin_system);
			break;
		case palerain_option_case_api_server:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_api_server);
			break;
		case palerain_option_case_webhook:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_webhook);
			break;
		case palerain_option_case_ci_cd:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ci_cd);
			break;
		case palerain_option_case_config_template:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_config_templates);
			break;

		/* === Maintenance & Utility Features === */
		case palerain_option_case_auto_update:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_auto_update);
			break;
		case palerain_option_case_health_check:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_self_diagnostics);
			break;
		case palerain_option_case_cache_clean:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_cache_manage);
			break;
		case palerain_option_case_log_rotate:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_log_rotation);
			break;
		case palerain_option_case_disk_cleanup:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_disk_cleanup);
			break;
		case palerain_option_case_health_monitor:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_health_monitor);
			break;
		case palerain_option_case_version_check:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_version_check);
			break;
		case palerain_option_case_dep_manager:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dep_manager);
			break;

		/* === Bonus Features === */
		case palerain_option_case_benchmark:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_benchmark);
			break;
		case palerain_option_case_persist_check:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_persist_check);
			break;

		/* === Additional Features === */
		case palerain_option_case_device_info_detailed:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_info_detailed);
			break;
		case palerain_option_case_device_tree:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_device_tree);
			break;
		case palerain_option_case_kernel_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_dump);
			break;
		case palerain_option_case_usb_sniffer:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_sniffer);
			break;
		case palerain_option_case_boot_chain:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_boot_chain);
			break;
		case palerain_option_case_firmware_keys:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_firmware_keys);
			break;
		case palerain_option_case_pongo_shell:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_shell);
			break;
		case palerain_option_case_recovery_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_mode_test);
			break;

		/* === Ultra Verbose & Logging Features (10) === */
		case palerain_option_case_ultra_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ultra_verbose);
			break;
		case palerain_option_case_trace_usb:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_usb);
			break;
		case palerain_option_case_trace_memory:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_memory);
			break;
		case palerain_option_case_trace_exploit:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_exploit);
			break;
		case palerain_option_case_trace_pongo:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_pongo);
			break;
		case palerain_option_case_trace_kpf:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_kpf);
			break;
		case palerain_option_case_trace_ramdisk:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_ramdisk);
			break;
		case palerain_option_case_trace_overlay:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_overlay);
			break;
		case palerain_option_case_trace_bootx:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trace_bootx);
			break;
		case palerain_option_case_log_all_stages:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_log_all_stages);
			break;

		/* === PongoOS Stage Diagnostics (10) === */
		case palerain_option_case_pongo_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_verbose);
			break;
		case palerain_option_case_pongo_cmd_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_cmd_log);
			break;
		case palerain_option_case_pongo_response_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_response_log);
			break;
		case palerain_option_case_pongo_mem_map:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_mem_map);
			break;
		case palerain_option_case_pongo_reg_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_reg_dump);
			break;
		case palerain_option_case_pongo_usb_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_usb_log);
			break;
		case palerain_option_case_pongo_exception_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_exception_log);
			break;
		case palerain_option_case_pongo_task_list:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_task_list);
			break;
		case palerain_option_case_pongo_heap_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_heap_dump);
			break;
		case palerain_option_case_pongo_symbols:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_symbols);
			break;

		/* === Exploit Stage Diagnostics (10) === */
		case palerain_option_case_exploit_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_verbose);
			break;
		case palerain_option_case_exploit_step_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_step_trace);
			break;
		case palerain_option_case_exploit_mem_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_mem_trace);
			break;
		case palerain_option_case_exploit_reg_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_reg_trace);
			break;
		case palerain_option_case_exploit_timing:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_timing);
			break;
		case palerain_option_case_exploit_retry_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_retry_log);
			break;
		case palerain_option_case_exploit_payload_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_payload_log);
			break;
		case palerain_option_case_exploit_state_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_state_dump);
			break;
		case palerain_option_case_exploit_side_effects:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_side_effects);
			break;
		case palerain_option_case_exploit_validation:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_validation);
			break;

		/* === Kernel & Memory Diagnostics (10) === */
		case palerain_option_case_kernel_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_verbose);
			break;
		case palerain_option_case_kernel_mem_map:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_mem_map);
			break;
		case palerain_option_case_kernel_slide_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_slide_log);
			break;
		case palerain_option_case_kernel_patch_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_patch_log);
			break;
		case palerain_option_case_kernel_symbols:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_symbols);
			break;
		case palerain_option_case_kernel_stack_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_stack_trace);
			break;
		case palerain_option_case_kernel_page_track:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_page_track);
			break;
		case palerain_option_case_kernel_vm_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_vm_log);
			break;
		case palerain_option_case_kernel_kalloc_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_kalloc_log);
			break;
		case palerain_option_case_kernel_zone_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernel_zone_log);
			break;

		/* === USB & Communication Diagnostics (10) === */
		case palerain_option_case_usb_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_verbose);
			break;
		case palerain_option_case_usb_control_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_control_trace);
			break;
		case palerain_option_case_usb_bulk_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_bulk_trace);
			break;
		case palerain_option_case_usb_iso_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_iso_trace);
			break;
		case palerain_option_case_usb_device_tree:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_device_tree);
			break;
		case palerain_option_case_usb_endpoint_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_endpoint_log);
			break;
		case palerain_option_case_usb_speed_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_speed_log);
			break;
		case palerain_option_case_usb_power_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_power_log);
			break;
		case palerain_option_case_usb_reset_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_reset_trace);
			break;
		case palerain_option_case_usb_hub_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_hub_log);
			break;

		/* === Device Tree & Firmware Analysis (10) === */
		case palerain_option_case_dtree_full_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_full_dump);
			break;
		case palerain_option_case_dtree_compare:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_compare);
			break;
		case palerain_option_case_dtree_property_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_property_log);
			break;
		case palerain_option_case_dtree_compatible:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_compatible);
			break;
		case palerain_option_case_dtree_reg_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_reg_dump);
			break;
		case palerain_option_case_dtree_interrupts:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_interrupts);
			break;
		case palerain_option_case_dtree_clocks:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_clocks);
			break;
		case palerain_option_case_dtree_power:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_power);
			break;
		case palerain_option_case_dtree_gpio:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtree_gpio);
			break;
		case palerain_option_case_firmware_analyze:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_firmware_analyze);
			break;

		/* === Security & Cryptography Features (10) === */
		case palerain_option_case_aes_engine_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_aes_engine_trace);
			break;
		case palerain_option_case_sep_comm_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_comm_log);
			break;
		case palerain_option_case_sep_firmware_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_firmware_dump);
			break;
		case palerain_option_case_sep_key_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_key_trace);
			break;
		case palerain_option_case_gid_uid_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_gid_uid_trace);
			break;
		case palerain_option_case_firmware_decrypt:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_firmware_decrypt);
			break;
		case palerain_option_case_img4_parse_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_img4_parse_log);
			break;
		case palerain_option_case_ap_ticket_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ap_ticket_log);
			break;
		case palerain_option_case_sep_ticket_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_ticket_log);
			break;
		case palerain_option_case_keybag_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_keybag_dump);
			break;

		/* === Advanced Jailbreak Features (10) === */
		case palerain_option_case_rootless_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_rootless_verbose);
			break;
		case palerain_option_case_fakefs_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fakefs_verbose);
			break;
		case palerain_option_case_bind_mount_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bind_mount_log);
			break;
		case palerain_option_case_overlay_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_overlay_verbose);
			break;
		case palerain_option_case_sandbox_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sandbox_log);
			break;
		case palerain_option_case_amfi_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_amfi_log);
			break;
		case palerain_option_case_codesign_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_codesign_log);
			break;
		case palerain_option_case_entitlement_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_entitlement_log);
			break;
		case palerain_option_case_sip_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sip_log);
			break;
		case palerain_option_case_tfp0_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_tfp0_log);
			break;

		/* === Automation & Testing Features (10) === */
		case palerain_option_case_auto_retry:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_auto_retry);
			break;
		case palerain_option_case_max_retries:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_max_retries);
			break;
		case palerain_option_case_test_mode:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_test_mode);
			break;
		case palerain_option_case_stress_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_stress_test);
			break;
		case palerain_option_case_fuzz_usb:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_usb);
			break;
		case palerain_option_case_fuzz_exploit:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_exploit);
			break;
		case palerain_option_case_fuzz_pongo:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_pongo);
			break;
		case palerain_option_case_ci_mode:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ci_mode);
			break;
		case palerain_option_case_benchmark_all:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_benchmark_all);
			break;
		case palerain_option_case_perf_report:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_report);
			break;

		/* === Developer & Research Features (10) === */
		case palerain_option_case_research_mode:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_research_mode);
			break;
		case palerain_option_case_debug_breakpoints:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_debug_breakpoints);
			break;
		case palerain_option_case_memory_watchpoints:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_memory_watchpoints);
			break;
		case palerain_option_case_instruction_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_instruction_trace);
			break;
		case palerain_option_case_coverage_tracking:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_coverage_tracking);
			break;
		case palerain_option_case_custom_hooks:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_custom_hooks);
			break;
		case palerain_option_case_plugin_debug:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_plugin_debug);
			break;
		case palerain_option_case_api_debug:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_api_debug);
			break;
		case palerain_option_case_script_debug:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_script_debug);
			break;
		case palerain_option_case_experimental:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_experimental);
			break;

		/* === Phase 2: Advanced Diagnostics & Analysis (100 features) === */

		/* === Hardware & Peripheral Diagnostics (10) === */
		case palerain_option_case_gpio_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_gpio_trace);
			break;
		case palerain_option_case_i2c_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_i2c_trace);
			break;
		case palerain_option_case_spi_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_spi_trace);
			break;
		case palerain_option_case_uart_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_uart_trace);
			break;
		case palerain_option_case_pmic_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pmic_trace);
			break;
		case palerain_option_case_display_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_display_trace);
			break;
		case palerain_option_case_touch_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_touch_trace);
			break;
		case palerain_option_case_audio_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_audio_trace);
			break;
		case palerain_option_case_camera_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_camera_trace);
			break;
		case palerain_option_case_sensor_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sensor_trace);
			break;

		/* === Power & Thermal Management (10) === */
		case palerain_option_case_power_domains:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_power_domains);
			break;
		case palerain_option_case_thermal_zones:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_thermal_zones);
			break;
		case palerain_option_case_battery_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_battery_log);
			break;
		case palerain_option_case_charging_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_charging_log);
			break;
		case palerain_option_case_dvfs_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dvfs_log);
			break;
		case palerain_option_case_idle_states:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_idle_states);
			break;
		case palerain_option_case_suspend_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_suspend_log);
			break;
		case palerain_option_case_wakeup_sources:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_wakeup_sources);
			break;
		case palerain_option_case_power_stats:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_power_stats);
			break;
		case palerain_option_case_energy_model:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_energy_model);
			break;

		/* === Filesystem & Storage Diagnostics (10) === */
		case palerain_option_case_apfs_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_apfs_verbose);
			break;
		case palerain_option_case_apfs_snapshots:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_apfs_snapshots);
			break;
		case palerain_option_case_apfs_clones:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_apfs_clones);
			break;
		case palerain_option_case_apfs_encryption:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_apfs_encryption);
			break;
		case palerain_option_case_nand_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_nand_trace);
			break;
		case palerain_option_case_nvme_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_nvme_trace);
			break;
		case palerain_option_case_storage_stats:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_storage_stats);
			break;
		case palerain_option_case_trim_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_trim_log);
			break;
		case palerain_option_case_wear_leveling:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_wear_leveling);
			break;
		case palerain_option_case_bad_block_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bad_block_log);
			break;

		/* === Network & Connectivity Diagnostics (10) === */
		case palerain_option_case_wifi_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_wifi_trace);
			break;
		case palerain_option_case_bluetooth_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bluetooth_trace);
			break;
		case palerain_option_case_cellular_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_cellular_trace);
			break;
		case palerain_option_case_nfc_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_nfc_trace);
			break;
		case palerain_option_case_tcpip_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_tcpip_trace);
			break;
		case palerain_option_case_dns_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dns_trace);
			break;
		case palerain_option_case_tls_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_tls_trace);
			break;
		case palerain_option_case_vpn_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_vpn_trace);
			break;
		case palerain_option_case_airdrop_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_airdrop_trace);
			break;
		case palerain_option_case_continuity_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_continuity_trace);
			break;

		/* === Graphics & Display Diagnostics (10) === */
		case palerain_option_case_gpu_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_gpu_trace);
			break;
		case palerain_option_case_metal_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_metal_trace);
			break;
		case palerain_option_case_iosurface_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_iosurface_trace);
			break;
		case palerain_option_case_cvms_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_cvms_trace);
			break;
		case palerain_option_case_compositor_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_compositor_trace);
			break;
		case palerain_option_case_display_link:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_display_link);
			break;
		case palerain_option_case_vsync_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_vsync_trace);
			break;
		case palerain_option_case_layer_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_layer_trace);
			break;
		case palerain_option_case_color_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_color_trace);
			break;
		case palerain_option_case_hdr_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hdr_trace);
			break;

		/* === Security & Enclave Diagnostics (10) === */
		case palerain_option_case_sep_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_verbose);
			break;
		case palerain_option_case_sep_mailbox:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_mailbox);
			break;
		case palerain_option_case_sep_memory:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_memory);
			break;
		case palerain_option_case_sep_firmware_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sep_firmware_log);
			break;
		case palerain_option_case_tz_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_tz_verbose);
			break;
		case palerain_option_case_tz_memory:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_tz_memory);
			break;
		case palerain_option_case_tz_smc_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_tz_smc_trace);
			break;
		case palerain_option_case_secure_boot_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_secure_boot_log);
			break;
		case palerain_option_case_keybag_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_keybag_trace);
			break;
		case palerain_option_case_entanglement_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_entanglement_log);
			break;

		/* === Memory Management Diagnostics (10) === */
		case palerain_option_case_vm_map_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_vm_map_dump);
			break;
		case palerain_option_case_vm_fault_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_vm_fault_trace);
			break;
		case palerain_option_case_physmem_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_physmem_log);
			break;
		case palerain_option_case_page_reclaim:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_page_reclaim);
			break;
		case palerain_option_case_compression_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_compression_log);
			break;
		case palerain_option_case_swap_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_swap_trace);
			break;
		case palerain_option_case_jemalloc_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_jemalloc_trace);
			break;
		case palerain_option_case_zone_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_zone_trace);
			break;
		case palerain_option_case_vm_pressure:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_vm_pressure);
			break;
		case palerain_option_case_memory_tags:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_memory_tags);
			break;

		/* === Scheduler & Process Diagnostics (10) === */
		case palerain_option_case_sched_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_sched_trace);
			break;
		case palerain_option_case_thread_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_thread_trace);
			break;
		case palerain_option_case_workqueue_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_workqueue_trace);
			break;
		case palerain_option_case_timer_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_timer_trace);
			break;
		case palerain_option_case_ipc_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ipc_trace);
			break;
		case palerain_option_case_mach_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_mach_trace);
			break;
		case palerain_option_case_exception_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exception_trace);
			break;
		case palerain_option_case_signal_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_signal_trace);
			break;
		case palerain_option_case_proc_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_proc_trace);
			break;
		case palerain_option_case_task_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_task_trace);
			break;

		/* === IOKit & Driver Diagnostics (10) === */
		case palerain_option_case_ioregistry_dump:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ioregistry_dump);
			break;
		case palerain_option_case_iokit_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_iokit_trace);
			break;
		case palerain_option_case_driver_match:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_driver_match);
			break;
		case palerain_option_case_property_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_property_trace);
			break;
		case palerain_option_case_service_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_service_trace);
			break;
		case palerain_option_case_notification_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_notification_trace);
			break;
		case palerain_option_case_powerman_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_powerman_trace);
			break;
		case palerain_option_case_thermal_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_thermal_trace);
			break;
		case palerain_option_case_pci_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pci_trace);
			break;
		case palerain_option_case_thunderbolt_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_thunderbolt_trace);
			break;

		/* === Boot & Startup Diagnostics (10) === */
		case palerain_option_case_iboot_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_iboot_verbose);
			break;
		case palerain_option_case_iboot_env:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_iboot_env);
			break;
		case palerain_option_case_iboot_commands:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_iboot_commands);
			break;
		case palerain_option_case_diags_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_diags_log);
			break;
		case palerain_option_case_recovery_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_log);
			break;
		case palerain_option_case_dfu_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_log);
			break;
		case palerain_option_case_ramdisk_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ramdisk_log);
			break;
		case palerain_option_case_kernelcache_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kernelcache_log);
			break;
		case palerain_option_case_prelink_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_prelink_log);
			break;
		case palerain_option_case_boot_progress:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_boot_progress);
			break;

		/* === Phase 3: PongoOS Stage Diagnostics & Advanced Features (100 features) === */

		/* === PongoOS Stage Diagnostics (20) === */
		case palerain_option_case_pongo_stage_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_stage_verbose);
			break;
		case palerain_option_case_pongo_stage_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_stage_log);
			break;
		case palerain_option_case_pongo_mem_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_mem_log);
			break;
		case palerain_option_case_pongo_heap_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_heap_log);
			break;
		case palerain_option_case_pongo_task_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_task_log);
			break;
		case palerain_option_case_pongo_irq_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_irq_log);
			break;
		case palerain_option_case_pongo_timer_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_timer_log);
			break;
		case palerain_option_case_pongo_uart_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_uart_log);
			break;
		case palerain_option_case_pongo_usb_event_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_usb_event_log);
			break;
		case palerain_option_case_pongo_wdt_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_wdt_log);
			break;
		case palerain_option_case_pongo_crash_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_crash_log);
			break;
		case palerain_option_case_pongo_syscall_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_syscall_log);
			break;
		case palerain_option_case_pongo_ipc_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_ipc_log);
			break;
		case palerain_option_case_pongo_sem_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_sem_log);
			break;
		case palerain_option_case_pongo_mutex_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_mutex_log);
			break;
		case palerain_option_case_pongo_cond_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_cond_log);
			break;
		case palerain_option_case_pongo_queue_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_queue_log);
			break;
		case palerain_option_case_pongo_pool_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_pool_log);
			break;
		case palerain_option_case_pongo_dma_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_dma_log);
			break;
		case palerain_option_case_pongo_pmu_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_pongo_pmu_log);
			break;

		/* === Kernel Exploitation Diagnostics (10) === */
		case palerain_option_case_exploit_diag_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_diag_verbose);
			break;
		case palerain_option_case_exploit_stage_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_stage_log);
			break;
		case palerain_option_case_exploit_heap_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_heap_log);
			break;
		case palerain_option_case_exploit_rop_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_rop_log);
			break;
		case palerain_option_case_exploit_kcall_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_kcall_log);
			break;
		case palerain_option_case_exploit_kread_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_kread_log);
			break;
		case palerain_option_case_exploit_kwrite_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_kwrite_log);
			break;
		case palerain_option_case_exploit_pac_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_pac_log);
			break;
		case palerain_option_case_exploit_pp_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_pp_log);
			break;
		case palerain_option_case_exploit_slide_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_slide_log);
			break;

		/* === Checkra1n Integration Diagnostics (10) === */
		case palerain_option_case_checkra1n_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_verbose);
			break;
		case palerain_option_case_checkra1n_stage_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_stage_log);
			break;
		case palerain_option_case_checkra1n_pongo_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_pongo_log);
			break;
		case palerain_option_case_checkra1n_boot_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_boot_log);
			break;
		case palerain_option_case_checkra1n_ramdisk_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_ramdisk_log);
			break;
		case palerain_option_case_checkra1n_kernel_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_kernel_log);
			break;
		case palerain_option_case_checkra1n_hook_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_hook_log);
			break;
		case palerain_option_case_checkra1n_shell_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_shell_log);
			break;
		case palerain_option_case_checkra1n_ssh_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_ssh_log);
			break;
		case palerain_option_case_checkra1n_cleanup_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_checkra1n_cleanup_log);
			break;

		/* === Advanced USB Diagnostics (10) === */
		case palerain_option_case_usb_diag_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_diag_verbose);
			break;
		case palerain_option_case_usb_ep_diag_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_ep_diag_log);
			break;
		case palerain_option_case_usb_transfer_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_transfer_log);
			break;
		case palerain_option_case_usb_control_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_control_log);
			break;
		case palerain_option_case_usb_bulk_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_bulk_log);
			break;
		case palerain_option_case_usb_interrupt_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_interrupt_log);
			break;
		case palerain_option_case_usb_isochronous_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_isochronous_log);
			break;
		case palerain_option_case_usb_device_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_device_log);
			break;
		case palerain_option_case_usb_interface_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_interface_log);
			break;
		case palerain_option_case_usb_config_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_usb_config_log);
			break;

		/* === DFU Mode Diagnostics (10) === */
		case palerain_option_case_dfu_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_verbose);
			break;
		case palerain_option_case_dfu_state_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_state_log);
			break;
		case palerain_option_case_dfu_command_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_command_log);
			break;
		case palerain_option_case_dfu_upload_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_upload_log);
			break;
		case palerain_option_case_dfu_download_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_download_log);
			break;
		case palerain_option_case_dfu_getstatus_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_getstatus_log);
			break;
		case palerain_option_case_dfu_clrstatus_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_clrstatus_log);
			break;
		case palerain_option_case_dfu_abort_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_abort_log);
			break;
		case palerain_option_case_dfu_detach_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_detach_log);
			break;
		case palerain_option_case_dfu_reset_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dfu_reset_log);
			break;

		/* === Recovery Mode Diagnostics (10) === */
		case palerain_option_case_recovery_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_verbose);
			break;
		case palerain_option_case_recovery_state_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_state_log);
			break;
		case palerain_option_case_recovery_command_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_command_log);
			break;
		case palerain_option_case_recovery_upload_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_upload_log);
			break;
		case palerain_option_case_recovery_download_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_download_log);
			break;
		case palerain_option_case_recovery_nonce_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_nonce_log);
			break;
		case palerain_option_case_recovery_apnonce_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_apnonce_log);
			break;
		case palerain_option_case_recovery_sepnonce_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_sepnonce_log);
			break;
		case palerain_option_case_recovery_ticket_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_ticket_log);
			break;
		case palerain_option_case_recovery_reset_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_recovery_reset_log);
			break;

		/* === Bootloader Diagnostics (10) === */
		case palerain_option_case_bootloader_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_verbose);
			break;
		case palerain_option_case_bootloader_stage_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_stage_log);
			break;
		case palerain_option_case_bootloader_iboot_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_iboot_log);
			break;
		case palerain_option_case_bootloader_ibec_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_ibec_log);
			break;
		case palerain_option_case_bootloader_ibss_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_ibss_log);
			break;
		case palerain_option_case_bootloader_lowlevel_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_lowlevel_log);
			break;
		case palerain_option_case_bootloader_mem_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_mem_log);
			break;
		case palerain_option_case_bootloader_reg_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_reg_log);
			break;
		case palerain_option_case_bootloader_dev_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_dev_log);
			break;
		case palerain_option_case_bootloader_cmd_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_bootloader_cmd_log);
			break;

		/* === Filesystem Diagnostics (10) === */
		case palerain_option_case_fs_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_verbose);
			break;
		case palerain_option_case_fs_mount_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_mount_log);
			break;
		case palerain_option_case_fs_unmount_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_unmount_log);
			break;
		case palerain_option_case_fs_read_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_read_log);
			break;
		case palerain_option_case_fs_write_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_write_log);
			break;
		case palerain_option_case_fs_create_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_create_log);
			break;
		case palerain_option_case_fs_delete_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_delete_log);
			break;
		case palerain_option_case_fs_rename_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_rename_log);
			break;
		case palerain_option_case_fs_stat_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_stat_log);
			break;
		case palerain_option_case_fs_xattr_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fs_xattr_log);
			break;

		/* === Network Diagnostics (10) === */
		case palerain_option_case_net_verbose:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_verbose);
			break;
		case palerain_option_case_net_packet_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_packet_log);
			break;
		case palerain_option_case_net_socket_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_socket_log);
			break;
		case palerain_option_case_net_route_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_route_log);
			break;
		case palerain_option_case_net_arp_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_arp_log);
			break;
		case palerain_option_case_net_ndp_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_ndp_log);
			break;
		case palerain_option_case_net_dhcp_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_dhcp_log);
			break;
		case palerain_option_case_net_dns_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_dns_log);
			break;
		case palerain_option_case_net_tls_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_tls_log);
			break;
		case palerain_option_case_net_http_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_net_http_log);
			break;

		/* === Phase 4: Advanced Developer Tools & Debugging (100 features) === */

		/* === Kernel Development & Debugging (15) === */
		case palerain_option_case_kern_debug_enter:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_debug_enter);
			break;
		case palerain_option_case_kern_debug_wait:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_debug_wait);
			break;
		case palerain_option_case_kern_break_early:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_break_early);
			break;
		case palerain_option_case_kern_watchdog_disable:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_watchdog_disable);
			break;
		case palerain_option_case_kern_panic_log:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_panic_log);
			break;
		case palerain_option_case_kern_backtrace_all:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_backtrace_all);
			break;
		case palerain_option_case_kern_lock_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_lock_stat);
			break;
		case palerain_option_case_kern_intr_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_intr_stat);
			break;
		case palerain_option_case_kern_syscall_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_syscall_stat);
			break;
		case palerain_option_case_kern_vm_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_vm_stat);
			break;
		case palerain_option_case_kern_task_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_task_stat);
			break;
		case palerain_option_case_kern_zone_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_zone_stat);
			break;
		case palerain_option_case_kern_kalloc_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_kalloc_stat);
			break;
		case palerain_option_case_kern_upl_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_upl_stat);
			break;
		case palerain_option_case_kern_io_stat:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_kern_io_stat);
			break;

		/* === Dynamic Analysis & Instrumentation (15) === */
		case palerain_option_case_dtrace_enable:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dtrace_enable);
			break;
		case palerain_option_case_ktrace_enable:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ktrace_enable);
			break;
		case palerain_option_case_ftrace_enable:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_ftrace_enable);
			break;
		case palerain_option_case_perf_counters:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_counters);
			break;
		case palerain_option_case_branch_trace:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_branch_trace);
			break;
		case palerain_option_case_call_graph:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_call_graph);
			break;
		case palerain_option_case_hot_path_detect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hot_path_detect);
			break;
		case palerain_option_case_mem_leak_detect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_mem_leak_detect);
			break;
		case palerain_option_case_use_after_free:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_use_after_free);
			break;
		case palerain_option_case_double_free_detect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_double_free_detect);
			break;
		case palerain_option_case_buffer_overflow:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_buffer_overflow);
			break;
		case palerain_option_case_race_detect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_race_detect);
			break;
		case palerain_option_case_deadlock_detect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_deadlock_detect);
			break;
		case palerain_option_case_stack_canary_check:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_stack_canary_check);
			break;
		case palerain_option_case_shadow_stack:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_shadow_stack);
			break;

		/* === Fuzzing & Testing Infrastructure (10) === */
		case palerain_option_case_fuzz_kernel:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_kernel);
			break;
		case palerain_option_case_fuzz_driver:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_driver);
			break;
		case palerain_option_case_fuzz_filesystem:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_filesystem);
			break;
		case palerain_option_case_fuzz_network:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_network);
			break;
		case palerain_option_case_fuzz_usb_stack:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_usb_stack);
			break;
		case palerain_option_case_fuzz_ipc:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_ipc);
			break;
		case palerain_option_case_fuzz_mach:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fuzz_mach);
			break;
		case palerain_option_case_property_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_property_test);
			break;
		case palerain_option_case_model_check:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_model_check);
			break;
		case palerain_option_case_symbolic_exec:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_symbolic_exec);
			break;

		/* === Reverse Engineering & Analysis (10) === */
		case palerain_option_case_re_disassemble:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_disassemble);
			break;
		case palerain_option_case_re_cfg_build:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_cfg_build);
			break;
		case palerain_option_case_re_data_flow:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_data_flow);
			break;
		case palerain_option_case_re_type_recovery:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_type_recovery);
			break;
		case palerain_option_case_re_struct_recovery:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_struct_recovery);
			break;
		case palerain_option_case_re_string_analysis:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_string_analysis);
			break;
		case palerain_option_case_re_xref_find:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_xref_find);
			break;
		case palerain_option_case_re_pattern_match:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_pattern_match);
			break;
		case palerain_option_case_re_signature_gen:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_signature_gen);
			break;
		case palerain_option_case_re_diff_binary:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_re_diff_binary);
			break;

		/* === Exploit Development (10) === */
		case palerain_option_case_exploit_dev_mode:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_dev_mode);
			break;
		case palerain_option_case_exploit_payload_gen:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_payload_gen);
			break;
		case palerain_option_case_exploit_rop_build:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_rop_build);
			break;
		case palerain_option_case_exploit_jop_build:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_jop_build);
			break;
		case palerain_option_case_exploit_srop_build:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_srop_build);
			break;
		case palerain_option_case_exploit_pac_bypass:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_pac_bypass);
			break;
		case palerain_option_case_exploit_pp_bypass:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_pp_bypass);
			break;
		case palerain_option_case_exploit_kaslr_leak:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_kaslr_leak);
			break;
		case palerain_option_case_exploit_heap_feng:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_heap_feng);
			break;
		case palerain_option_case_exploit_race_win:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_exploit_race_win);
			break;

		/* === Hardware Bring-up & Validation (10) === */
		case palerain_option_case_hw_bringup:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_bringup);
			break;
		case palerain_option_case_hw_memory_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_memory_test);
			break;
		case palerain_option_case_hw_cache_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_cache_test);
			break;
		case palerain_option_case_hw_interconnect:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_interconnect);
			break;
		case palerain_option_case_hw_timer_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_timer_test);
			break;
		case palerain_option_case_hw_interrupt_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_interrupt_test);
			break;
		case palerain_option_case_hw_dma_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_dma_test);
			break;
		case palerain_option_case_hw_pmu_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_pmu_test);
			break;
		case palerain_option_case_hw_thermal_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_thermal_test);
			break;
		case palerain_option_case_hw_power_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_hw_power_test);
			break;

		/* === Firmware & Bootloader Development (10) === */
		case palerain_option_case_fw_dev_mode:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_dev_mode);
			break;
		case palerain_option_case_fw_patch_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_patch_test);
			break;
		case palerain_option_case_fw_verify_chain:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_verify_chain);
			break;
		case palerain_option_case_fw_key_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_key_test);
			break;
		case palerain_option_case_fw_rollback_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_rollback_test);
			break;
		case palerain_option_case_fw_dfu_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_dfu_test);
			break;
		case palerain_option_case_fw_recovery_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_recovery_test);
			break;
		case palerain_option_case_fw_ramdisk_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_ramdisk_test);
			break;
		case palerain_option_case_fw_overlay_test:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_overlay_test);
			break;
		case palerain_option_case_fw_integrity:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_fw_integrity);
			break;

		/* === Performance & Optimization (10) === */
		case palerain_option_case_perf_baseline:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_baseline);
			break;
		case palerain_option_case_perf_regression:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_regression);
			break;
		case palerain_option_case_perf_optimize:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_optimize);
			break;
		case palerain_option_case_perf_tune_cpu:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_cpu);
			break;
		case palerain_option_case_perf_tune_gpu:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_gpu);
			break;
		case palerain_option_case_perf_tune_mem:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_mem);
			break;
		case palerain_option_case_perf_tune_io:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_io);
			break;
		case palerain_option_case_perf_tune_net:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_tune_net);
			break;
		case palerain_option_case_perf_energy:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_energy);
			break;
		case palerain_option_case_perf_thermal:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_perf_thermal);
			break;

		/* === Developer Workflow & Productivity (10) === */
		case palerain_option_case_dev_hot_reload:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_hot_reload);
			break;
		case palerain_option_case_dev_live_patch:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_live_patch);
			break;
		case palerain_option_case_dev_repl:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_repl);
			break;
		case palerain_option_case_dev_snippets:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_snippets);
			break;
		case palerain_option_case_dev_templates:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_templates);
			break;
		case palerain_option_case_dev_ci_integration:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_ci_integration);
			break;
		case palerain_option_case_dev_git_hooks:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_git_hooks);
			break;
		case palerain_option_case_dev_docker:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_docker);
			break;
		case palerain_option_case_dev_vscode:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_vscode);
			break;
		case palerain_option_case_dev_clangd:
			PALERAIN_OPTION_SET(palerain_flags, palerain_option_dev_clangd);
			break;

		default:
			usage(1, argv[0]);
			break;
		}
	}
	if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_palerain_version)) {
		printf(
			"palera1n " PALERAIN_VERSION "\n"
			BUILD_COMMIT " " BUILD_NUMBER " (" BUILD_BRANCH ")\n\n"
			"Build style: " BUILD_STYLE "\n"
			"Build tag: " BUILD_TAG "\n"
#ifdef USE_LIBUSB
			"USB backend: libusb\n"
#else
			"USB backend: IOKit\n"
#endif
			"Build options: " BUILD_OPTIONS "\n"
		);
		return 0;
	}

	if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_telnetd)) {
		LOG(LOG_WARNING, "telnetd is enabled, this is a security hole");
	}

	if ((PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tui)) && (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_cli))) {
		LOG(LOG_FATAL, "cannot specify both --tui and --cli");
		return -1;
	}

	/* Check if any CLI-only option is set */
	if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exit_recovery) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_enter_recovery) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_reboot_device) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_info) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfuhelper_only) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_exit) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_full) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_simulator_pwned_dfu) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_list) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_group) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_autodetect) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_auto_update) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_self_diagnostics) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_cache_manage) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_log_rotation) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_disk_cleanup) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_health_monitor) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_version_check) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dep_manager) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_benchmark) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_persist_check) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_diagnostics) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dry_run) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_interactive_wizard) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_export) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_filter) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_rename) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_notes) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_favorite) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_backup_restore) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_multi_exploit) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_chain) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_custom_payload) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_boot_presets) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_patches) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ramdisk_cust) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_overlay_mgmt) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_verbose_boot_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_progress_anim) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_colored_themes) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sound_notify) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_haptic_feedback) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_multi_lang) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_accessibility) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_quick_actions) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_debug_console) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_mem_inspect) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_reg_viewer) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_syscall_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_debug) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_profile) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_crash_report) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_log_analyze) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_batch_ops) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_scheduled_tasks) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_script_engine) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_plugin_system) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_api_server) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_webhook) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ci_cd) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_config_templates) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_auto) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_auto) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_safe_boot) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloop_detect) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_auto_recover) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_data_protect) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_emergency_exit) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pwned_dfu) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_info_detailed) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_tree) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_sniffer) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_boot_chain) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_firmware_keys) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_shell) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_mode_test) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ultra_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_usb) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_memory) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_exploit) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_pongo) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_kpf) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_ramdisk) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_overlay) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trace_bootx) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_log_all_stages) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_cmd_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_response_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_mem_map) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_reg_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_usb_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_exception_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_task_list) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_heap_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_symbols) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_step_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_mem_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_reg_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_timing) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_retry_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_payload_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_state_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_side_effects) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_validation) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_mem_map) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_slide_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_patch_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_symbols) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_stack_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_page_track) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_vm_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_kalloc_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernel_zone_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_control_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_bulk_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_iso_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_device_tree) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_endpoint_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_speed_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_power_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_reset_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_hub_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_full_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_compare) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_property_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_compatible) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_reg_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_interrupts) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_clocks) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_power) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtree_gpio) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_firmware_analyze) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_aes_engine_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_comm_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_firmware_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_key_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_gid_uid_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_firmware_decrypt) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_img4_parse_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ap_ticket_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_ticket_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_keybag_dump) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_rootless_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fakefs_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bind_mount_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_overlay_verbose) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sandbox_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_amfi_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_codesign_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_entitlement_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sip_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tfp0_log) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_auto_retry) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_max_retries) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_test_mode) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_stress_test) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_usb) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_exploit) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_pongo) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ci_mode) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_benchmark_all) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_report) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_research_mode) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_debug_breakpoints) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_memory_watchpoints) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_instruction_trace) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_coverage_tracking) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_custom_hooks) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_plugin_debug) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_api_debug) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_script_debug) ||
	    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_experimental) ||

		    /* === Phase 2: Advanced Diagnostics & Analysis (bits 206-305) === */
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_gpio_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_i2c_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_spi_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_uart_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pmic_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_display_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_touch_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_audio_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_camera_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sensor_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_power_domains) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_thermal_zones) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_battery_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_charging_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dvfs_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_idle_states) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_suspend_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_wakeup_sources) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_power_stats) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_energy_model) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_apfs_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_apfs_snapshots) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_apfs_clones) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_apfs_encryption) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_nand_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_nvme_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_storage_stats) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_trim_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_wear_leveling) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bad_block_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_wifi_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bluetooth_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_cellular_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_nfc_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tcpip_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dns_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tls_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_vpn_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_airdrop_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_continuity_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_gpu_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_metal_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_iosurface_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_cvms_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_compositor_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_display_link) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_vsync_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_layer_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_color_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hdr_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_mailbox) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_memory) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sep_firmware_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tz_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tz_memory) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_tz_smc_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_secure_boot_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_keybag_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_entanglement_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_vm_map_dump) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_vm_fault_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_physmem_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_page_reclaim) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_compression_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_swap_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_jemalloc_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_zone_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_vm_pressure) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_memory_tags) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_sched_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_thread_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_workqueue_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_timer_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ipc_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_mach_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exception_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_signal_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_proc_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_task_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ioregistry_dump) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_iokit_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_driver_match) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_property_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_service_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_notification_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_powerman_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_thermal_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pci_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_thunderbolt_trace) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_iboot_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_iboot_env) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_iboot_commands) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_diags_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ramdisk_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kernelcache_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_prelink_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_boot_progress) ||

		    /* === Phase 3: PongoOS Stage Diagnostics & Advanced Features (bits 306-406) === */
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_stage_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_stage_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_mem_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_heap_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_task_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_irq_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_timer_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_uart_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_usb_event_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_wdt_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_crash_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_syscall_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_ipc_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_sem_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_mutex_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_cond_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_queue_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_pool_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_dma_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_pmu_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_diag_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_stage_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_heap_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_rop_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_kcall_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_kread_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_kwrite_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_pac_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_pp_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_slide_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_stage_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_pongo_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_boot_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_ramdisk_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_kernel_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_hook_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_shell_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_ssh_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_checkra1n_cleanup_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_diag_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_ep_diag_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_transfer_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_control_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_bulk_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_interrupt_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_isochronous_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_device_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_interface_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_usb_config_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_state_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_command_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_upload_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_download_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_getstatus_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_clrstatus_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_abort_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_detach_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfu_reset_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_state_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_command_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_upload_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_download_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_nonce_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_apnonce_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_sepnonce_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_ticket_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_recovery_reset_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_stage_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_iboot_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_ibec_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_ibss_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_lowlevel_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_mem_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_reg_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_dev_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_bootloader_cmd_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_mount_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_unmount_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_read_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_write_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_create_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_delete_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_rename_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_stat_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fs_xattr_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_verbose) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_packet_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_socket_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_route_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_arp_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_ndp_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_dhcp_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_dns_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_tls_log) ||
		    PALERAIN_OPTION_TEST(palerain_flags, palerain_option_net_http_log) ||

			/* === Phase 4: Advanced Developer Tools & Debugging (bits 407-506) === */

			/* === Kernel Development & Debugging (15) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_debug_enter) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_debug_wait) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_break_early) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_watchdog_disable) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_panic_log) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_backtrace_all) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_lock_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_intr_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_syscall_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_vm_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_task_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_zone_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_kalloc_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_upl_stat) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_kern_io_stat) ||

			/* === Dynamic Analysis & Instrumentation (15) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dtrace_enable) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ktrace_enable) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_ftrace_enable) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_counters) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_branch_trace) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_call_graph) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hot_path_detect) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_mem_leak_detect) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_use_after_free) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_double_free_detect) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_buffer_overflow) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_race_detect) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_deadlock_detect) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_stack_canary_check) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_shadow_stack) ||

			/* === Fuzzing & Testing Infrastructure (10) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_kernel) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_driver) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_filesystem) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_network) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_usb_stack) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_ipc) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fuzz_mach) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_property_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_model_check) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_symbolic_exec) ||

			/* === Reverse Engineering & Analysis (10) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_disassemble) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_cfg_build) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_data_flow) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_type_recovery) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_struct_recovery) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_string_analysis) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_xref_find) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_pattern_match) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_signature_gen) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_re_diff_binary) ||

			/* === Exploit Development (10) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_dev_mode) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_payload_gen) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_rop_build) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_jop_build) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_srop_build) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_pac_bypass) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_pp_bypass) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_kaslr_leak) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_heap_feng) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exploit_race_win) ||

			/* === Hardware Bring-up & Validation (10) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_bringup) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_memory_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_cache_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_interconnect) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_timer_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_interrupt_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_dma_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_pmu_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_thermal_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_hw_power_test) ||

			/* === Firmware & Bootloader Development (10) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_dev_mode) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_patch_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_verify_chain) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_key_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_rollback_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_dfu_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_recovery_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_ramdisk_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_overlay_test) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_fw_integrity) ||

			/* === Performance & Optimization (10) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_baseline) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_regression) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_optimize) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_tune_cpu) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_tune_gpu) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_tune_mem) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_tune_io) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_tune_net) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_energy) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_perf_thermal) ||

			/* === Developer Workflow & Productivity (10) === */
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_hot_reload) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_live_patch) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_repl) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_snippets) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_templates) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_ci_integration) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_git_hooks) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_docker) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_vscode) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dev_clangd)) {
		PALERAIN_OPTION_CLEAR(palerain_flags, palerain_option_tui);
		PALERAIN_OPTION_SET(palerain_flags, palerain_option_cli);
	} else {
#ifdef ROOTFUL
		if ((!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_rootless)) && (!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_rootful))) {
			LOG(LOG_FATAL, "Please specify rootful (-f) or rootless (-l)");
			return -1;
		}
#else
		PALERAIN_OPTION_SET(palerain_flags, palerain_option_rootless);
#endif
	}
    
	snprintf(palerain_flags_cmd, 0x80, "palera1n_flags 0x%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64, palerain_flags[6], palerain_flags[5], palerain_flags[4], palerain_flags[3], palerain_flags[2], palerain_flags[1], palerain_flags[0]);
	LOG(LOG_VERBOSE3, "palerain_flags: %s", palerain_flags_cmd);
	if (override_kpf.magic == OVERRIDE_MAGIC) {
		LOG(LOG_VERBOSE4, "kpf override length %" PRIu32 " -> %" PRIu32, override_kpf.orig_len, checkra1n_kpf_pongo_lzma_len);
		LOG(LOG_VERBOSE4, "kpf override ptr %p -> %p", override_kpf.orig_ptr, **kpf_to_upload);
	}
	if (override_ramdisk.magic == OVERRIDE_MAGIC) {
		LOG(LOG_VERBOSE4, "ramdisk override length %" PRIu32 " -> %" PRIu32, override_ramdisk.orig_len, ramdisk_dmg_lzma_len);
		LOG(LOG_VERBOSE4, "ramdisk override ptr %p -> %p", override_ramdisk.orig_ptr, **ramdisk_to_upload);
	}
	if (override_overlay.magic == OVERRIDE_MAGIC) {
		LOG(LOG_VERBOSE4, "overlay override length %" PRIu32 " -> %" PRIu32, override_overlay.orig_len, binpack_dmg_len);
		LOG(LOG_VERBOSE4, "overlay override ptr %p -> %p", override_overlay.orig_ptr, **overlay_to_upload);
	}
#ifdef ROOTFUL
	if (!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_rootful)) {
		if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_setup_rootful)) {
			LOG(LOG_FATAL, "Cannot setup rootful when rootless is requested. Use -f to enable rootful mode.");
			return -1;
		}
	}
#endif
	if (!(
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_dfuhelper_only) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_enter_recovery) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exit_recovery) ||
			PALERAIN_OPTION_TEST(palerain_flags, palerain_option_reboot_device)))
	{
#ifdef NO_CHECKRAIN
		if (checkra1n_len == 0 && ext_checkra1n == NULL)
		{
			LOG(LOG_FATAL, "checkra1n omitted in build but no override specified");
			return -1;
		}
		if (!(PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_exit) || PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pongo_exit)))
		{
#ifdef NO_KPF
			if (checkra1n_kpf_pongo_lzma_len == 0)
			{
				LOG(LOG_FATAL, "kernel patchfinder omitted in build but no override specified");
				return -1;
			}
#endif
		}
#endif

#ifdef NO_EMBED_HELPER
	if (libcheckra1nhelper_dylib_len == 0 && gOverrideLibcheckra1nHelper == NULL) {
			LOG(LOG_FATAL, "checkra1n helper omitted in build but no override specified");
			return -1;
	}
#endif
	}

	for (index = optind; index < argc; index++)
	{
		if (!strcmp("windows", argv[index]))
		{
			fprintf(stderr,
					"Windows not really using for manipulating OSX images,\n"
					"compiled in mingw tool for this working unstable and incorrectly\n");
			return -2;
		}
		else
		{
			fprintf(stderr, "%s: unknown argument: %s\n", argv[0], argv[index]);
			usage(1, argv[0]);
		}
	}
	if (verbose >= 2) setenv("LIBUSB_DEBUG", "1", 1);

	if (verbose >= 3)
	{
		libusbmuxd_set_debug_level(verbose - 2);
		irecv_set_debug_level(1);
		setenv("LIBUSB_DEBUG", "2", 1);
	}
	if (verbose >= 4) {
		idevice_set_debug_level(1);
		setenv("LIBUSB_DEBUG", "3", 1);
	}
	if (verbose >= 5)
		setenv("LIBUSB_DEBUG", "4", 1);
    return 0;
}
