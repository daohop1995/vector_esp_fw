# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# list(APPEND SDKCONFIG_DEFAULTS "sdkconfig.defaults")
set(PROJECT_VER "01000001")
# CONFIG_APP_PROJECT_VER_FROM_CONFIG option is set, the value of 
# CONFIG_APP_PROJECT_VER

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(vector_esp_fw)
