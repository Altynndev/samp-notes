# samp-notes

A highly customizable, lightweight, and easy-to-use improved notification system

## 📜 Features
- **Queue System:** Notifications won't overlap; if multiple are triggered, they will appear one after another.
- **Customization:** Full support for titles, long messages, and custom hex colors.
- **Auto-hide:** Integrated timer to automatically hide notifications after a set duration.
- **Optimized:** Efficient TextDraw management to avoid hitting global limits.
- **Dynamic structure:** Optimized for two-dimensional array structures to create custom notifications with sprites, preview models, etc... Using the "Type" parameter in the dynamic creation of the notification.

## 🛠 Installation
1. Download the `samp-notes.inc` file.
2. Place it in your `/pawno/include/` folder.
3. Include it at the top of your Gamemode or Filterscript:
   ```pawn
   #include <samp-notes>

## 🚀 Basic Usage
To display a notification to a player, simply use the ShowPlayerNotification function and fill the parameters:

## 📋 Requirements
a_samp
YSI-Includes (optional, recommended for efficient timers).

## 📄 Credits
Creator: Leap (dc: altyn.dev)
