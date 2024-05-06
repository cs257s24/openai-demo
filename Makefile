main: main.cpp openai.hpp nlohmann/json.hpp
	g++ main.cpp -std=c++17 -lcurl -o main

clean:
	rm -f main
