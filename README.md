# 🛰️ C-MQTT — Lightweight MQTT Broker in C

**C-MQTT** is a lightweight, fast, and minimal MQTT broker written in pure C. It implements the MQTT 3.1.1 protocol and is designed for embedded systems, IoT applications, and environments where resources are constrained but performance matters.

## 🚀 Features

- 🧵 Multi-threaded client handling (configurable)
- ⚙️ Full support for MQTT 3.1.1
- 🪝 QoS 0 and QoS 1 support
- 🗂️ Topic-based publish/subscribe mechanism
- 🔒 Optional username/password authentication
- 🧪 Lightweight and extensible design
- 🖥️ Built-in CLI for debugging and monitoring

## 📦 Getting Started

### Prerequisites

- GCC or Clang
- `CMake`

### Build

```bash
git clone https://github.com/Turtel216/c-mqtt.git
cd c-mqtt
mkdir build
cd build
cmake ..
cmake --build .
```

### Run

```bash
./cmqtt
```

The broker will start on port `1883` by default. You can change this via command-line arguments or config file (see below).

## ⚙️ Configuration

You can pass options via CLI or a simple config file:

### Command-line

```bash
./cmqtt -p 1884 -a 0.0.0.0 --auth
```

### Configuration File (`config.ini`)

```ini
[server]
port = 1883
address = 127.0.0.1

[security]
auth_enabled = true
username = admin
password = secret
```

## 📚 Protocol Support

| Feature              | Support     |
|----------------------|-------------|
| MQTT 3.1.1            | ❌ (planned) |
| QoS 0                 | ❌ (planned) |
| QoS 1                 | ❌ (planned) |
| QoS 2                 | ❌ (planned) |
| Retained Messages     | ❌ (planned) |
| Last Will & Testament | ❌ (planned) |

## 🔧 Project Structure

```
c-mqtt/
├── src/
│   ├── mqtt.c
│   ├── pack.c
│   └── ...
├── include/
│   ├── mqtt.h
│   ├── pack.h
├── tests/
├── config/
├── CMakeLists.txt
└── README.md
```

## 🧪 Testing

Basic unit and integration tests are included:

```bash
cd build
ctest
```

You can also use tools like `mosquitto_pub` and `mosquitto_sub` for manual testing.

## 📜 License

MIT License. See [LICENSE](LICENSE) for details.

## 🙌 Contributing

Contributions, bug reports, and feature requests are welcome!

1. Fork the repo
2. Create your feature branch (`git checkout -b feature/foo`)
3. Commit your changes
4. Push to the branch
5. Open a pull request
