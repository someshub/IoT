display the list of detected devices on the TFT display.

Explanation

    BLE Setup:
        The BLE scanning setup remains unchanged.

    TFT Display Setup:
        The TFT display setup is also unchanged.

    Main Loop:
        Perform BLE scanning for the specified time (scanTime).
        After scanning, print the number of devices found to the Serial Monitor.
        Call displayDevices(&foundDevices) to display the list of detected devices on the TFT display.
        Clear the scan results to free memory and delay for 2 seconds before the next scan.

    Display Devices Function:
        Clear the screen and set the cursor to the top left.
        Iterate through the found devices, convert each device to a string, truncate if necessary, and print each device on a new line.
        Adjust the y-coordinate for each new line and ensure no overflow by breaking out of the loop if the y-coordinate exceeds the display height.
