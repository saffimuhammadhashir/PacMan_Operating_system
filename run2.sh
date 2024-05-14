#     https://github.com/shiftkey/desktop?tab=readme-ov-file#installation-via-package-manager
rm -r sfml-app
g++ main2.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -pthread -lX11
./sfml-app
