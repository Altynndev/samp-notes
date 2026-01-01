# samp-notes

A modern and dynamic notification system for SA-MP & open.mp based on TextDraws, designed to be lightweight and easy to use.

## ✨ Features
- **Queue System:** Notifications won't overlap; if multiple are triggered, they will appear one after another.
- **Customization:** Full support for titles, long messages, and custom hex colors.
- **Auto-hide:** Integrated timer to automatically hide notifications after a set duration.
- **Optimized:** Efficient TextDraw management to avoid hitting global limits.

## 🛠 Installation
1. Download the `samp-notes.inc` file.
2. Place it in your `/pawno/include/` folder.
3. Include it at the top of your Gamemode or Filterscript:
   ```pawn
   #include <samp-notes>

## 🚀 Basic Usage
To display a notification to a player, simply use the ShowNotification function:

`public OnPlayerConnect(playerid)
{
    ShowNotification(
        playerid, 
        "Welcome", 
        "Thanks for joining the server!", 
        0x33AA33FF, // Green color
        5000        // Time in milliseconds (5 seconds)
    );
    return 1;
}`

## 📋 Requirements
SA-MP 0.3.7 or higher / open.mp.
YSI-Includes (optional, recommended for efficient timers).

## 📄 Credits
Creator: Leap (dc: altyn.dev)
