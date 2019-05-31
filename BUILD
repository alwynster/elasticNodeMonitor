load(
    "@AvrToolchain//:helpers.bzl",
    "cpu_frequency_flag",
    "create_unity_library",
    "default_embedded_binary",
    "generate_hex",
    "mcu_avr_gcc_flag",
)
load(
    "@EmbeddedSystemsBuildScripts//:unity.bzl",
    "generate_test_runner",
    "unity_test",
	"generate_a_unity_test_for_every_file"
)

commonCopts = mcu_avr_gcc_flag() + cpu_frequency_flag()

LUFA_COPTS = [
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config",
    "-isystem external/LUFA/",
    # "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config/",
]

#default_embedded_binary(
#	name = "main",
#	srcs = [
#		"src/main.c"],
#	copts = elasticNodeCopts,
#	linkopts = mcu_avr_gcc_flag() + ["-Wl,-u,vfprintf -lprintf_flt -lm"],
#	deps = [
#		"//lib:DebugLib",
#		"//lib:FpgaLibs",
#		"//lib:LedLibs",
#		"//lib:FlashLib",
#		"//lib:XorLib",
#		"//lib:ControlLib",
#        "//lib:SelectmapLib",
#		"//lib:CurrentSenseLib",
#
#		"@CommunicationModule",
#	] + select({
#        "@AvrToolchain//config:ElasticNode4": [],
#        "@AvrToolchain//config:ElasticNode3": [
#			"//lib:SoftSerialLib",
#			"//lib:SoftSpiLib"
#			]
#    }),
#    uploader = "//:stk_upload_script"
#)

default_embedded_binary(
	name = "demo",
	srcs = [
		"src/demoICAC.c"],
	copts = commonCopts + LUFA_COPTS,
	linkopts = commonCopts + LUFA_COPTS + ["-Wl,-u,vfprintf -lprintf_flt -lm"],
	deps = [
		"//lib:DebugLib",
        "//lib:CurrentSenseLib"
	],
    uploader = "//:stk_upload_script"
)


# Uploading stuff
load("//:vars.bzl", "defineProgrammer")

defineProgrammer(
  name = "programmerPort",
  port = "/dev/tty.usbmodem142101"
)

STK_UPLOAD_SCRIPT_TEMPLATE = """
{sudo}avrdude -v -V -p $$1 -c stk500v2 -P $(PROGPORT) -U flash:w:$$2 -e;
"""

genrule(
    name = "stk_upload_script",
    outs = ["stk_upload_script.sh"],
    cmd = "echo '" +
#    select({
#        "@AvrToolchain//host_config:dfu_needs_sudo": UPLOAD_SCRIPT_TEMPLATE.format(
#            export = "",
#            sudo = "sudo ",
#        ),
#        "@AvrToolchain//host_config:dfu_needs_askpass": UPLOAD_SCRIPT_TEMPLATE.format(
#            export = "export SUDO_ASKPASS=$(ASKPASS)",
#            sudo = "sudo ",
#        ),
#        "//conditions:default":
        STK_UPLOAD_SCRIPT_TEMPLATE.format(
            sudo = "",
        )
#    })
     + "' > $@",
    toolchains = [":programmerPort"]
)