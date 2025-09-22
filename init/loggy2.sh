#!/system/bin/sh

# Log file path
LOGFILE="/cache/loggy2.log"

# UART device
UART="/dev/ttyMSM0"

# Ensure log file exists
#[ -f "$LOGFILE" ] || touch "$LOGFILE"

# Ensure UART device exists and is writable
if [ ! -w "$UART" ]; then
    echo "UART device $UART not writable!"
    exit 1
fi

#echo "---- KRAB_DEBUG_BEGIN ----" > $UART
#ls -lah /dev/block/bootdevice/by-name/ > $UART
#echo "---- KRAB_DEBUG_END ----" > $UART

# Handle Ctrl+C and terminate cleanly
trap "echo 'Stopping log capture'; exit 0" INT TERM

# Read logcat line by line
logcat | while IFS= read -r line
do
    LOG_LINE="LC: $line"

    # Append to logfile
#    echo "$LOG_LINE" >> "$LOGFILE"

    # Send to UART
    echo "$LOG_LINE" > "$UART"
done
