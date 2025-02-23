# An-IoT-based-platform-to-monitor-a-water-pump
## Introduction

Water pumps play a critical role in household and industrial applications, ensuring a steady water supply for various needs. However, unexpected pump failures can lead to costly repairs, downtime, and inconvenience. This project aims to monitor household water pump health using IoT sensors and cloud integration. By continuously analyzing real-time sensor data, the system detects potential issues early, preventing failures and reducing maintenance costs.

## Features

Real-time monitoring of water flow, pressure, current, voltage, and vibration.

IoT-based data transmission to ThingSpeak.

Email alerts for abnormal conditions.

Predictive maintenance capabilities.

## Hardware Components

ESP32 - Microcontroller with Wi-Fi.

Current Sensor (ZMCT103C) - Measures current.

Water Flow Sensor (YFS-201) - Monitors flow rate.

Vibration Sensor (SW-420) - Detects pump vibrations.

Voltage Sensor (ZMPT101B) - Monitors voltage levels.

1HP Water Pump - System under observation.

## Software & Tools

Arduino IDE - Programming the ESP32.

ThingSpeak - Cloud dashboard for data visualization.

Email Notification Service - Alerts for threshold breaches.

## Working Principle

Sensors collect real-time data.

ESP32 processes and transmits data to ThingSpeak.

Threshold breaches trigger email alerts.

