#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
using namespace std;

const int MANDATORY_REST_MINUTES = 15;

struct Flight {
    string id;
    int startMin;
    int endMin;
};

struct Crew {
    string id;
    int availableMin;
    bool operator<(const Crew& other) const {
        if (availableMin != other.availableMin)
            return availableMin < other.availableMin;
        return id > other.id; // changed: reverse ID order so lowest ID is picked first
    }
};

int toMinutes(int h, int m) { return (h * 60) + m; }
string toTimeStr(int totalMin) {
    int h = (totalMin / 60) % 24;
    int m = totalMin % 60;
    string hStr = (h < 10) ? "0" + to_string(h) : to_string(h);
    string mStr = (m < 10) ? "0" + to_string(m) : to_string(m);
    return hStr + ":" + mStr;
}
bool compareFlights(const Flight& a, const Flight& b) {
    return a.startMin < b.startMin;
}

// Enum for assignment status
enum Status { ASSIGNED, FAILED };

int main() {
    int n;
    cout << "   AIRLINE CREW SCHEDULING SYSTEM\n";
    cout << "Enter Total Number of Flights: ";
    cin >> n;

    vector<Flight> flights(n);
    cout << "\n--- ENTER FLIGHT DETAILS ---\n";
    for (int i = 0; i < n; i++) {
        string fid; int sH, sM, eH, eM;
        cout << "Flight " << (i + 1) << ": ";
        cin >> fid >> sH >> sM >> eH >> eM;
        flights[i].id = fid;
        flights[i].startMin = toMinutes(sH, sM);
        flights[i].endMin = toMinutes(eH, eM);
    }

    int m;
    cout << "\nEnter Total Number of Crew Members: ";
    cin >> m;
    multiset<Crew> crewPool;
    for (int i = 0; i < m; i++) {
        string cid;
        cout << "Crew " << (i + 1) << " ID: ";
        cin >> cid;
        crewPool.insert({cid, 0});
    }

    sort(flights.begin(), flights.end(), compareFlights);

    int assignedCount = 0;

    cout << "\n       GENERATED SCHEDULE\n";

    for (const auto& flight : flights) {
        Crew tempSearch;
        tempSearch.availableMin = flight.startMin;
        tempSearch.id = "{";

        auto it = crewPool.upper_bound(tempSearch);

        Status currentStatus;

        if (it == crewPool.begin()) {
            currentStatus = FAILED;
        } else {
            --it;
            Crew selectedCrew = *it;
            crewPool.erase(it);

            int idleTime = flight.startMin - selectedCrew.availableMin;

            cout << "Flight " << flight.id
                 << " (" << toTimeStr(flight.startMin) << " - " << toTimeStr(flight.endMin) << ")"
                 << " -> Crew " << selectedCrew.id
                 << " (Idle: " << idleTime << "m)\n";

            selectedCrew.availableMin = flight.endMin + MANDATORY_REST_MINUTES;
            crewPool.insert(selectedCrew);
            assignedCount++;
            currentStatus = ASSIGNED;
        }

        // Switch case for output
        switch (currentStatus) {
            case ASSIGNED:
                cout << "[SUCCESS] Flight " << flight.id << " assigned successfully.\n";
                break;
            case FAILED:
                cout << "[FAILED] Flight " << flight.id
                     << " (" << toTimeStr(flight.startMin) << "): No Crew Available.\n";
                break;
        }
        cout << "---------------------------------------------\n";
    }

    cout << "Total Flights: " << n << endl;
    cout << "Successful Assignments: " << assignedCount << endl;
    cout << "Unassigned Flights: " << (n - assignedCount) << endl;

    return 0;
}