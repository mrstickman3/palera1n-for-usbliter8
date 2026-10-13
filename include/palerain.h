#ifndef PALERAIN_H
#define PALERAIN_H

#if defined(__APPLE__)
#define NSIG	__DARWIN_NSIG
#endif

#include "xxd-embedded.h"
#include "paleinfo.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <usbmuxd.h>
#include <libirecovery.h>
#include <libimobiledevice/libimobiledevice.h>

#define LOG(loglevel, ...) p1_log(loglevel, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define CLEAR() printf("\33[2K\r");
#define palerain_option_case(x) (512 + x)
#define palerain_option_version 129
#define CMD_LEN_MAX 512
#define OVERRIDE_MAGIC 0xd803b376

#define palerain_option_case_version      palerain_option_case(0)
#define palerain_option_case_force_revert palerain_option_case(1)
#define palerain_option_case_libcheckra1nhelper_path palerain_option_case(2)
#define palerain_option_case_cli		  palerain_option_case(3)
#define palerain_option_case_pwned_dfu    palerain_option_case(4)
#define palerain_option_case_config_file  palerain_option_case(5)
#define palerain_option_case_device_list  palerain_option_case(6)
#define palerain_option_case_device_filter palerain_option_case(7)
#define palerain_option_case_backup_path  palerain_option_case(8)
#define palerain_option_case_restore_path palerain_option_case(9)
#define palerain_option_case_theme        palerain_option_case(10)
#define palerain_option_case_language     palerain_option_case(11)
#define palerain_option_case_script       palerain_option_case(12)
#define palerain_option_case_plugin       palerain_option_case(13)
#define palerain_option_case_benchmark    palerain_option_case(14)
#define palerain_option_case_persist_check palerain_option_case(15)
#define palerain_option_case_device_favorite palerain_option_case(16)
#define palerain_option_case_auto_update  palerain_option_case(17)
#define palerain_option_case_health_check palerain_option_case(18)
#define palerain_option_case_cache_clean  palerain_option_case(19)
#define palerain_option_case_log_rotate   palerain_option_case(20)
#define palerain_option_case_disk_cleanup palerain_option_case(21)
#define palerain_option_case_version_check palerain_option_case(22)
#define palerain_option_case_dep_manager  palerain_option_case(23)
#define palerain_option_case_wizard       palerain_option_case(24)
#define palerain_option_case_batch        palerain_option_case(25)
#define palerain_option_case_schedule     palerain_option_case(26)
#define palerain_option_case_webhook      palerain_option_case(27)
#define palerain_option_case_api_server   palerain_option_case(28)
#define palerain_option_case_debug_console palerain_option_case(29)
#define palerain_option_case_mem_inspect  palerain_option_case(30)
#define palerain_option_case_reg_viewer   palerain_option_case(31)
#define palerain_option_case_syscall_trace palerain_option_case(32)
#define palerain_option_case_kern_debug   palerain_option_case(33)
#define palerain_option_case_perf_profile palerain_option_case(34)
#define palerain_option_case_crash_report palerain_option_case(35)
#define palerain_option_case_log_analyze  palerain_option_case(36)
#define palerain_option_case_multi_exploit palerain_option_case(37)
#define palerain_option_case_exploit_chain palerain_option_case(38)
#define palerain_option_case_custom_payload palerain_option_case(39)
#define palerain_option_case_boot_presets palerain_option_case(40)
#define palerain_option_case_kern_patches palerain_option_case(41)
#define palerain_option_case_ramdisk_cust palerain_option_case(42)
#define palerain_option_case_overlay_mgmt palerain_option_case(43)
#define palerain_option_case_verbose_boot_log palerain_option_case(44)
#define palerain_option_case_backup_restore palerain_option_case(45)
#define palerain_option_case_recovery_auto palerain_option_case(46)
#define palerain_option_case_dfu_auto     palerain_option_case(47)
#define palerain_option_case_safe_boot    palerain_option_case(48)
#define palerain_option_case_bootloop_detect palerain_option_case(49)
#define palerain_option_case_auto_recover palerain_option_case(50)
#define palerain_option_case_data_protect palerain_option_case(51)
#define palerain_option_case_emergency_exit palerain_option_case(52)
#define palerain_option_case_colored_themes palerain_option_case(53)
#define palerain_option_case_sound_notify palerain_option_case(54)
#define palerain_option_case_haptic_feedback palerain_option_case(55)
#define palerain_option_case_multi_lang   palerain_option_case(56)
#define palerain_option_case_accessibility palerain_option_case(57)
#define palerain_option_case_quick_actions palerain_option_case(58)
#define palerain_option_case_progress_anim palerain_option_case(59)
#define palerain_option_case_device_group palerain_option_case(60)
#define palerain_option_case_device_export palerain_option_case(61)
#define palerain_option_case_device_autodetect palerain_option_case(62)
#define palerain_option_case_device_rename palerain_option_case(63)
#define palerain_option_case_device_notes palerain_option_case(64)
#define palerain_option_case_ci_cd palerain_option_case(65)
#define palerain_option_case_config_template palerain_option_case(66)
#define palerain_option_case_health_monitor palerain_option_case(67)
#define palerain_option_case_device_info_detailed palerain_option_case(68)
#define palerain_option_case_device_tree palerain_option_case(69)
#define palerain_option_case_kernel_dump palerain_option_case(70)
#define palerain_option_case_usb_sniffer palerain_option_case(71)
#define palerain_option_case_boot_chain palerain_option_case(72)
#define palerain_option_case_firmware_keys palerain_option_case(73)
#define palerain_option_case_pongo_shell palerain_option_case(74)
#define palerain_option_case_recovery_test palerain_option_case(75)

/* === Ultra Verbose & Logging Features (10) === */
#define palerain_option_case_ultra_verbose       palerain_option_case(76)
#define palerain_option_case_trace_usb           palerain_option_case(77)
#define palerain_option_case_trace_memory        palerain_option_case(78)
#define palerain_option_case_trace_exploit       palerain_option_case(79)
#define palerain_option_case_trace_pongo         palerain_option_case(80)
#define palerain_option_case_trace_kpf           palerain_option_case(81)
#define palerain_option_case_trace_ramdisk       palerain_option_case(82)
#define palerain_option_case_trace_overlay       palerain_option_case(83)
#define palerain_option_case_trace_bootx         palerain_option_case(84)
#define palerain_option_case_log_all_stages      palerain_option_case(85)

/* === PongoOS Stage Diagnostics (10) === */
#define palerain_option_case_pongo_verbose       palerain_option_case(86)
#define palerain_option_case_pongo_cmd_log       palerain_option_case(87)
#define palerain_option_case_pongo_response_log  palerain_option_case(88)
#define palerain_option_case_pongo_mem_map       palerain_option_case(89)
#define palerain_option_case_pongo_reg_dump      palerain_option_case(90)
#define palerain_option_case_pongo_usb_log       palerain_option_case(91)
#define palerain_option_case_pongo_exception_log palerain_option_case(92)
#define palerain_option_case_pongo_task_list     palerain_option_case(93)
#define palerain_option_case_pongo_heap_dump     palerain_option_case(94)
#define palerain_option_case_pongo_symbols       palerain_option_case(95)

/* === Exploit Stage Diagnostics (10) === */
#define palerain_option_case_exploit_verbose     palerain_option_case(96)
#define palerain_option_case_exploit_step_trace  palerain_option_case(97)
#define palerain_option_case_exploit_mem_trace   palerain_option_case(98)
#define palerain_option_case_exploit_reg_trace   palerain_option_case(99)
#define palerain_option_case_exploit_timing      palerain_option_case(100)
#define palerain_option_case_exploit_retry_log   palerain_option_case(101)
#define palerain_option_case_exploit_payload_log palerain_option_case(102)
#define palerain_option_case_exploit_state_dump  palerain_option_case(103)
#define palerain_option_case_exploit_side_effects palerain_option_case(104)
#define palerain_option_case_exploit_validation  palerain_option_case(105)

/* === Kernel & Memory Diagnostics (10) === */
#define palerain_option_case_kernel_verbose      palerain_option_case(106)
#define palerain_option_case_kernel_mem_map      palerain_option_case(107)
#define palerain_option_case_kernel_slide_log    palerain_option_case(108)
#define palerain_option_case_kernel_patch_log    palerain_option_case(109)
#define palerain_option_case_kernel_symbols      palerain_option_case(110)
#define palerain_option_case_kernel_stack_trace  palerain_option_case(111)
#define palerain_option_case_kernel_page_track   palerain_option_case(112)
#define palerain_option_case_kernel_vm_log       palerain_option_case(113)
#define palerain_option_case_kernel_kalloc_log   palerain_option_case(114)
#define palerain_option_case_kernel_zone_log     palerain_option_case(115)

/* === USB & Communication Diagnostics (10) === */
#define palerain_option_case_usb_verbose         palerain_option_case(116)
#define palerain_option_case_usb_control_trace   palerain_option_case(117)
#define palerain_option_case_usb_bulk_trace      palerain_option_case(118)
#define palerain_option_case_usb_iso_trace       palerain_option_case(119)
#define palerain_option_case_usb_device_tree     palerain_option_case(120)
#define palerain_option_case_usb_endpoint_log    palerain_option_case(121)
#define palerain_option_case_usb_speed_log       palerain_option_case(122)
#define palerain_option_case_usb_power_log       palerain_option_case(123)
#define palerain_option_case_usb_reset_trace     palerain_option_case(124)
#define palerain_option_case_usb_hub_log         palerain_option_case(125)

/* === Device Tree & Firmware Analysis (10) === */
#define palerain_option_case_dtree_full_dump     palerain_option_case(126)
#define palerain_option_case_dtree_compare       palerain_option_case(127)
#define palerain_option_case_dtree_property_log  palerain_option_case(128)
#define palerain_option_case_dtree_compatible    palerain_option_case(129)
#define palerain_option_case_dtree_reg_dump      palerain_option_case(130)
#define palerain_option_case_dtree_interrupts    palerain_option_case(131)
#define palerain_option_case_dtree_clocks        palerain_option_case(132)
#define palerain_option_case_dtree_power         palerain_option_case(133)
#define palerain_option_case_dtree_gpio          palerain_option_case(134)
#define palerain_option_case_firmware_analyze    palerain_option_case(135)

/* === Security & Cryptography Features (10) === */
#define palerain_option_case_aes_engine_trace    palerain_option_case(136)
#define palerain_option_case_sep_comm_log        palerain_option_case(137)
#define palerain_option_case_sep_firmware_dump   palerain_option_case(138)
#define palerain_option_case_sep_key_trace       palerain_option_case(139)
#define palerain_option_case_gid_uid_trace       palerain_option_case(140)
#define palerain_option_case_firmware_decrypt    palerain_option_case(141)
#define palerain_option_case_img4_parse_log      palerain_option_case(142)
#define palerain_option_case_ap_ticket_log       palerain_option_case(143)
#define palerain_option_case_sep_ticket_log      palerain_option_case(144)
#define palerain_option_case_keybag_dump         palerain_option_case(145)

/* === Advanced Jailbreak Features (10) === */
#define palerain_option_case_rootless_verbose    palerain_option_case(146)
#define palerain_option_case_fakefs_verbose      palerain_option_case(147)
#define palerain_option_case_bind_mount_log      palerain_option_case(148)
#define palerain_option_case_overlay_verbose     palerain_option_case(149)
#define palerain_option_case_sandbox_log         palerain_option_case(150)
#define palerain_option_case_amfi_log            palerain_option_case(151)
#define palerain_option_case_codesign_log        palerain_option_case(152)
#define palerain_option_case_entitlement_log     palerain_option_case(153)
#define palerain_option_case_sip_log             palerain_option_case(154)
#define palerain_option_case_tfp0_log            palerain_option_case(155)

/* === Automation & Testing Features (10) === */
#define palerain_option_case_auto_retry          palerain_option_case(156)
#define palerain_option_case_max_retries         palerain_option_case(157)
#define palerain_option_case_test_mode           palerain_option_case(158)
#define palerain_option_case_stress_test         palerain_option_case(159)
#define palerain_option_case_fuzz_usb            palerain_option_case(160)
#define palerain_option_case_fuzz_exploit        palerain_option_case(161)
#define palerain_option_case_fuzz_pongo          palerain_option_case(162)
#define palerain_option_case_ci_mode             palerain_option_case(163)
#define palerain_option_case_benchmark_all       palerain_option_case(164)
#define palerain_option_case_perf_report         palerain_option_case(165)

/* === Developer & Research Features (10) === */
#define palerain_option_case_research_mode       palerain_option_case(166)
#define palerain_option_case_debug_breakpoints   palerain_option_case(167)
#define palerain_option_case_memory_watchpoints  palerain_option_case(168)
#define palerain_option_case_instruction_trace   palerain_option_case(169)
#define palerain_option_case_coverage_tracking   palerain_option_case(170)
#define palerain_option_case_custom_hooks        palerain_option_case(171)
#define palerain_option_case_plugin_debug        palerain_option_case(172)
#define palerain_option_case_api_debug           palerain_option_case(173)
#define palerain_option_case_script_debug        palerain_option_case(174)
#define palerain_option_case_experimental        palerain_option_case(175)

/* === Phase 2: Advanced Diagnostics & Analysis (100 features, bits 206-305) === */

/* === Hardware & Peripheral Diagnostics (10) === */
#define palerain_option_case_gpio_trace          palerain_option_case(176)
#define palerain_option_case_i2c_trace           palerain_option_case(177)
#define palerain_option_case_spi_trace           palerain_option_case(178)
#define palerain_option_case_uart_trace          palerain_option_case(179)
#define palerain_option_case_pmic_trace          palerain_option_case(180)
#define palerain_option_case_display_trace       palerain_option_case(181)
#define palerain_option_case_touch_trace         palerain_option_case(182)
#define palerain_option_case_audio_trace         palerain_option_case(183)
#define palerain_option_case_camera_trace        palerain_option_case(184)
#define palerain_option_case_sensor_trace        palerain_option_case(185)

/* === Power & Thermal Management (10) === */
#define palerain_option_case_power_domains       palerain_option_case(186)
#define palerain_option_case_thermal_zones       palerain_option_case(187)
#define palerain_option_case_battery_log         palerain_option_case(188)
#define palerain_option_case_charging_log        palerain_option_case(189)
#define palerain_option_case_dvfs_log            palerain_option_case(190)
#define palerain_option_case_idle_states         palerain_option_case(191)
#define palerain_option_case_suspend_log         palerain_option_case(192)
#define palerain_option_case_wakeup_sources      palerain_option_case(193)
#define palerain_option_case_power_stats         palerain_option_case(194)
#define palerain_option_case_energy_model        palerain_option_case(195)

/* === Filesystem & Storage Diagnostics (10) === */
#define palerain_option_case_apfs_verbose        palerain_option_case(196)
#define palerain_option_case_apfs_snapshots      palerain_option_case(197)
#define palerain_option_case_apfs_clones         palerain_option_case(198)
#define palerain_option_case_apfs_encryption     palerain_option_case(199)
#define palerain_option_case_nand_trace          palerain_option_case(200)
#define palerain_option_case_nvme_trace          palerain_option_case(201)
#define palerain_option_case_storage_stats       palerain_option_case(202)
#define palerain_option_case_trim_log            palerain_option_case(203)
#define palerain_option_case_wear_leveling       palerain_option_case(204)
#define palerain_option_case_bad_block_log       palerain_option_case(205)

/* === Network & Connectivity Diagnostics (10) === */
#define palerain_option_case_wifi_trace          palerain_option_case(206)
#define palerain_option_case_bluetooth_trace     palerain_option_case(207)
#define palerain_option_case_cellular_trace      palerain_option_case(208)
#define palerain_option_case_nfc_trace           palerain_option_case(209)
#define palerain_option_case_tcpip_trace         palerain_option_case(210)
#define palerain_option_case_dns_trace           palerain_option_case(211)
#define palerain_option_case_tls_trace           palerain_option_case(212)
#define palerain_option_case_vpn_trace           palerain_option_case(213)
#define palerain_option_case_airdrop_trace       palerain_option_case(214)
#define palerain_option_case_continuity_trace    palerain_option_case(215)

/* === Graphics & Display Diagnostics (10) === */
#define palerain_option_case_gpu_trace           palerain_option_case(216)
#define palerain_option_case_metal_trace         palerain_option_case(217)
#define palerain_option_case_iosurface_trace     palerain_option_case(218)
#define palerain_option_case_cvms_trace          palerain_option_case(219)
#define palerain_option_case_compositor_trace    palerain_option_case(220)
#define palerain_option_case_display_link        palerain_option_case(221)
#define palerain_option_case_vsync_trace         palerain_option_case(222)
#define palerain_option_case_layer_trace         palerain_option_case(223)
#define palerain_option_case_color_trace         palerain_option_case(224)
#define palerain_option_case_hdr_trace           palerain_option_case(225)

/* === Security & Enclave Diagnostics (10) === */
#define palerain_option_case_sep_verbose         palerain_option_case(226)
#define palerain_option_case_sep_mailbox         palerain_option_case(227)
#define palerain_option_case_sep_memory          palerain_option_case(228)
#define palerain_option_case_sep_firmware_log    palerain_option_case(229)
#define palerain_option_case_tz_verbose          palerain_option_case(230)
#define palerain_option_case_tz_memory           palerain_option_case(231)
#define palerain_option_case_tz_smc_trace        palerain_option_case(232)
#define palerain_option_case_secure_boot_log     palerain_option_case(233)
#define palerain_option_case_keybag_trace        palerain_option_case(234)
#define palerain_option_case_entanglement_log    palerain_option_case(235)

/* === Memory Management Diagnostics (10) === */
#define palerain_option_case_vm_map_dump         palerain_option_case(236)
#define palerain_option_case_vm_fault_trace      palerain_option_case(237)
#define palerain_option_case_physmem_log         palerain_option_case(238)
#define palerain_option_case_page_reclaim        palerain_option_case(239)
#define palerain_option_case_compression_log     palerain_option_case(240)
#define palerain_option_case_swap_trace          palerain_option_case(241)
#define palerain_option_case_jemalloc_trace      palerain_option_case(242)
#define palerain_option_case_zone_trace          palerain_option_case(243)
#define palerain_option_case_vm_pressure         palerain_option_case(244)
#define palerain_option_case_memory_tags         palerain_option_case(245)

/* === Scheduler & Process Diagnostics (10) === */
#define palerain_option_case_sched_trace         palerain_option_case(246)
#define palerain_option_case_thread_trace        palerain_option_case(247)
#define palerain_option_case_workqueue_trace     palerain_option_case(248)
#define palerain_option_case_timer_trace         palerain_option_case(249)
#define palerain_option_case_ipc_trace           palerain_option_case(250)
#define palerain_option_case_mach_trace          palerain_option_case(251)
#define palerain_option_case_exception_trace     palerain_option_case(252)
#define palerain_option_case_signal_trace        palerain_option_case(253)
#define palerain_option_case_proc_trace          palerain_option_case(254)
#define palerain_option_case_task_trace          palerain_option_case(255)

/* === IOKit & Driver Diagnostics (10) === */
#define palerain_option_case_ioregistry_dump     palerain_option_case(256)
#define palerain_option_case_iokit_trace         palerain_option_case(257)
#define palerain_option_case_driver_match        palerain_option_case(258)
#define palerain_option_case_property_trace      palerain_option_case(259)
#define palerain_option_case_service_trace       palerain_option_case(260)
#define palerain_option_case_notification_trace  palerain_option_case(261)
#define palerain_option_case_powerman_trace      palerain_option_case(262)
#define palerain_option_case_thermal_trace       palerain_option_case(263)
#define palerain_option_case_pci_trace           palerain_option_case(264)
#define palerain_option_case_thunderbolt_trace   palerain_option_case(265)

/* === Boot & Startup Diagnostics (10) === */
#define palerain_option_case_iboot_verbose       palerain_option_case(266)
#define palerain_option_case_iboot_env           palerain_option_case(267)
#define palerain_option_case_iboot_commands      palerain_option_case(268)
#define palerain_option_case_diags_log           palerain_option_case(269)
#define palerain_option_case_recovery_log        palerain_option_case(270)
#define palerain_option_case_dfu_log             palerain_option_case(271)
#define palerain_option_case_ramdisk_log         palerain_option_case(272)
#define palerain_option_case_kernelcache_log     palerain_option_case(273)
#define palerain_option_case_prelink_log         palerain_option_case(274)
#define palerain_option_case_boot_progress       palerain_option_case(275)

/* === PongoOS Stage Diagnostics (20) === */
#define palerain_option_case_pongo_stage_verbose palerain_option_case(276)
#define palerain_option_case_pongo_stage_log     palerain_option_case(277)
#define palerain_option_case_pongo_mem_log       palerain_option_case(278)
#define palerain_option_case_pongo_heap_log      palerain_option_case(279)
#define palerain_option_case_pongo_task_log      palerain_option_case(280)
#define palerain_option_case_pongo_irq_log       palerain_option_case(281)
#define palerain_option_case_pongo_timer_log     palerain_option_case(282)
#define palerain_option_case_pongo_uart_log      palerain_option_case(283)
#define palerain_option_case_pongo_usb_event_log palerain_option_case(284)
#define palerain_option_case_pongo_wdt_log       palerain_option_case(285)
#define palerain_option_case_pongo_crash_log     palerain_option_case(286)
#define palerain_option_case_pongo_syscall_log   palerain_option_case(287)
#define palerain_option_case_pongo_ipc_log       palerain_option_case(288)
#define palerain_option_case_pongo_sem_log       palerain_option_case(289)
#define palerain_option_case_pongo_mutex_log     palerain_option_case(290)
#define palerain_option_case_pongo_cond_log      palerain_option_case(291)
#define palerain_option_case_pongo_queue_log     palerain_option_case(292)
#define palerain_option_case_pongo_pool_log      palerain_option_case(293)
#define palerain_option_case_pongo_dma_log       palerain_option_case(294)
#define palerain_option_case_pongo_pmu_log       palerain_option_case(295)

/* === Kernel Exploitation Diagnostics (10) === */
#define palerain_option_case_exploit_diag_verbose palerain_option_case(296)
#define palerain_option_case_exploit_stage_log   palerain_option_case(297)
#define palerain_option_case_exploit_heap_log    palerain_option_case(298)
#define palerain_option_case_exploit_rop_log     palerain_option_case(299)
#define palerain_option_case_exploit_kcall_log   palerain_option_case(300)
#define palerain_option_case_exploit_kread_log   palerain_option_case(301)
#define palerain_option_case_exploit_kwrite_log  palerain_option_case(302)
#define palerain_option_case_exploit_pac_log     palerain_option_case(303)
#define palerain_option_case_exploit_pp_log      palerain_option_case(304)
#define palerain_option_case_exploit_slide_log   palerain_option_case(305)

/* === Checkra1n Integration Diagnostics (10) === */
#define palerain_option_case_checkra1n_verbose   palerain_option_case(306)
#define palerain_option_case_checkra1n_stage_log palerain_option_case(307)
#define palerain_option_case_checkra1n_pongo_log palerain_option_case(308)
#define palerain_option_case_checkra1n_boot_log  palerain_option_case(309)
#define palerain_option_case_checkra1n_ramdisk_log palerain_option_case(310)
#define palerain_option_case_checkra1n_kernel_log palerain_option_case(311)
#define palerain_option_case_checkra1n_hook_log  palerain_option_case(312)
#define palerain_option_case_checkra1n_shell_log palerain_option_case(313)
#define palerain_option_case_checkra1n_ssh_log   palerain_option_case(314)
#define palerain_option_case_checkra1n_cleanup_log palerain_option_case(315)

/* === Advanced USB Diagnostics (10) === */
#define palerain_option_case_usb_diag_verbose    palerain_option_case(316)
#define palerain_option_case_usb_ep_diag_log     palerain_option_case(317)
#define palerain_option_case_usb_transfer_log    palerain_option_case(318)
#define palerain_option_case_usb_control_log     palerain_option_case(319)
#define palerain_option_case_usb_bulk_log        palerain_option_case(320)
#define palerain_option_case_usb_interrupt_log   palerain_option_case(321)
#define palerain_option_case_usb_isochronous_log palerain_option_case(322)
#define palerain_option_case_usb_device_log      palerain_option_case(323)
#define palerain_option_case_usb_interface_log   palerain_option_case(324)
#define palerain_option_case_usb_config_log      palerain_option_case(325)

/* === DFU Mode Diagnostics (10) === */
#define palerain_option_case_dfu_verbose         palerain_option_case(326)
#define palerain_option_case_dfu_state_log       palerain_option_case(327)
#define palerain_option_case_dfu_command_log     palerain_option_case(328)
#define palerain_option_case_dfu_upload_log      palerain_option_case(329)
#define palerain_option_case_dfu_download_log    palerain_option_case(330)
#define palerain_option_case_dfu_getstatus_log   palerain_option_case(331)
#define palerain_option_case_dfu_clrstatus_log   palerain_option_case(332)
#define palerain_option_case_dfu_abort_log       palerain_option_case(333)
#define palerain_option_case_dfu_detach_log      palerain_option_case(334)
#define palerain_option_case_dfu_reset_log       palerain_option_case(335)

/* === Recovery Mode Diagnostics (10) === */
#define palerain_option_case_recovery_verbose    palerain_option_case(336)
#define palerain_option_case_recovery_state_log  palerain_option_case(337)
#define palerain_option_case_recovery_command_log palerain_option_case(338)
#define palerain_option_case_recovery_upload_log palerain_option_case(339)
#define palerain_option_case_recovery_download_log palerain_option_case(340)
#define palerain_option_case_recovery_nonce_log  palerain_option_case(341)
#define palerain_option_case_recovery_apnonce_log palerain_option_case(342)
#define palerain_option_case_recovery_sepnonce_log palerain_option_case(343)
#define palerain_option_case_recovery_ticket_log palerain_option_case(344)
#define palerain_option_case_recovery_reset_log  palerain_option_case(345)

/* === Bootloader Diagnostics (10) === */
#define palerain_option_case_bootloader_verbose  palerain_option_case(346)
#define palerain_option_case_bootloader_stage_log palerain_option_case(347)
#define palerain_option_case_bootloader_iboot_log palerain_option_case(348)
#define palerain_option_case_bootloader_ibec_log palerain_option_case(349)
#define palerain_option_case_bootloader_ibss_log palerain_option_case(350)
#define palerain_option_case_bootloader_lowlevel_log palerain_option_case(351)
#define palerain_option_case_bootloader_mem_log  palerain_option_case(352)
#define palerain_option_case_bootloader_reg_log  palerain_option_case(353)
#define palerain_option_case_bootloader_dev_log  palerain_option_case(354)
#define palerain_option_case_bootloader_cmd_log  palerain_option_case(355)

/* === Filesystem Diagnostics (10) === */
#define palerain_option_case_fs_verbose          palerain_option_case(356)
#define palerain_option_case_fs_mount_log        palerain_option_case(357)
#define palerain_option_case_fs_unmount_log      palerain_option_case(358)
#define palerain_option_case_fs_read_log         palerain_option_case(359)
#define palerain_option_case_fs_write_log        palerain_option_case(360)
#define palerain_option_case_fs_create_log       palerain_option_case(361)
#define palerain_option_case_fs_delete_log       palerain_option_case(362)
#define palerain_option_case_fs_rename_log       palerain_option_case(363)
#define palerain_option_case_fs_stat_log         palerain_option_case(364)
#define palerain_option_case_fs_xattr_log        palerain_option_case(365)

/* === Network Diagnostics (10) === */
#define palerain_option_case_net_verbose         palerain_option_case(366)
#define palerain_option_case_net_packet_log      palerain_option_case(367)
#define palerain_option_case_net_socket_log      palerain_option_case(368)
#define palerain_option_case_net_route_log       palerain_option_case(369)
#define palerain_option_case_net_arp_log         palerain_option_case(370)
#define palerain_option_case_net_ndp_log         palerain_option_case(371)
#define palerain_option_case_net_dhcp_log        palerain_option_case(372)
#define palerain_option_case_net_dns_log         palerain_option_case(373)
#define palerain_option_case_net_tls_log         palerain_option_case(374)
#define palerain_option_case_net_http_log        palerain_option_case(375)

/* === Phase 4: Advanced Developer Tools & Debugging (100 features, bits 407-506) === */

/* === Kernel Development & Debugging (15) === */
#define palerain_option_case_kern_debug_enter    palerain_option_case(376)
#define palerain_option_case_kern_debug_wait     palerain_option_case(377)
#define palerain_option_case_kern_break_early    palerain_option_case(378)
#define palerain_option_case_kern_watchdog_disable palerain_option_case(379)
#define palerain_option_case_kern_panic_log      palerain_option_case(380)
#define palerain_option_case_kern_backtrace_all  palerain_option_case(381)
#define palerain_option_case_kern_lock_stat      palerain_option_case(382)
#define palerain_option_case_kern_intr_stat      palerain_option_case(383)
#define palerain_option_case_kern_syscall_stat   palerain_option_case(384)
#define palerain_option_case_kern_vm_stat        palerain_option_case(385)
#define palerain_option_case_kern_task_stat      palerain_option_case(386)
#define palerain_option_case_kern_zone_stat      palerain_option_case(387)
#define palerain_option_case_kern_kalloc_stat    palerain_option_case(388)
#define palerain_option_case_kern_upl_stat       palerain_option_case(389)
#define palerain_option_case_kern_io_stat        palerain_option_case(390)

/* === Dynamic Analysis & Instrumentation (15) === */
#define palerain_option_case_dtrace_enable       palerain_option_case(391)
#define palerain_option_case_ktrace_enable       palerain_option_case(392)
#define palerain_option_case_ftrace_enable       palerain_option_case(393)
#define palerain_option_case_perf_counters       palerain_option_case(394)
#define palerain_option_case_branch_trace        palerain_option_case(395)
#define palerain_option_case_call_graph          palerain_option_case(396)
#define palerain_option_case_hot_path_detect     palerain_option_case(397)
#define palerain_option_case_mem_leak_detect     palerain_option_case(398)
#define palerain_option_case_use_after_free      palerain_option_case(399)
#define palerain_option_case_double_free_detect  palerain_option_case(400)
#define palerain_option_case_buffer_overflow     palerain_option_case(401)
#define palerain_option_case_race_detect         palerain_option_case(402)
#define palerain_option_case_deadlock_detect     palerain_option_case(403)
#define palerain_option_case_stack_canary_check  palerain_option_case(404)
#define palerain_option_case_shadow_stack        palerain_option_case(405)

/* === Fuzzing & Testing Infrastructure (10) === */
#define palerain_option_case_fuzz_kernel         palerain_option_case(406)
#define palerain_option_case_fuzz_driver         palerain_option_case(407)
#define palerain_option_case_fuzz_filesystem     palerain_option_case(408)
#define palerain_option_case_fuzz_network        palerain_option_case(409)
#define palerain_option_case_fuzz_usb_stack      palerain_option_case(410)
#define palerain_option_case_fuzz_ipc            palerain_option_case(411)
#define palerain_option_case_fuzz_mach           palerain_option_case(412)
#define palerain_option_case_property_test       palerain_option_case(413)
#define palerain_option_case_model_check         palerain_option_case(414)
#define palerain_option_case_symbolic_exec       palerain_option_case(415)

/* === Reverse Engineering & Analysis (10) === */
#define palerain_option_case_re_disassemble      palerain_option_case(416)
#define palerain_option_case_re_cfg_build        palerain_option_case(417)
#define palerain_option_case_re_data_flow        palerain_option_case(418)
#define palerain_option_case_re_type_recovery    palerain_option_case(419)
#define palerain_option_case_re_struct_recovery  palerain_option_case(420)
#define palerain_option_case_re_string_analysis  palerain_option_case(421)
#define palerain_option_case_re_xref_find        palerain_option_case(422)
#define palerain_option_case_re_pattern_match    palerain_option_case(423)
#define palerain_option_case_re_signature_gen    palerain_option_case(424)
#define palerain_option_case_re_diff_binary      palerain_option_case(425)

/* === Exploit Development (10) === */
#define palerain_option_case_exploit_dev_mode    palerain_option_case(426)
#define palerain_option_case_exploit_payload_gen palerain_option_case(427)
#define palerain_option_case_exploit_rop_build   palerain_option_case(428)
#define palerain_option_case_exploit_jop_build   palerain_option_case(429)
#define palerain_option_case_exploit_srop_build  palerain_option_case(430)
#define palerain_option_case_exploit_pac_bypass  palerain_option_case(431)
#define palerain_option_case_exploit_pp_bypass   palerain_option_case(432)
#define palerain_option_case_exploit_kaslr_leak  palerain_option_case(433)
#define palerain_option_case_exploit_heap_feng   palerain_option_case(434)
#define palerain_option_case_exploit_race_win    palerain_option_case(435)

/* === Hardware Bring-up & Validation (10) === */
#define palerain_option_case_hw_bringup          palerain_option_case(436)
#define palerain_option_case_hw_memory_test      palerain_option_case(437)
#define palerain_option_case_hw_cache_test       palerain_option_case(438)
#define palerain_option_case_hw_interconnect     palerain_option_case(439)
#define palerain_option_case_hw_timer_test       palerain_option_case(440)
#define palerain_option_case_hw_interrupt_test   palerain_option_case(441)
#define palerain_option_case_hw_dma_test         palerain_option_case(442)
#define palerain_option_case_hw_pmu_test         palerain_option_case(443)
#define palerain_option_case_hw_thermal_test     palerain_option_case(444)
#define palerain_option_case_hw_power_test       palerain_option_case(445)

/* === Firmware & Bootloader Development (10) === */
#define palerain_option_case_fw_dev_mode         palerain_option_case(446)
#define palerain_option_case_fw_patch_test       palerain_option_case(447)
#define palerain_option_case_fw_verify_chain     palerain_option_case(448)
#define palerain_option_case_fw_key_test         palerain_option_case(449)
#define palerain_option_case_fw_rollback_test    palerain_option_case(450)
#define palerain_option_case_fw_dfu_test         palerain_option_case(451)
#define palerain_option_case_fw_recovery_test    palerain_option_case(452)
#define palerain_option_case_fw_ramdisk_test     palerain_option_case(453)
#define palerain_option_case_fw_overlay_test     palerain_option_case(454)
#define palerain_option_case_fw_integrity        palerain_option_case(455)

/* === Performance & Optimization (10) === */
#define palerain_option_case_perf_baseline       palerain_option_case(456)
#define palerain_option_case_perf_regression     palerain_option_case(457)
#define palerain_option_case_perf_optimize       palerain_option_case(458)
#define palerain_option_case_perf_tune_cpu       palerain_option_case(459)
#define palerain_option_case_perf_tune_gpu       palerain_option_case(460)
#define palerain_option_case_perf_tune_mem       palerain_option_case(461)
#define palerain_option_case_perf_tune_io        palerain_option_case(462)
#define palerain_option_case_perf_tune_net       palerain_option_case(463)
#define palerain_option_case_perf_energy         palerain_option_case(464)
#define palerain_option_case_perf_thermal        palerain_option_case(465)

/* === Developer Workflow & Productivity (10) === */
#define palerain_option_case_dev_hot_reload      palerain_option_case(466)
#define palerain_option_case_dev_live_patch      palerain_option_case(467)
#define palerain_option_case_dev_repl            palerain_option_case(468)
#define palerain_option_case_dev_snippets        palerain_option_case(469)
#define palerain_option_case_dev_templates       palerain_option_case(470)
#define palerain_option_case_dev_ci_integration  palerain_option_case(471)
#define palerain_option_case_dev_git_hooks       palerain_option_case(472)
#define palerain_option_case_dev_docker          palerain_option_case(473)
#define palerain_option_case_dev_vscode          palerain_option_case(474)
#define palerain_option_case_dev_clangd          palerain_option_case(475)


#ifdef USE_LIBUSB
#include <libusb-1.0/libusb.h>

#define USB_RET_SUCCESS         LIBUSB_SUCCESS
#define USB_RET_NOT_RESPONDING  LIBUSB_ERROR_OTHER
#define USB_RET_IO              LIBUSB_ERROR_IO
#define USB_RET_NO_DEVICE		LIBUSB_ERROR_NO_DEVICE
typedef int32_t usb_ret_t;
typedef libusb_device_handle *usb_device_handle_t;

typedef struct stuff
{
    pthread_t th;
    libusb_device *dev;
    usb_device_handle_t handle;
} stuff_t;
usb_ret_t USBBulkUpload(usb_device_handle_t handle, void *data, int len);
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#pragma clang diagnostic pop

#define USB_RET_SUCCESS         KERN_SUCCESS
#define USB_RET_NOT_RESPONDING  kIOReturnNotResponding
#define USB_RET_IO              kIOReturnNotReady
#define USB_RET_NO_DEVICE       kIOReturnNoDevice

typedef IOReturn usb_ret_t;
typedef IOUSBInterfaceInterface245 **usb_device_handle_t;

typedef struct
{
    pthread_t th;
    volatile uint64_t regID;
    IOUSBDeviceInterface245 **dev;
    usb_device_handle_t handle;
} stuff_t;
usb_ret_t USBBulkUpload(usb_device_handle_t handle, void *data, uint32_t len);
#endif

#ifndef PALERAIN_VERSION
#define PALERAIN_VERSION "2.2.1"
#endif

#if defined(__APPLE__)
#include <mach-o/loader.h>
#include <mach-o/ldsyms.h>
#else
#define MH_MAGIC_64 0xfeedfacf
#define MH_CIGAM_64 0xcffaedfe
#define MH_KEXT_BUNDLE 0xb

typedef int32_t cpu_type_t;
typedef int32_t cpu_subtype_t;

#define CPU_ARCH_ABI64          0x01000000
#define CPU_TYPE_ARM            ((cpu_type_t) 12)
#define CPU_TYPE_ARM64          (CPU_TYPE_ARM | CPU_ARCH_ABI64)

struct mach_header_64
{
	uint32_t magic;			  /* mach magic number identifier */
	cpu_type_t cputype;		  /* cpu specifier */
	cpu_subtype_t cpusubtype; /* machine specifier */
	uint32_t filetype;		  /* type of file */
	uint32_t ncmds;			  /* number of load commands */
	uint32_t sizeofcmds;	  /* the size of all the load commands */
	uint32_t flags;			  /* flags */
	uint32_t reserved;		  /* reserved */
};
#endif

typedef void *(*pthread_start_t)(void *);

// Keep in sync with Pongo
#define PONGO_USB_VENDOR    0x05ac
#define PONGO_USB_PRODUCT   0x4141
#define CMD_LEN_MAX         512
#define UPLOADSZ_MAX        (1024 * 1024 * 128)

typedef enum {
	LOG_FATAL = 0,
	LOG_ERROR = 1,
	LOG_WARNING = 2,
	LOG_INFO = 3,
	LOG_VERBOSE = 4,
	LOG_VERBOSE2 = 5,
	LOG_VERBOSE3 = 6,
	LOG_VERBOSE4 = 7,
	LOG_VERBOSE5 = 8,
} log_level_t;

typedef struct {
	uint64_t ecid;
	char* productType;
	char* productVersion;
	char* buildVersion;
	char* CPUArchitecture;
	const char* displayName;
} devinfo_t;

typedef struct {
	int mode;
	int cpid;
	char product_type[0x20];
	char display_name[0x20];
	char iboot_ver[0x20];
} recvinfo_t;

typedef struct {
	uint32_t magic; /* 0xd803b376*/
	unsigned char* ptr; /* pointer to the override file in memory */
	uint32_t len; /* length of override file */
	unsigned char* orig_ptr; /* pointer to the overriden file */
	uint32_t orig_len; /* length of the overriden file */
	int fd; /* file descriptor of the override file */
} override_file_t;

typedef unsigned char niarelap_file_t[];

extern unsigned int verbose;

extern char *simulator_chip;
extern int dry_run;
extern int simulator_fast;

extern char* pongo_path;
#ifdef TUI
extern bool tui_started;
#endif

extern palerain_option_t palerain_flags;
extern palerain_option_t* palerain_flags_p;

extern pthread_t dfuhelper_thread, pongo_thread;
extern bool pongo_thr_running, dfuhelper_thr_running;

extern niarelap_file_t *kpf_to_upload_1, *ramdisk_to_upload_1, *overlay_to_upload_1;
extern niarelap_file_t **kpf_to_upload, **ramdisk_to_upload, **overlay_to_upload;
extern override_file_t override_ramdisk, override_kpf, override_overlay;
extern pthread_mutex_t log_mutex;

extern pthread_mutex_t spin_mutex, found_pongo_mutex, ecid_dfu_wait_mutex;
extern pthread_mutex_t device_has_booted_mutex;
extern pthread_mutex_t pongo_thr_running_mutex;
extern pthread_mutex_t dfuhelper_thr_running_mutex;

extern bool device_has_booted;
extern char xargs_cmd[0x270];
extern char palerain_flags_cmd[0x80];
extern char* ext_checkra1n;

void thr_cleanup(void* ptr);
void* dfuhelper(void* ptr);
int p1_log(log_level_t loglevel, const char *fname, int lineno, const char *fxname, const char* __restrict format, ...);
/* devhelper helpers */
void devinfo_free(devinfo_t *dev);
bool cpid_is_arm64(unsigned int cpid);
bool cpid_needs_usbliter8(unsigned int cpid);
/* devhelper commands */
int subscribe_cmd(usbmuxd_event_cb_t device_event_cb, irecv_device_event_cb_t irecv_event_cb);
int unsubscribe_cmd(void);
int devinfo_cmd(devinfo_t *dev, const char *udid);
int enter_recovery_cmd(const char* udid);
int reboot_cmd(const char* udid);
int passstat_cmd(unsigned char* status, const char* udid);
int recvinfo_cmd(recvinfo_t* info, const uint64_t ecid);
int autoboot_cmd(const uint64_t ecid);
int exitrecv_cmd(const uint64_t ecid);

int exec_checkra1n(void);
int override_file(override_file_t *finfo, niarelap_file_t** orig, unsigned int *orig_len, char *filename);
void* pongo_helper(void* ptr);
const unsigned char *
boyermoore_horspool_memmem(const unsigned char* haystack, size_t hlen,
                           const unsigned char* needle,   size_t nlen);


usb_ret_t USBControlTransfer(usb_device_handle_t handle, uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint32_t wLength, void *data, uint32_t *wLenDone);
const char *usb_strerror(usb_ret_t err);
int wait_for_pongo(void);
int tui(void);
int optparse(int argc, char* argv[]);

bool get_spin(void);
bool set_spin(bool val);
bool get_found_pongo(void);
bool set_found_pongo(bool val);
uint64_t get_ecid_wait_for_dfu(void);
uint64_t set_ecid_wait_for_dfu(uint64_t ecid);

void io_start(stuff_t *stuff);
void io_stop(stuff_t *stuff);

void print_credits(void);

bool get_device_has_booted(void);
void set_device_has_booted(bool val);

bool get_pongo_thr_running(void);
void set_pongo_thr_running(bool val);
bool get_dfuhelper_thr_running(void);
void set_dfuhelper_thr_running(bool val);

int log_file_init(const char *path);
void log_file_cleanup(void);

void dfuhelper_request_cancel(void);
void pongo_request_cancel(void);

/* New feature declarations */
int enumerate_usb_devices(void);
int check_device_compatibility(uint64_t ecid);
int load_config_file(const char *path);
int save_config_file(const char *path);
const char* get_error_string(int error_code);
#define FORMAT_TEXT 0
#define FORMAT_XML 1

int print_device_info_detailed(uint64_t ecid, int format);
bool is_device_supported(struct irecv_device_info *info);
int wait_for_device_mode(uint64_t ecid, int target_mode, int timeout_ms);

/* New feature declarations */
int check_firmware_version(const char *product_version);
int enter_recovery_with_retry(const char *udid, int max_retries);
int get_device_build_info(uint64_t ecid, char **build_version, char **cpid_str);
bool device_needs_usbliter8(uint64_t ecid);
int ensure_device_in_dfu(uint64_t ecid);
void print_progress_bar(const char *label, size_t current, size_t total, int width);

/* Additional feature declarations */
int print_device_info_colored(uint64_t ecid);
int auto_configure_for_device(void);
int check_usb_connection(void);
void format_bytes(uint64_t bytes, char *buf, size_t buf_size);
void print_transfer_stats(size_t total_bytes, double elapsed_seconds);
int save_jailbreak_log(const char *path);
bool is_running_as_root(void);
int verify_dependencies(void);
char *create_session_temp_dir(void);
void cleanup_session_temp_dir(char *path);
void format_duration(int seconds, char *buf, size_t buf_size);
void print_timestamped_status(const char *status);

/* === Device Management Features (8) === */
int device_list_all(int format);
int device_filter_by_type(const char *type);
int device_group_by_ios_version(void);
int device_export_list(const char *path, int format);
int device_auto_detect(void);
int device_rename(uint64_t ecid, const char *new_name);
int device_add_note(uint64_t ecid, const char *note);
int device_toggle_favorite(uint64_t ecid);

/* === Safety & Recovery Features (8) === */
int backup_device_data(const char *backup_path);
int restore_device_data(const char *restore_path);
int recovery_mode_automate(const char *udid);
int dfu_mode_automate(const char *udid);
int safe_mode_boot(uint64_t ecid);
int bootloop_detect_and_recover(uint64_t ecid);
int auto_recovery_mode(void);
int emergency_exit_handler(void);

/* === Exploit & Boot Features (8) === */
int multi_exploit_support(int exploit_type);
int exploit_chain_execute(int chain_id);
int custom_payload_load(const char *payload_path);
int boot_argument_presets(int preset_id);
int kernel_patch_manage(int action, const char *patch_path);
int ramdisk_customize(const char *ramdisk_path);
int overlay_manage(int action, const char *overlay_path);
int verbose_boot_logging(int level);

/* === User Experience Features (8) === */
int interactive_wizard(void);
int progress_animation_start(const char *label);
int progress_animation_update(int percent);
int progress_animation_stop(void);
int theme_set(const char *theme_name);
int sound_notification_play(int sound_id);
int haptic_feedback_trigger(int pattern);
int language_set(const char *lang_code);
int accessibility_mode_enable(void);
int quick_action_register(const char *name, void (*callback)(void));

/* === Developer & Debug Features (8) === */
int debug_console_start(int port);
int memory_inspect(uint64_t address, size_t size);
int register_viewer(void);
int syscall_tracer_start(void);
int kernel_debugger_attach(void);
int performance_profiler_start(void);
int crash_reporter_generate(const char *output_path);
int log_analyzer_run(const char *log_path);

/* === Automation & Scripting Features (8) === */
int batch_operation_run(const char *config_path);
int scheduled_task_add(const char *cron_expr, const char *command);
int script_engine_execute(const char *script_path);
int plugin_system_load(const char *plugin_path);
int api_server_start(int port);
int webhook_register(const char *url, const char *event);
int ci_cd_integrate(const char *config_path);
int config_template_apply(const char *template_path);

/* === Maintenance & Utility Features (8) === */
int auto_update_check(void);
int auto_update_apply(void);
int self_diagnostics_run(void);
int cache_manage(int action);
int log_rotation_manage(int max_size_mb, int max_files);
int disk_cleanup_run(void);
int health_monitor_start(int interval_sec);
int version_checker_run(void);
int dependency_manager_check(void);

/* === Bonus Features (2) === */
int device_benchmark_run(uint64_t ecid);
int jailbreak_persistence_check(uint64_t ecid);

#endif
char* get_embedded_pongo_path(void);
