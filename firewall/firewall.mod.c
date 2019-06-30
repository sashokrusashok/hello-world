#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x884142c7, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xe9278edd, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x4c91030a, __VMLINUX_SYMBOL_STR(class_remove_file_ns) },
	{ 0x514e8d, __VMLINUX_SYMBOL_STR(nf_unregister_hook) },
	{ 0xe8f11fa0, __VMLINUX_SYMBOL_STR(nf_register_hook) },
	{ 0xa853a99d, __VMLINUX_SYMBOL_STR(class_create_file_ns) },
	{ 0x4436b614, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xd0d8621b, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0xb6ed1e53, __VMLINUX_SYMBOL_STR(strncpy) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "64923FDBEDEFD1FABE4D932");
