# 🎵 SFML Music Player

This is a fully functional Music Player developed using **C++** and **SFML (Simple and Fast Multimedia Library)**. The project showcases clean architecture using **Object-Oriented Programming (OOP)** principles such as **encapsulation**, **inheritance**, **polymorphism**, **abstraction**, **composition**, and **file handling**.

---

## 📌 Features

- 🎧 Play, Pause, Resume, and Stop music
- ❤️ Add/remove songs to/from **MyFavourite** playlist
- 📂 Load/save playlist using file handling
- 📃 List all songs in the library
- 🎹 Simple CLI Interface (with scope for GUI enhancement)
- 💾 Persistent data using file system

---

## 🧠 OOP Concepts Covered

### ✅ 1. **Encapsulation**
- Music player logic is wrapped inside classes like `MusicPlayer`, `Playlist`, and `Song`.
- Data members are kept private and accessed via public member functions (getters/setters).

### ✅ 2. **Abstraction**
- Users interact with high-level interfaces without needing to understand the low-level implementation of SFML audio playback.

### ✅ 3. **Inheritance**
- A base class `AudioItem` (optional in design) can be extended by `Song` or other media types in the future (e.g., Podcast).
- You can define generic playback behavior and override as needed.

### ✅ 4. **Polymorphism**
- Use of virtual functions for common audio operations like `play()`, `pause()`, `stop()` across potentially different media classes.

### ✅ 5. **Composition**
- `Playlist` class has a collection (e.g., vector) of `Song` objects — not inheritance but "has-a" relationship.

### ✅ 6. **File Handling**
- All user actions such as adding songs to the favourites list are saved to files and reloaded when the application starts.

---

## 🛠️ Technologies Used

- 💻 **C++** (Modern C++)
- 🎵 **SFML** (Audio module)
- 📝 Standard C++ File Handling (`fstream`)
- 🧪 Object-Oriented Programming
- 🧰 CLI (Command-line interface) for user interaction

---

## 📂 Project Structure
FML-MusicPlayer/
├── include/
│ └── Song.h
│ └── Playlist.h
│ └── MusicPlayer.h
├── src/
│ └── main.cpp
│ └── Song.cpp
│ └── Playlist.cpp
│ └── MusicPlayer.cpp
├── songs/ # Music files (.ogg, .wav, .mp3 etc.)
├── MyFavourite.txt
└── README.md

---

## 🚀 How to Run

### 🔧 Prerequisites
- SFML installed and linked properly.
- C++ compiler (g++, clang, MSVC, etc.)
- CMake or manual build setup

### 🔨 Build & Run
```bash
g++ src/*.cpp -o MusicPlayer -lsfml-audio -lsfml-system
./MusicPlayer

👤 Author
Asad Ahmed
📍 FAST-NUCES | 💻 Software Developer in Training
📧 asadahmedk09@gmail.com
🔗 GitHub: Asadahmed09

🙌 Acknowledgements
SFML Library

StackOverflow and SFML documentation

My university teachers and peers for guidance and support

