#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x7a0cf9e5, "module_layout" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x97ad189a, "device_destroy" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x7408ca98, "__register_chrdev" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xea147363, "printk" },
	{ 0xa8f59416, "gpio_direction_output" },
	{ 0x9b98d1c5, "device_create" },
	{ 0xe9ce8b95, "omap_ioremap" },
	{ 0x15331242, "omap_iounmap" },
	{ 0xfe990052, "gpio_free" },
	{ 0xb03e36e0, "class_destroy" },
	{ 0x543712b0, "__class_create" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

