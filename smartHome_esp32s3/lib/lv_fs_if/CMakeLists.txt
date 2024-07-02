idf_component_register(SRCS "lv_fs_fatfs.c"
                        "lv_fs_if.c"
                       INCLUDE_DIRS "./"
                       REQUIRES lvgl fatfs)

endif()
