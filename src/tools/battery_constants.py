import sys
import pandas as pd
import numpy as np

def scale_voltage(voltages, min_voltage, max_voltage):
    scaled = [int(255 * (v - min_voltage) / (max_voltage - min_voltage)) for v in voltages]
    for i in range(1, len(scaled)):
        scaled[i] = min(scaled[i], scaled[i - 1])
    return scaled

def main(filename):
    # Load the CSV file, explicitly skipping the first row (header)
    data = pd.read_csv(filename, skiprows=1, header=None, names=['Time', 'Voltage'])

    # Convert Time to datetime and sort just in case
    data['Time'] = pd.to_datetime(data['Time'], format='%H:%M:%S', errors='coerce')
    data.sort_values('Time', inplace=True)

    # Find the minimum and maximum voltage
    min_voltage = data['Voltage'].min()
    max_voltage = data['Voltage'].max()

    # Divide the range into 100 parts and calculate the average voltage for each part
    data['TimeIndex'] = pd.cut(data['Time'], bins=100, labels=False)
    avg_voltages = data.groupby('TimeIndex')['Voltage'].mean().tolist()

    # Scale the average voltages and ensure they do not increase
    scaled_voltages = scale_voltage(avg_voltages, min_voltage, max_voltage)

    # Output the constants in the desired format
    print(f"const float min_voltage = {min_voltage};")
    print(f"const float max_voltage = {max_voltage};")
    print("const uint8_t scaled_voltage[100] = {")
    for i in range(0, 100, 10):
        line = ", ".join(map(str, scaled_voltages[i:i+10]))
        print(f"  {line},")
    print("};")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py filename.csv")
    else:
        main(sys.argv[1])
