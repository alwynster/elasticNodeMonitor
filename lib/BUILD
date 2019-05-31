package(default_visibility = ["//visibility:public"])

load(
    "@AvrToolchain//:helpers.bzl",
    "cpu_frequency_flag",
    "mcu_avr_gcc_flag",
)

genericPinDefinitionFolder = "PinDefinitions/"

folderOpts = "-isystem " + " -isystem lib/".join(native.glob(["*"], exclude_directories=0))

filegroup(
	name = "ElasticNodePinsHeaders",
	srcs = select({
        "@AvrToolchain//config:ElasticNode4": glob([genericPinDefinitionFolder + "ElasticNode4/*.h"]),
        "@AvrToolchain//config:ElasticNode3": glob([genericPinDefinitionFolder + "ElasticNode3/*.h"]),
    })
)

commonCopts = mcu_avr_gcc_flag() + cpu_frequency_flag() # + [folderOpts] 

PinCopts = select({
        "@AvrToolchain//config:ElasticNode4": ["-Ilib/" + genericPinDefinitionFolder + "ElasticNode4"],
        "@AvrToolchain//config:ElasticNode3": ["-isystem lib/" + genericPinDefinitionFolder + "ElasticNode3"],
    })

elasticNodeCopts = [
    # "-DF_CPU=8000000UL", # 12M
    # "-DF_USB=8000000UL",
    # "-DBAUD=115200",
	# "-DARCH=ARCH_AVR8",
	# "-mmcu=$(MCU)",
	"-Wl,-u,vfprintf -lprintf_flt -lm",
	"-Os", "-std=c99"
	] + commonCopts + PinCopts

LUFA_COPTS = [
    "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config",
    "-isystem external/LUFA/",
    # "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config/",
]

exports_files(
	glob(["*/*.h"]),
	visibility = ["//visibility:public"],
)

# filegroups
filegroup(
	name = "UartSrcs",
	srcs = glob(["uart/*.c"])
)

filegroup(
	name = "UartHeaders",
	srcs = glob(["uart/*.h"])
)

filegroup(
	name = "LufaSetupSrcs",
	srcs = glob(["LUFA-Setup/*.c"])
)

filegroup(
	name = "LufaSetupHeaders",
	srcs = glob(["LUFA-Setup/*.h"])
)

# lib definitions
cc_library(
	name = "DebugLib",
	srcs = 
	select({
        "@AvrToolchain//config:ElasticNode4": ["debug/debugUsb.c", "LufaSetupSrcs"],
        "@AvrToolchain//config:ElasticNode3": ["debug/debugUart.c"],
    }),
	hdrs = glob(["debug/*.h"]) + 	
	select({
        "@AvrToolchain//config:ElasticNode4": ["LufaSetupHeaders"],
        "@AvrToolchain//config:ElasticNode3": [],
    }),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts + LUFA_COPTS,
	linkstatic = True,
	deps = select({
        "@AvrToolchain//config:ElasticNode4": ["@LUFA//:VirtualSerial"],
        "@AvrToolchain//config:ElasticNode3": ["UartLib"],
    }),
)

cc_library(
	name = "UartLib",
	srcs = ["UartSrcs"],
	hdrs = ["UartHeaders"],

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,

	deps = ["CircularBufferLib"]
)

cc_library(
	name = "FpgaFlashLib",
	srcs = select({
        "@AvrToolchain//config:ElasticNode4": ["fpgaFlash/fpgaFlashSpi.c"],
        "@AvrToolchain//config:ElasticNode3": ["fpgaFlash/fpgaFlashSoftSpi.c"],
    }),
	hdrs = glob(["fpgaFlash/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,

	deps = ["PinLib"] + select({
        "@AvrToolchain//config:ElasticNode4": ["SpiLib"],
        "@AvrToolchain//config:ElasticNode3": ["SoftSpiLib"],
    })
)


cc_library(
	name = "Stubs",
	srcs = glob(["stubs/**/*.c"]),
	hdrs = glob(["stubs/**/*.h"]),
	copts = elasticNodeCopts,
	deps = [
		"FpgaLibs", 
		"LedLibs", 
		"ExperimentLibs",
		"StubsLocal",
		"FpgaFlashLib"
		]+ select({
        "@AvrToolchain//config:ElasticNode4": [],
        "@AvrToolchain//config:ElasticNode3": []
		})
)

cc_library(
	name = "StubsLocal",
	srcs = glob(["stubsLocal/*.c"]),
	hdrs = glob(["stubsLocal/*.h"]),
#	copts = elasticNodeCopts,
	deps = ["@mcu-ann//:mcuAnnLib"],
    visibility = ["//visibility:public"]
)

cc_library(
	name = "TimerLib",
	srcs = glob(["timer/*.c"]),
	hdrs = glob(["timer/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["IoLibs", "DebugLib"]
)

cc_library(
	name = "ExperimentLibs",
	srcs = glob(["experiments/*.c", "transfer/*.c"]),
	hdrs = glob(["experiments/*.h", "transfer/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["LedLibs", "TimerLib", "FlashLib", "CircularBufferLib"]
)

cc_library(
	name = "ControlLib",
	srcs = glob(["control/*.c"]),
	hdrs = glob(["control/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = [
		"FpgaLibs", 
		"ExperimentLibs", 
		"FlashLib", 
		"LedLibs",
		"StubsLocal",
		"Stubs",
		"XorLib",
		"MultibootLib",
        "SelectmapLib"
	]
)



fpgaFolders = ["jtag", "fpga", "xmem"]
cc_library(
	name = "FpgaLibs",
	srcs = glob([folder + "/*.c" for folder in fpgaFolders]),
	hdrs = glob([folder + "/*.h" for folder in fpgaFolders]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	
	deps = ["IoLibs", "DebugLib", "PinLib"] +
	select({
        "@AvrToolchain//config:ElasticNode4": [],
        "@AvrToolchain//config:ElasticNode3": []
	})
)

cc_library(
	name = "PinLib",
	# srcs = glob([folder + "/*.c" for folder in fpgaFolders]),
	hdrs = [":ElasticNodePinsHeaders"],

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	
	# deps = ["IoLibs", "DebugLib"] + 
	# select({
    #     "@AvrToolchain//config:ElasticNode4": [],
    #     "@AvrToolchain//config:ElasticNode3": []
	# })
)

cc_library(
	name = "IoLibs",
	srcs = glob(["io/*.c"]),
	hdrs = glob(["io/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
)

cc_library(
	name = "MultibootLib",
	srcs = glob(["multiboot/*.c"]),
	hdrs = glob(["multiboot/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["FpgaLibs", "SpiArbitrationLib"]
)

cc_library(
	name = "SelectmapLib",
	srcs = glob(["selectmap/*.c"]),
	hdrs = glob(["selectmap/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["SpiArbitrationLib"]
)

cc_library(
	name = "CircularBufferLib",
	srcs = glob(["circular_buffer/*.c"]),
	hdrs = glob(["circular_buffer/*.h"]),

	# visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["InterruptLib"]
)

cc_library(
	name = "FlashLib",
	srcs = glob(["flash/*.c"]),
	hdrs = glob(["flash/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["SpiLib", "LedLibs", "IoLibs", "SpiArbitrationLib", "FpgaFlashLib"]
	#  + select({
        # "@AvrToolchain//config:ElasticNode4": [],
        # "@AvrToolchain//config:ElasticNode3": [
		# 	]
    # })
)

cc_library(
	name = "SoftSerialLib",
	srcs = glob(["softSerial/*.c"]),
	hdrs = glob(["softSerial/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["InterruptLib", "CurrentSenseLib"]
)

cc_library(
	name = "SoftSpiLib",
	srcs = glob(["softspi/*.c"]),
	hdrs = glob(["softspi/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["FpgaLibs"]
)

cc_library(
	name = "SpiLib",
	srcs = glob(["spi/*.c"]),
	hdrs = glob(["spi/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["PinLib", "DebugLib"]
)

cc_library(
	name = "XorLib",
	srcs = glob(["xor/*.c"]),
	hdrs = glob(["xor/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["IoLibs"]
)

cc_library(
	name = "SpiArbitrationLib",
	srcs = glob(["spiArbitration/*.c"]),
	hdrs = glob(["spiArbitration/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["FpgaLibs", "FpgaFlashLib"]
)

# cc_library(
# 	name = "xmemLib",
# 	srcs = glob(["lib/xmem/*.c"]),
# 	hdrs = glob(["lib/xmem/*.h"]),

# 	visibility = ["//visibility:public"],

# 	copts = elasticNodeCopts,
# 	deps = ["IoLibs"]
# )

cc_library(
	name = "CurrentSenseLib",
	srcs = glob(["currentMeasurement/*.c"]),
	hdrs = glob(["currentMeasurement/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
	deps = ["CircularBufferLib", "DebugLib"]
)

cc_library(
	name = "InterruptLib",
	srcs = glob(["interrupts/*.c"]),
	hdrs = glob(["interrupts/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,
)

cc_library(
	name = "LedLibs",
	srcs = glob(["leds/*.c"]),
	hdrs = glob(["leds/*.h"]),

	visibility = ["//visibility:public"],

	copts = elasticNodeCopts,

	deps = ["IoLibs", "FpgaLibs"]
)

