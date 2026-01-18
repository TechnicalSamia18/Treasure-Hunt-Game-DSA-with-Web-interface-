#include <iostream>
#include <string>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX_ROOMS = 20;

int adj[MAX_ROOMS][MAX_ROOMS];       // adjacency matrix
string rooms[MAX_ROOMS];
bool treasureInRoom[MAX_ROOMS];
bool originalTreasure[MAX_ROOMS];    // remembers original treasure spots
int roomCount = 0;

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

void showRooms() {
    cout << "==================== Rooms and Connections ====================\n";
    for (int i = 0; i < roomCount; i++) {
        cout << rooms[i] << ": ";
        for (int j = 0; j < roomCount; j++)
            if (adj[i][j]) cout << rooms[j] << " ";
        cout << endl;
    }
    cout << "===============================================================\n";
}

void shortestPath(string startName, string targetName) {
    int start = getRoomIndex(startName);
    int target = getRoomIndex(targetName);

    bool visited[MAX_ROOMS] = {false};
    int parent[MAX_ROOMS];
    for (int i = 0; i < MAX_ROOMS; i++) parent[i] = -1;

    queue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int curr = q.front(); q.pop();
        if (curr == target) break;
        for (int i = 0; i < roomCount; i++) {
            if (adj[curr][i] && !visited[i]) {
                visited[i] = true;
                parent[i] = curr;
                q.push(i);
            }
        }
    }

    if (!visited[target]) {
        cout << "No path to " << targetName << " found.\n";
        return;
    }

    int path[MAX_ROOMS], len = 0;
    for (int at = target; at != -1; at = parent[at])
        path[len++] = at;

    cout << "Treasure was hidden in " << targetName << "\n";
    cout << "Path: ";
    for (int i = len - 1; i >= 0; i--) cout << rooms[path[i]] << " ";
    cout << endl;
}

string getHint(string roomName) {
    if (roomName == "Armory") return "The treasure lies where weapons rest in silence.";
    if (roomName == "TreasureRoom") return "The treasure lies where riches are locked away.";
    if (roomName == "Dungeon") return "The treasure lies deep underground, cold and dark.";
    if (roomName == "Library") return "The treasure lies where knowledge rests and dust gathers.";
    if (roomName == "Kitchen") return "The treasure lies where food fills the air with warmth.";
    if (roomName == "Garden") return "The treasure lies where flowers bloom and secrets grow.";
    if (roomName == "Observatory") return "The treasure lies where stars are watched at night.";
    if (roomName == "Hall") return "The treasure lies where footsteps echo endlessly.";
    if (roomName == "Balcony") return "The treasure lies where winds whisper tales.";
    return "Somewhere hidden in the unknown halls...";
}

void giveHint(bool &hintUsed) {
    if (hintUsed) {
        cout << "Out of hints! You're on your own now.\n";
        return;
    }
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (treasureInRoom[i]) {
            cout << "Hint: " << getHint(rooms[i]) << endl;
            hintUsed = true;
            return;
        }
    }
}

int main() {
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

    addPath("Entrance","Hall");
    addPath("Entrance","Library");
    addPath("Hall","Armory");
    addPath("Hall","Library");
    addPath("Library","Kitchen");
    addPath("Library","Armory");
    addPath("Armory","TreasureRoom");
    addPath("Kitchen","TreasureRoom");
    addPath("Dungeon","Kitchen");
    addPath("Dungeon","Hall");
    addPath("Observatory","Balcony");
    addPath("Library","Observatory");
    addPath("Garden","Balcony");
    addPath("Kitchen","Garden");

    cout << "================ Welcome to the Random Treasure Hunt! ================\n";
    cout << "Generating a mysterious map... please wait.\n\n";
    showRooms();

    // Random treasures
    int totalTreasures = 3, assigned = 0;
    while (assigned < totalTreasures) {
        int r = rand() % roomCount;
        if (rooms[r] != "Entrance" && !treasureInRoom[r]) {
            treasureInRoom[r] = true;
            originalTreasure[r] = true;
            assigned++;
        }
    }

    string player = "Entrance";
    int found = 0, moves = 0;
    int maxMoves = 8;  // reduced from 20 to 8
    bool hintUsed = false;
    string move;

    while (found < totalTreasures && moves < maxMoves) {
        int idx = getRoomIndex(player);
        cout << "\n---------------- Move " << moves + 1 << " ----------------\n";
        cout << "Current Room: " << player << endl;
        cout << "Adjacent Rooms: ";
        for (int i = 0; i < roomCount; i++)
            if (adj[idx][i]) cout << rooms[i] << " ";
        cout << "\nMoves Left: " << maxMoves - moves
             << " | Treasures Found: " << found << "/" << totalTreasures << endl;

        cout << "Enter a room name or type 'hint': ";
        cin >> move;

        if (move == "hint") { giveHint(hintUsed); continue; }

        int moveIdx = getRoomIndex(move);
        if (moveIdx != -1 && adj[idx][moveIdx]) {
            player = move;
            moves++;
            if (treasureInRoom[moveIdx]) {
                cout << "You found a treasure in " << move << "!\n";
                treasureInRoom[moveIdx] = false;
                found++;
            }
        } else cout << "Invalid move! Try again.\n";
    }

    cout << "\n==================== Game Over ====================\n";
    if (found == totalTreasures)
        cout << "Congratulations! You found all treasures in " << moves << " moves.\n";
    else {
        cout << "Game Over! You ran out of moves.\n";
        cout << "Treasures collected: " << found << "/" << totalTreasures << endl;
    }

    cout << "\n================== Treasure Map Summary ==================\n";
    for (int i = 0; i < roomCount; i++)
        if (originalTreasure[i]) shortestPath("Entrance", rooms[i]);
    cout << "==========================================================\n";

    return 0;
}

