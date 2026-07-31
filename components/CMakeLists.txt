set(src_dirs
            SYSTEM/keypad    
            SYSTEM/MY_UART
            SYSTEM/MY_IIC
            SYSTEM/MY_ADC
            SYSTEM/MY_SPI
            SYSTEM/MY_GPTIM
            SYSTEM/MY_WIFI_AP
            SYSTEM/ADC_equipment

            SHOW/OLED
            SHOW/LCD144

            MODULE/LED
            MODULE/KEY
            MODULE/DHT11
            MODULE/SG90
            MODULE/BH1750
            MODULE/ADXL345
            MODULE/RC522
            MODULE/AS608
            )


set(include_dirs            
            SYSTEM/keypad  
            SYSTEM/MY_UART
            SYSTEM/MY_IIC
            SYSTEM/MY_ADC
            SYSTEM/MY_SPI
            SYSTEM/MY_GPTIM      
            SYSTEM/MY_WIFI_AP
            SYSTEM/ADC_equipment
            
            SHOW/OLED
            SHOW/LCD144

            MODULE/LED
            MODULE/KEY
            MODULE/DHT11
            MODULE/SG90
            MODULE/BH1750
            MODULE/ADXL345
            MODULE/RC522
            MODULE/AS608
            )


set(requires
            REQUIRES nvs_flash
            esp_wifi
            esp_timer
            esp_adc
            driver
            esp_driver_ledc)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)
