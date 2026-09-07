idf_component_register(
    SRCS
        "main.c"
        "state/dummy_data.c"
        "drivers/lcd/DEV_Config.c"
        "drivers/lcd/LCD_Driver.c"
        "drivers/lcd/GUI_Paint.c"
        "drivers/lcd/font12.c"
        "drivers/lcd/font24.c"
        "drivers/encoder/encoder.c"
        "drivers/pcf8574/pcf8574.c"
    INCLUDE_DIRS
        "."
        "state"
        "drivers/lcd"
        "drivers/encoder"
        "drivers/pcf8574"
    REQUIRES
        driver
        esp_driver_gpio
        esp_driver_i2c
        esp_driver_spi
)

# Waveshare vendor driver code triggers harmless warnings (type-limits, etc.)
# Disable -Werror for these third-party files so they build unmodified.
set_source_files_properties(
    "drivers/lcd/GUI_Paint.c"
    "drivers/lcd/LCD_Driver.c"
    "drivers/lcd/DEV_Config.c"
    PROPERTIES COMPILE_FLAGS "-Wno-error=type-limits -Wno-error=unused-but-set-variable"
)
