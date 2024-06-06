@echo off
mkdir "release/bootloader" || echo ""
mkdir "release/partition_table" || echo ""
copy build\bootloader\bootloader.bin release\bootloader
copy build\partition_table\partition-table.bin release\partition_table
copy build\vector_esp_fw.bin release
copy partitions.csv release
copy flash_cmd.txt release
del vector_esp_fw_fw.zip || echo ""
tar.exe -a -c -f vector_esp_fw_fw.zip	release
