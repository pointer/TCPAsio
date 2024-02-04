    import tkinter as tk
    from tkinter import ttk

    # Create the main window
    root = tk.Tk()
    root.title("BLE Device Scanner")

    # Add a label
    label = ttk.Label(root, text="Press the button to scan for BLE devices")
    label.pack(pady=20)

    # Function to call when scanning for devices
    def scan_for_devices():
        print("Scanning for devices...")

    # Add a button
    scan_button = ttk.Button(root, text="Scan", command=scan_for_devices)
    scan_button.pack(pady=20)

    # Start the GUI event loop
    root.mainloop()
    
        import asyncio
    from bleak import BleakClient

    READ_CHARACTERISTIC_UUID = "your-read-characteristic-uuid-here"
    WRITE_CHARACTERISTIC_UUID = "your-write-characteristic-uuid-here"

    async def read_from_device(address):
        async with BleakClient(address) as client:
            value = await client.read_gatt_char(READ_CHARACTERISTIC_UUID)
            print("Read data:", value)

    async def write_to_device(address, data):
        async with BleakClient(address) as client:
            await client.write_gatt_char(WRITE_CHARACTERISTIC_UUID, data)
            print("Data written")

    def read_data():
        address = "device-address-here"
        asyncio.run(read_from_device(address))

    def write_data():
        address = "device-address-here"
        data = bytearray([0x00, 0x01, 0x02])  # Example data
        asyncio.run(write_to_device(address, data))
        

import tkinter as tk
from tkinter import ttk
import asyncio
from bleak import BleakClient
import threading

# BLE device details
DEVICE_ADDRESS = "device-address-here"
READ_CHARACTERISTIC_UUID = "your-read-characteristic-uuid-here"

# Asynchronous function to continuously read from device
async def continuous_read_from_device(address, uuid, interval=1.0):
    async with BleakClient(address) as client:
        while True:
            value = await client.read_gatt_char(uuid)
            print("Read data:", value)
            await asyncio.sleep(interval)

# Function to start the asynchronous loop in a thread
def start_reading():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    loop.run_until_complete(continuous_read_from_device(DEVICE_ADDRESS, READ_CHARACTERISTIC_UUID))

# Function to run the async function in a separate thread
def on_read_button_click():
    threading.Thread(target=start_reading, daemon=True).start()

# Setting up the GUI
root = tk.Tk()
root.title("BLE Device Continuous Reader")

read_button = ttk.Button(root, text="Start Reading", command=on_read_button_click)
read_button.pack(pady=20)

root.mainloop()
