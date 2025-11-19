idf_component_register(SRCS main.c game.c
                       INCLUDE_DIRS .
                       PRIV_REQUIRES config joy pin cursor lcd esp_timer)
# PRIV_REQUIRES ...
# target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
