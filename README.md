
# 💧 **Smart Water Level Monitoring System** 🚰

### Overview
This project is a **real-time water level monitoring system** designed to help manage water levels efficiently. It integrates an **ESP8266 microcontroller**, sensors, OLED display, push buttons, and a buzzer, all working together seamlessly. With this system, users can monitor water levels and receive alerts, all while managing settings through an intuitive **web interface**.

### 🌟 Key Features
- **Modular Design:** Separates the **sensor** and **display** functionalities into distinct modules, enhancing flexibility and maintainability.
- **Real-Time Web Interface:** Allows control and monitoring from any device connected to the network.
- **Graphical Display:** OLED screen shows WiFi status, water level, and a visual representation of the tank’s water level.
- **Alerts & Notifications:** Buzzer and display alerts for low or high water levels, ensuring timely intervention.
- **Expandable for Additional Data:** Displays weather data and other details like current time for a more informative user experience.

---

## 🧩 **Project Structure**

```plaintext
├── **Sensor Module**        # Manages water level sensors and data acquisition
├── **Display Module**       # Manages OLED screen display and water tank representation
├── **Web Interface**        # WebSocket-powered interface for real-time monitoring and control
├── **Alert System**         # Includes buzzer and push buttons for notifications and user feedback
```

### 1. **Sensor Module**
   - Responsible for acquiring and processing water level data.
   - Operates independently to keep the display updated in real time.

### 2. **Display Module**
   - Uses an OLED screen to show:
     - WiFi status
     - Current water level
     - Graphical representation of the tank

### 3. **Web Interface**
   - Accessible from any device on the local network.
   - Allows users to view water levels and control settings with ease.
   - Uses **WebSocket** for **real-time data updates**.

### 4. **Alert System**
   - Push buttons for direct user interaction and feedback.
   - Buzzer notifications for critical alerts (e.g., water level too high or too low).

---

## ⚙️ **Technologies & Components**

- **Hardware:** ESP8266, water level sensors, OLED display, push buttons, buzzer
- **Communication:** WebSocket for efficient, real-time data exchange
- **Programming Language:** C++ for embedded programming on ESP8266
- **Display:** OLED graphics for easy-to-read, real-time data visuals

---

## 📸 Visual Overview
![Water Level Monitoring System](https://your-image-link.com)  
*Image of the setup showing the sensor module, display module, and web interface.*

---

## 🎨 **Getting Started**

### Prerequisites
- ESP8266 microcontroller with Arduino IDE configured
- Libraries: `ESP8266WiFi`, `WebSocket`, `Adafruit_GFX`, `Adafruit_SSD1306`

### Setup
1. **Clone the Repository**
   ```bash
   git clone https://github.com/your-repo/water-level-monitoring
   ```
2. **Upload Code to ESP8266**
   - Open the project in **Arduino IDE**.
   - Configure **WiFi credentials** and **WebSocket settings**.
   - Upload the code to your ESP8266.

3. **Access the Web Interface**
   - Connect to the same network as your ESP8266.
   - Open the web interface through the ESP8266’s IP address.

---

## 💡 **Future Enhancements**

- **Cloud Integration** for remote monitoring and data logging.
- **Mobile App** for notifications and control on the go.
- **Expanded Visuals** with animations or additional metrics on the OLED screen.

---

## 📜 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

### Happy Monitoring!