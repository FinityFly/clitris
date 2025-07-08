<div align="center">

# 🎮 clitris

**A minimalistic, high-fidelity, guideline Tetris implementation for the command line**

[![GitHub release](https://img.shields.io/github/v/release/FinityFly/clitris?style=for-the-badge)](https://github.com/FinityFly/clitris/releases)
[![Build Status](https://img.shields.io/github/actions/workflow/status/FinityFly/clitris/release.yaml?style=for-the-badge)](https://github.com/FinityFly/clitris/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey?style=for-the-badge)](https://github.com/FinityFly/clitris)

[![Downloads](https://img.shields.io/github/downloads/FinityFly/clitris/total?style=for-the-badge)](https://github.com/FinityFly/clitris/releases)
[![Stars](https://img.shields.io/github/stars/FinityFly/clitris?style=for-the-badge)](https://github.com/FinityFly/clitris/stargazers)
[![Issues](https://img.shields.io/github/issues/FinityFly/clitris?style=for-the-badge)](https://github.com/FinityFly/clitris/issues)

[**🚀 Quick Start**](#-installation) • [**📖 Documentation**](#-controls) • [**🎮 Demo**](#-demo) • [**🤝 Contributing**](#-contributing)

---

<table width="100%">
  <tr>
    <td width="50%" align="center">
      <img src="assets/thumbnail.png" height="500" alt="clitris gameplay screenshot">
      <br><strong>🎮 Gameplay</strong>
    </td>
    <td width="50%" align="center">
      <img src="assets/settings.png" height="500" alt="clitris settings interface">
      <br><strong>⚙️ Settings</strong>
    </td>
  </tr>
</table>

</div>

## 📋 Table of Contents

- [🎮 clitris](#-clitris)
  - [📋 Table of Contents](#-table-of-contents)
  - [🔍 About](#-about)
    - [✨ Features](#-features)
  - [🎬 Demo](#-demo)
  - [🚀 Installation](#-installation)
    - [🍺 Homebrew (Recommended)](#-homebrew-recommended)
    - [🐧 Linux/macOS](#-linuxmacos)
  - [🎯 Quick Start](#-quick-start)
  - [🎮 Controls](#-controls)
  - [⚙️ Configuration](#️-configuration)
  - [🏗️ Building](#️-building)
    - [Prerequisites](#prerequisites)
    - [Build](#build)
  - [🧹 Uninstall](#-uninstall)
    - [🍺 Homebrew](#-homebrew)
    - [🐧 Linux/macOS](#-linuxmacos-1)
  - [🤝 Contributing](#-contributing)
  - [📄 License](#-license)

## 🔍 About

clitris is a professional-grade Tetris implementation for the terminal, featuring modern guideline Tetris and competitive gameplay mechanics. I was slightly irritated by the lack of high-quality yet lightweight terminal tetris clones and decided to do something about it. Enjoy!

### ✨ Features

- **🎯 Guideline Compliant**: SRS rotation system, official scoring, standard TETR.IO-like mechanics
- **⚡ Performance Optimized**: 60 FPS gameplay with sub-millisecond input response  
- **🔧 Highly Configurable**: Customizable handling (ARR/DAS/DCD/SDF) and keybind settings
- **🎮 4 Game Modes**: Sprint, Blitz, Zen, and Cheese Race
- **🎨 Clean and Modern Terminal UI**: Minimalist guideline tetris design with ghost piece, hold/next piece windows, and live stats display

## 🎬 Demo

```
Demo vid will exist soontm!
```

## 🚀 Installation

### 🍺 Homebrew (Recommended)

```bash
# Install
brew install FinityFly/tap/clitris

# Upgrade
brew upgrade clitris
```

### 🐧 Linux/macOS
```bash
curl -fsSL https://raw.githubusercontent.com/FinityFly/clitris/main/install.sh | bash
```

## 🎯 Quick Start

```bash
clitris  # or ./clitris if built locally
```

## 🎮 Controls

All controls are fully customizable in the in-game settings menu. The default keybinds are:

| Action        | Default Keys         |
|---------------|---------------------|
| Move Left     | `←` `j`             |
| Move Right    | `→` `l`             |
| Soft Drop     | `↓` `k`             |
| Hard Drop     | `Space`             |
| Hold          | `↑` `i`             |
| Rotate CW     | `x`                 |
| Rotate CCW    | `z`                 |
| 180° Rotate   | `c`                 |
| Pause         | `p`                 |
| Quit          | `q` `Esc`           |
| Restart       | `r` `\`            |

You can remap any action to any key (or multiple keys) in the settings menu.

## ⚙️ Configuration

All handling and keybind settings are saved automatically in a binary file (`settings.bin`) in your user config directory:
- **macOS:** `~/Library/Application Support/clitris/settings.bin`
- **Linux:** `~/.config/clitris/settings.bin`
- **Windows:** `%APPDATA%\clitris\settings.bin`

| Setting | Description              | Default |
|---------|--------------------------|---------|
| **ARR** | Auto Repeat Rate (ms)    | 10      |
| **DAS** | Delayed Auto Shift (ms)  | 50      |
| **DCD** | DAS Cut Delay (ms)       | 33      |
| **SDF** | Soft Drop Factor (ms)    | 1       |

Access the settings via `[5] Settings` in the main menu to change keybinds or handling.

## 🏗️ Building

### Prerequisites
- C++17 compiler
- ncurses library  
- make

### Build
```bash
git clone https://github.com/FinityFly/clitris.git
cd clitris && make
sudo mv clitris /usr/local/bin/clitris  # optional
```

## 🧹 Uninstall

### 🍺 Homebrew
```bash
brew uninstall FinityFly/tap/clitris
```

### 🐧 Linux/macOS
```bash
sudo rm /usr/local/bin/clitris
```

## 🤝 Contributing

Contributions welcome! Please check [Issues](https://github.com/FinityFly/clitris/issues) or submit a PR.

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.
