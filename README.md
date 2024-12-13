# PacMan Operating System

The *PacMan Operating System* is a C++ project that combines the classic Pac-Man game with operating system concepts such as threads, synchronization, semaphores, and process management. Developed using SFML for graphical rendering and advanced data structures for process control, this project serves as both a fun simulation and a learning tool for OS principles.


## Key Features
- **Thread Management**: The system simulates concurrent processes using multi-threading to handle different game elements (e.g., PacMan, ghosts, and resources).
- **Synchronization**: Implements semaphores and the bakery algorithm to ensure safe and efficient resource allocation between threads.
- **Custom Data Structures**: Advanced data structures are used to manage game entities, scheduling, and process synchronization.
- **SFML Graphics**: Leverages SFML to render dynamic graphics, including the PacMan maze, characters, and animations.
- **Process Management**: Emulates OS-like processes where threads interact, synchronize, and manage resources in real-time.

## Technologies Used
- **C++**: The primary programming language used for development.
- **SFML**: A multimedia library for creating the game window, handling graphics, and managing user input.
- **Threads**: Used for simulating concurrent processes and events in the game.
- **Semaphores**: Implemented to control access to shared resources between threads.
- **Bakery Algorithm**: A synchronization method used for ensuring safe execution of critical sections in multi-threading environments.

## Getting Started

### Prerequisites
- **C++ Compiler** (GCC, Clang, MSVC)
- **SFML Library**: [SFML Installation Guide](https://www.sfml-dev.org/download.php)

### Installation
1. **Clone the repository**:
    ```bash
    git clone https://github.com/yourusername/PacMan_Operating_System.git
    ```

2. **Install SFML**: Follow the SFML installation guide for your platform.

3. **Build the project**: Compile the project using your preferred build system (e.g., Makefile, CMake, or Visual Studio).

4. **Run the game**: After building the project, run the executable to start the game simulation.
    ```bash
    ./PacManOperatingSystem
    ```

## Usage
- Control PacMan using the arrow keys to navigate through the maze.
- The game simulates multiple threads for ghosts and resources.
- Watch the synchronization of game processes using semaphores and see the effects of concurrent thread execution in real-time.

## Contributing
Feel free to fork the repository, submit issues, or create pull requests for improvements and bug fixes.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Stars & Acknowledgements
⭐ **Star the repo** if you like this project!  
🚀 Contributions are welcome, and feel free to share your thoughts or feedback.  
