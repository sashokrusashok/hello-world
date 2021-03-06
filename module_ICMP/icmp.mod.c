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
	{ 0x3d689d2e, __VMLINUX_SYMBOL_STR(ip_route_output_flow) },
	{ 0xde7ca2a5, __VMLINUX_SYMBOL_STR(dev_get_by_name) },
	{ 0x26a7df77, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0x7d50a24, __VMLINUX_SYMBOL_STR(csum_partial) },
	{ 0x14893b8a, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x74a019d7, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "E12BEB8012E81820505C899");
