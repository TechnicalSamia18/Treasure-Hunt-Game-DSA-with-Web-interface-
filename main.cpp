#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

using namespace std;

const int MAX_ROOMS = 20;
const int PORT = 8080;

// Game state
int adj[MAX_ROOMS][MAX_ROOMS];
string rooms[MAX_ROOMS];
bool treasureInRoom[MAX_ROOMS];
bool originalTreasure[MAX_ROOMS];
int roomCount = 0;
string currentRoom = "Entrance";
int treasuresFound = 0;
int moves = 0;
int maxMoves = 8;
bool hintUsed = false;

int getRoomIndex(string name) {
    for (int i = 0; i < roomCount; i++)
        if (rooms[i] == name) return i;
    return -1;
}

void addRoom(string name) {
    rooms[roomCount] = name;
    treasureInRoom[roomCount] = false;
    originalTreasure[roomCount] = false;
    roomCount++;
}

void addPath(string room1, string room2) {
    int i = getRoomIndex(room1);
    int j = getRoomIndex(room2);
    if (i != -1 && j != -1) {
        adj[i][j] = 1;
        adj[j][i] = 1;
    }
}

// BFS to find shortest path
vector<string> findShortestPath(string start, string end) {
    vector<string> path;
    int startIdx = getRoomIndex(start);
    int endIdx = getRoomIndex(end);
    
    if (startIdx == -1 || endIdx == -1) return path;
    
    // BFS
    queue<int> q;
    vector<bool> visited(roomCount, false);
    vector<int> parent(roomCount, -1);
    
    q.push(startIdx);
    visited[startIdx] = true;
    
    while (!q.empty()) {
        int current = q.front();
        q.pop();
        
        if (current == endIdx) break;
        
        for (int i = 0; i < roomCount; i++) {
            if (adj[current][i] && !visited[i]) {
                visited[i] = true;
                parent[i] = current;
                q.push(i);
            }
        }
    }
    
    // Reconstruct path
    if (visited[endIdx]) {
        int current = endIdx;
        while (current != -1) {
            path.insert(path.begin(), rooms[current]);
            current = parent[current];
        }
    }
    
    return path;
}

string getHint() {
    if (hintUsed) {
        return "Out of hints! You've already used your one hint for this quest.";
    }
    
    for (int i = 0; i < roomCount; i++) {
        if (treasureInRoom[i]) {
            hintUsed = true;
            
            if (rooms[i] == "Armory") return "The treasure lies where weapons rest in silence.";
            if (rooms[i] == "TreasureRoom") return "The treasure lies where riches are locked away.";
            if (rooms[i] == "Dungeon") return "The treasure lies deep underground, cold and dark.";
            if (rooms[i] == "Library") return "The treasure lies where knowledge rests and dust gathers.";
            if (rooms[i] == "Kitchen") return "The treasure lies where food fills the air with warmth.";
            if (rooms[i] == "Garden") return "The treasure lies where flowers bloom and secrets grow.";
            if (rooms[i] == "Observatory") return "The treasure lies where stars are watched at night.";
            if (rooms[i] == "Hall") return "The treasure lies where footsteps echo endlessly.";
            if (rooms[i] == "Balcony") return "The treasure lies where winds whisper tales.";
            
            return "A treasure awaits in " + rooms[i] + "...";
        }
    }
    
    return "No treasures remain to find!";
}

vector<string> getAdjacentRooms(string room) {
    vector<string> adjacent;
    int idx = getRoomIndex(room);
    if (idx == -1) return adjacent;
    
    for (int i = 0; i < roomCount; i++) {
        if (adj[idx][i]) {
            adjacent.push_back(rooms[i]);
        }
    }
    return adjacent;
}

string movePlayer(string targetRoom) {
    if (treasuresFound >= 3) {
        return "ERROR:Game already won";
    }
    if (moves >= maxMoves) {
        return "ERROR:Out of moves";
    }
    
    int currentIdx = getRoomIndex(currentRoom);
    int targetIdx = getRoomIndex(targetRoom);
    
    if (targetIdx == -1) {
        return "ERROR:Room not found";
    }
    
    if (!adj[currentIdx][targetIdx]) {
        return "ERROR:Rooms are not connected";
    }
    
    currentRoom = targetRoom;
    moves++;
    
    if (treasureInRoom[targetIdx]) {
        treasureInRoom[targetIdx] = false;
        treasuresFound++;
        return "TREASURE:Found treasure in " + targetRoom;
    }
    
    return "SUCCESS:Moved to " + targetRoom;
}

void resetGame() {
    currentRoom = "Entrance";
    treasuresFound = 0;
    moves = 0;
    hintUsed = false;
    
    for (int i = 0; i < roomCount; i++) {
        treasureInRoom[i] = false;
        originalTreasure[i] = false;
    }
    
    int totalTreasures = 3;
    int assigned = 0;
    
    while (assigned < totalTreasures) {
        int r = rand() % roomCount;
        
        if (rooms[r] != "Entrance" && !treasureInRoom[r]) {
            treasureInRoom[r] = true;
            originalTreasure[r] = true;
            assigned++;
            cout << "Treasure placed in: " << rooms[r] << endl;
        }
    }
}

string getGameState() {
    stringstream ss;
    ss << "{";
    ss << "\"currentRoom\":\"" << currentRoom << "\",";
    ss << "\"treasuresFound\":" << treasuresFound << ",";
    ss << "\"moves\":" << moves << ",";
    ss << "\"maxMoves\":" << maxMoves << ",";
    ss << "\"hintUsed\":" << (hintUsed ? "true" : "false") << ",";
    ss << "\"gameOver\":" << ((treasuresFound >= 3 || moves >= maxMoves) ? "true" : "false") << ",";
    ss << "\"won\":" << ((treasuresFound >= 3 && moves <= maxMoves) ? "true" : "false") << ",";
    ss << "\"treasureLocations\":[";
    
    bool first = true;
    for (int i = 0; i < roomCount; i++) {
        if (originalTreasure[i]) {
            if (!first) ss << ",";
            ss << "\"" << rooms[i] << "\"";
            first = false;
        }
    }
    ss << "],";
    
    ss << "\"rooms\":[";
    
    for (int i = 0; i < roomCount; i++) {
        if (i > 0) ss << ",";
        ss << "{\"name\":\"" << rooms[i] << "\",";
        ss << "\"hasTreasure\":" << (treasureInRoom[i] ? "true" : "false") << ",";
        ss << "\"adjacent\":[";
        
        vector<string> adj = getAdjacentRooms(rooms[i]);
        for (size_t j = 0; j < adj.size(); j++) {
            if (j > 0) ss << ",";
            ss << "\"" << adj[j] << "\"";
        }
        ss << "]}";
    }
    ss << "]}";
    return ss.str();
}

void initializeGame() {
    srand(time(0));
    
    for (int i = 0; i < MAX_ROOMS; i++)
        for (int j = 0; j < MAX_ROOMS; j++)
            adj[i][j] = 0;

    addRoom("Entrance");
    addRoom("Hall");
    addRoom("Armory");
    addRoom("TreasureRoom");
    addRoom("Library");
    addRoom("Kitchen");
    addRoom("Dungeon");
    addRoom("Observatory");
    addRoom("Garden");
    addRoom("Balcony");

    addPath("Entrance", "Hall");
    addPath("Entrance", "Library");
    addPath("Hall", "Armory");
    addPath("Hall", "Library");
    addPath("Hall", "Dungeon");
    addPath("Library", "Kitchen");
    addPath("Library", "Armory");
    addPath("Library", "Observatory");
    addPath("Armory", "TreasureRoom");
    addPath("Kitchen", "TreasureRoom");
    addPath("Kitchen", "Dungeon");
    addPath("Kitchen", "Garden");
    addPath("Observatory", "Balcony");
    addPath("Garden", "Balcony");

    cout << "Castle initialized with " << roomCount << " rooms" << endl;
    resetGame();
}

string handleRequest(string request) {
    if (request.find("OPTIONS") == 0) {
        return "OPTIONS";
    }
    
    if (request.find("GET /api/state") != string::npos) {
        return getGameState();
    }
    else if (request.find("GET /api/move?room=") != string::npos) {
        size_t pos = request.find("room=");
        if (pos != string::npos) {
            size_t end = request.find(" ", pos);
            if (end == string::npos) end = request.find("&", pos);
            if (end == string::npos) end = request.length();
            
            string room = request.substr(pos + 5, end - pos - 5);
            
            size_t spacePos;
            while ((spacePos = room.find("%20")) != string::npos) {
                room.replace(spacePos, 3, " ");
            }
            
            string result = movePlayer(room);
            
            stringstream ss;
            ss << "{\"success\":" << (result.find("SUCCESS") != string::npos || result.find("TREASURE") != string::npos ? "true" : "false") << ",";
            ss << "\"message\":\"" << result << "\",";
            ss << "\"foundTreasure\":" << (result.find("TREASURE") != string::npos ? "true" : "false") << "}";
            return ss.str();
        }
    }
    else if (request.find("GET /api/hint") != string::npos) {
        string hint = getHint();
        return "{\"hint\":\"" + hint + "\",\"used\":" + (hintUsed ? "true" : "false") + "}";
    }
    else if (request.find("GET /api/reset") != string::npos) {
        resetGame();
        return "{\"success\":true,\"message\":\"Game reset\"}";
    }
    else if (request.find("GET /api/path?") != string::npos) {
        size_t startPos = request.find("start=");
        size_t endPos = request.find("end=");
        
        if (startPos != string::npos && endPos != string::npos) {
            size_t startEnd = request.find("&", startPos);
            size_t endEnd = request.find(" ", endPos);
            if (endEnd == string::npos) endEnd = request.find("&", endPos);
            if (endEnd == string::npos) endEnd = request.length();
            
            string startRoom = request.substr(startPos + 6, startEnd - startPos - 6);
            string endRoom = request.substr(endPos + 4, endEnd - endPos - 4);
            
            vector<string> path = findShortestPath(startRoom, endRoom);
            
            stringstream ss;
            ss << "{\"path\":[";
            for (size_t i = 0; i < path.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << path[i] << "\"";
            }
            ss << "]}";
            return ss.str();
        }
    }
    
    return "{\"error\":\"Unknown endpoint\"}";
}

int main() {
    cout << "=== C++ Treasure Hunt Server ===" << endl;
    cout << "==================================" << endl;
    
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }
    #endif
    
    initializeGame();
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed" << endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(serverSocket);
        return 1;
    }
    
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        closesocket(serverSocket);
        return 1;
    }
    
    cout << "\n?? Server running on http://localhost:" << PORT << endl;
    cout << "?? Open index.html in your browser to play!" << endl;
    cout << "?? Press Ctrl+C to stop server\n" << endl;
    cout << "Waiting for connections...\n" << endl;
    
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket == INVALID_SOCKET) continue;
        
        char buffer[4096] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);
        string request(buffer);
        
        string requestLine = request.substr(0, request.find("\r\n"));
        cout << "?? " << requestLine << endl;
        
        string response = handleRequest(request);
        
        stringstream httpResponse;
        
        if (response == "OPTIONS") {
            httpResponse << "HTTP/1.1 204 No Content\r\n";
            httpResponse << "Access-Control-Allow-Origin: *\r\n";
            httpResponse << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
            httpResponse << "Access-Control-Allow-Headers: Content-Type\r\n";
            httpResponse << "Access-Control-Max-Age: 86400\r\n";
            httpResponse << "\r\n";
        } else {
            httpResponse << "HTTP/1.1 200 OK\r\n";
            httpResponse << "Content-Type: application/json\r\n";
            httpResponse << "Access-Control-Allow-Origin: *\r\n";
            httpResponse << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
            httpResponse << "Access-Control-Allow-Headers: Content-Type\r\n";
            httpResponse << "Content-Length: " << response.length() << "\r\n";
            httpResponse << "\r\n";
            httpResponse << response;
        }
        
        string fullResponse = httpResponse.str();
        send(clientSocket, fullResponse.c_str(), fullResponse.length(), 0);
        
        closesocket(clientSocket);
    }
    
    closesocket(serverSocket);
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    return 0;
}
