<div align="center">

# 🎮 clitris

**A modern, guideline-compliant Tetris implementation for the command line**

[![GitHub release](https://img.shields.io/github/v/release/FinityFly/clitris?style=for-the-badge)](https://github.com/FinityFly/clitris/releases)
[![Build Status](https://img.shields.io/github/actions/workflow/status/FinityFly/clitris/release.yaml?style=for-the-badge)](https://github.com/FinityFly/clitris/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey?style=for-the-badge)](https://github.com/FinityFly/clitris)

[![Downloads](https://img.shields.io/github/downloads/FinityFly/clitris/total?style=for-the-badge)](https://github.com/FinityFly/clitris/releases)
[![Stars](https://img.shields.io/github/stars/FinityFly/clitris?style=for-the-badge)](https://github.com/FinityFly/clitris/stargazers)
[![Issues](https://img.shields.io/github/issues/FinityFly/clitris?style=for-the-badge)](https://github.com/FinityFly/clitris/issues)

[**🚀 Quick Start**](#installation) • [**📖 Documentation**](#usage) • [**🎮 Demo**](#demo) • [**🤝 Contributing**](#contributing)

---

<img src="assets/thumbnail.png" height="500" alt="clitris gameplay screenshot">

</div>

## 📋 Table of Contents

- [🎮 clitris](#-clitris)
  - [📋 Table of Contents](#-table-of-contents)
  - [🔍 About](#-about)
    - [Why clitris?](#why-clitris)
  - [🎬 Demo](#-demo)
  - [✨ Features](#-features)
  - [🚀 Installation](#-installation)
    - [🍺 Homebrew (Recommended)](#-homebrew-recommended)
    - [📦 From Source](#-from-source)
  - [🎯 Quick Start](#-quick-start)
  - [🎮 Controls](#-controls)
  - [⚙️ Configuration](#️-configuration)
  - [🏗️ Building](#️-building)
    - [Prerequisites](#prerequisites)
    - [Build](#build)
  - [🤝 Contributing](#-contributing)
  - [📄 License](#-license)

## 🔍 About

clitris is a professional-grade Tetris implementation for the terminal, featuring modern guideline Tetris and competitive gameplay mechanics.

### Why clitris?

- **🎯 Guideline Compliant**: SRS rotation system and official scoring
- **⚡ Performance Optimized**: 60 FPS gameplay with sub-millisecond input response  
- **🔧 Highly Configurable**: Professional handling settings (ARR/DAS/DCD/SDF)
- **📊 Analytics**: Detailed statistics tracking
- **🎨 Clean Terminal UI**: Minimalist design

## 🎬 Demo

```
Demo vid will exist soontm!
```

## ✨ Features

- **🎮 Modern Gameplay** - SRS rotation, T-spins, perfect clears, 7-bag randomizer
- **⚙️ Pro Handling** - Configurable ARR, DAS, DCD, SDF settings
- **📊 Statistics** - PPS, combos, line clears, B2B tracking
- **🎨 Clean UI** - Ghost piece, hold system, live stats display

## 🚀 Installation

### 🍺 Homebrew (Recommended)

```bash
brew install FinityFly/tap/clitris
```

### 📦 From Source

```bash
git clone https://github.com/FinityFly/clitris.git
cd clitris && make
sudo mv tetris /usr/local/bin/clitris  # optional
```

## 🎯 Quick Start

```bash
clitris  # or ./clitris if built locally
```

## 🎮 Controls

| Movement | Keys | Rotation | Keys |
|----------|------|----------|------|
| Left | `←` `j` | Rotate CW | `x` |
| Right | `→` `l` | Rotate CCW | `z` |
| Soft Drop | `↓` `k` | 180° Rotate | `c` |
| Hard Drop | `Space` | Hold | `↑` `i` |

**System:** `r` Restart • `q` `Esc` Quit

## ⚙️ Configuration

| Setting | Description | Default |
|---------|-------------|---------|
| **ARR** | Auto Repeat Rate | 33ms |
| **DAS** | Delayed Auto Shift | 100ms |
| **DCD** | DAS Cut Delay | 0ms |
| **SDF** | Soft Drop Factor | 1ms |

Access via `[2] Settings` in the main menu.

## 🏗️ Building

### Prerequisites
- C++17 compiler
- ncurses library  
- make

### Build
```bash
git clone https://github.com/FinityFly/clitris.git
cd clitris
make
```

## 🤝 Contributing

Contributions welcome! Please check [Issues](https://github.com/FinityFly/clitris/issues) or submit a PR.

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.
