#settings
PROJECT_CFG=/home/tomv/workspace/bragmap_touch/Drivers/STM32F0_Discovery/stm32f0_discovery.cfg
PROJECT_HEX=/home/tomv/workspace/bragmap_touch/Build/bragmap_touch.hex

# find PIDs of all openocd instances
OPENOCD_PID=$(ps aux | grep '[o]penocd' | awk '{print $2}')

# kill openocd if found
if [ ${#OPENOCD_PID} != 0 ]
	then 

		echo "killing open ocd instances ["
		echo $OPENOCD_PID
		echo "]"
		kill $OPENOCD_PID
	else
		echo "Openocd not running.."
fi

#determine openocd mode
if [ $# -eq 0 ]
	then 
		OPENOCD_MODE="FLASH"
	else 
		if [ $1 == "debug" ]
			then
				OPENOCD_MODE="DEBUG"
			else
				OPENOCD_MODE="FLASH"
		fi
fi

#call openocd
if [ $OPENOCD_MODE == "FLASH" ]
	then 
		echo "calling openocd as:$USER -> in FLASH mode" 
		openocd -f $PROJECT_CFG -c "program $PROJECT_HEX verify reset"
	else
		echo "calling openocd as:$USER -> in FLASH mode" 		
		openocd -f $PROJECT_CFG -c init -c "reset init" -c halt
fi

