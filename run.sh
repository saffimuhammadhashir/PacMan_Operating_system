#     https://github.com/shiftkey/desktop?tab=readme-ov-file#installation-via-package-manager
rm -r sfml-app
g++ main.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
./sfml-app
