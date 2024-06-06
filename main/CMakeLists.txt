idf_component_register(
    SRCS 
		# hal
		"hal/bble/bble.c"
		"hal/bfs/bfs.c"
		"hal/bi2c/bi2c.c"
		"hal/bi2c/hard_i2c_lib.c"
		"hal/bi2c/soft_i2c_lib.c"
		"hal/brgb/brgb.c"
		"hal/brgb/led_strip_rmt_ws2812.c"
		"hal/lpi2c/lp_core_i2c.c"
		"hal/lpi2c/ulp_lp_core_i2c.c"
		# "hal/bi2s/bi2s.c"
        # ble
        # "ble/ble.c"
        # "ble/ble_lib.c"
		# lib
		"lib/baudio/baudio_codec_tlv320aic3120.c"
		"lib/bioexp/bioexp.c"
		"lib/bhaptic/bhaptic.c"
		"lib/pmods/pmods.c"
		"lib/pmods/adxl346/adxl346.c"
		"lib/pmods/ina219/ina219.c"
		"lib/pmods/kmx61/kmx61.c"
		"lib/pmods/mlx90640/mlx90640_api.c"
		"lib/pmods/mlx90640/mlx90640_i2c_driver.c"
		"lib/pmods/mlx90640/mlx90640.c"
		"lib/pmods/si7006/si7006.c"
		"lib/pmods/vl53l0x/vl53l0x.c"
		"lib/pmods/vl53l0x/vl53l0x_driver.c"
		# "lib/pmods/ADS7142/ADS7142.c"
		# "lib/pmods/FXOS8700CQ/FXOS8700CQ.c"
        # mod
		"mod/ble/ble_mod.c"
		"mod/daq/daq_mod.c"
		"mod/sys/sys_mod.c"
		# "daq/daq_mod.c"
		# "daq/i2c_lib.c"
        #
        "main.c"
    INCLUDE_DIRS 
        "."
)

# Create a SPIFFS image from the contents of the 'spiffs_content' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'idf.py -p PORT flash'.
# spiffs_create_partition_image(storage ../spiffs_content FLASH_IN_PROJECT)
