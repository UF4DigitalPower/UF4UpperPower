#此文件从模板自动生成! 请勿更改!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.28)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_ASM_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
else()
    message(STATUS "ccache not found")
endif()

set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(UF4UpperPower C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#关于软件浮点数的评论取消
#add_compile_options(-mfloat-abi=soft)

add_compile_options(-mcpu=cortex-m7 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

set(UF4COM "${CMAKE_SOURCE_DIR}/UF4COM")

include_directories(Core/Inc
        Drivers/STM32H7xx_HAL_Driver/Inc
        Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32H7xx/Include
        Drivers/CMSIS/Include
        UF4APP/USER
        UF4APP/TELECOM
        UF4BSP

        ${UF4COM}/uf4com/protocol/Inc
        ${UF4COM}/uf4com/register/Inc
        ${UF4COM}/uf4com/transport/uf4com_uart/Inc
)

add_definitions(-DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "Core/*.*"
        "Drivers/*.*"
        "UF4BSP/*.*"
        "UF4APP/*.*"
        "${UF4COM}/uf4com/protocol/Src/*.c"
        "${UF4COM}/uf4com/register/Src/*.c"
        "${UF4COM}/uf4com/transport/uf4com_uart/Src/*.c"

)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32H743ZITX_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m7 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

set(UF4_BANNER_TXT "${CMAKE_BINARY_DIR}/uf4_banner.txt")
set(UF4_BANNER_SCRIPT "${CMAKE_BINARY_DIR}/uf4_generate_banner.cmake")
file(WRITE "${UF4_BANNER_SCRIPT}" [=[
string(TIMESTAMP UF4_BUILD_TIME "%Y-%m-%d %H:%M:%S")
file(WRITE "${UF4_BANNER_TXT}" "

  ██╗   ██╗███████╗██╗  ██╗██╗   ██╗██████╗ ██████╗ ███████╗██████╗ ██████╗  ██████╗ ██╗    ██╗███████╗██████╗
  ██║   ██║██╔════╝██║  ██║██║   ██║██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔═══██╗██║    ██║██╔════╝██╔══██╗
  ██║   ██║█████╗  ███████║██║   ██║██████╔╝██████╔╝█████╗  ██████╔╝██████╔╝██║   ██║██║ █╗ ██║█████╗  ██████╔╝
  ██║   ██║██╔══╝  ╚════██║██║   ██║██╔═══╝ ██╔═══╝ ██╔══╝  ██╔══██╗██╔═══╝ ██║   ██║██║███╗██║██╔══╝  ██╔══██╗
  ╚██████╔╝██║          ██║╚██████╔╝██║     ██║     ███████╗██║  ██║██║     ╚██████╔╝╚███╔███╔╝███████╗██║  ██║
   ╚═════╝ ╚═╝          ╚═╝ ╚═════╝ ╚═╝     ╚═╝     ╚══════╝╚═╝  ╚═╝╚═╝      ╚═════╝  ╚══╝╚══╝ ╚══════╝╚═╝  ╚═╝

  ██████╗ ██╗   ██╗██╗██╗     ██████╗     ███████╗██╗   ██╗ ██████╗ ██████╗███████╗███████╗███████╗
  ██╔══██╗██║   ██║██║██║     ██╔══██╗    ██╔════╝██║   ██║██╔════╝██╔════╝██╔════╝██╔════╝██╔════╝
  ██████╔╝██║   ██║██║██║     ██║  ██║    ███████╗██║   ██║██║     ██║     █████╗  ███████╗███████╗
  ██╔══██╗██║   ██║██║██║     ██║  ██║    ╚════██║██║   ██║██║     ██║     ██╔══╝  ╚════██║╚════██║
  ██████╔╝╚██████╔╝██║███████╗██████╔╝    ███████║╚██████╔╝╚██████╗╚██████╗███████╗███████║███████║
  ╚═════╝  ╚═════╝ ╚═╝╚══════╝╚═════╝     ╚══════╝ ╚═════╝  ╚═════╝ ╚═════╝╚══════╝╚══════╝╚══════╝

  ┌──────────────────────────────────────────────────────────────────────────────┐
  │ Project      : ${UF4_PROJECT_NAME}
  │ Build Type   : ${UF4_BUILD_TYPE}
  │ Build Time   : ${UF4_BUILD_TIME}
  │ Host Platform: ${UF4_HOST_SYSTEM_NAME} ${UF4_HOST_SYSTEM_PROCESSOR}
  │ Target System: ${UF4_TARGET_SYSTEM_NAME}
  │ MCU          : STM32H743ZITx / STM32H743xx
  │ CPU/FPU      : Cortex-M7, hard-float, fpv4-sp-d16
  │ Memory       : FLASH 2048 KB
  │ RAM          : DTCMRAM 128 KB, RAM_D1 512 KB, RAM_D2 288 KB, RAM_D3 64 KB, ITCMRAM 64 KB
  │ External RAM : SDRAM 32 MB
  │ Linker       : ${UF4_LINKER_SCRIPT}
  │ Compiler     : ${UF4_C_COMPILER_ID} ${UF4_C_COMPILER_VERSION}
  │ Toolchain    : ${UF4_C_COMPILER}
  │ Output       : ${UF4_OUTPUT_ELF}
  └──────────────────────────────────────────────────────────────────────────────┘

  UF4UpperPower firmware generated successfully.

")
]=])

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_COMMAND}
                -DUF4_BANNER_TXT=${UF4_BANNER_TXT}
                -DUF4_PROJECT_NAME=${PROJECT_NAME}
                -DUF4_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DUF4_HOST_SYSTEM_NAME=${CMAKE_HOST_SYSTEM_NAME}
                -DUF4_HOST_SYSTEM_PROCESSOR=${CMAKE_HOST_SYSTEM_PROCESSOR}
                -DUF4_TARGET_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                -DUF4_LINKER_SCRIPT=${LINKER_SCRIPT}
                -DUF4_C_COMPILER_ID=${CMAKE_C_COMPILER_ID}
                -DUF4_C_COMPILER_VERSION=${CMAKE_C_COMPILER_VERSION}
                -DUF4_C_COMPILER=${CMAKE_C_COMPILER}
                -DUF4_OUTPUT_ELF=$<TARGET_FILE:${PROJECT_NAME}.elf>
                -P "${UF4_BANNER_SCRIPT}"
        COMMAND ${CMAKE_COMMAND} -E cat "${UF4_BANNER_TXT}"
        COMMENT "Generating firmware files"
        VERBATIM
)
