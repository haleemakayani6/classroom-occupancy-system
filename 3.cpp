#include <iostream>
#include <string>
#include <limits>
using namespace std;

const int MAX_STUDENTS = 100;
const int MAX_LOGS = 500;
const int MAX_CLASSROOMS = 50;

// Base class
class Person {
protected:
    string name;
    int id;
public:
    Person(string n = "", int i = 0) {
        name = n;
        id = i;
    }
    virtual void displayInfo() const = 0;
    string getName() const { return name; }
    int getId() const { return id; }
};

// Student class
class Student : public Person {
    string program;
public:
    Student(string n = "", int i = 0, string p = "") : Person(n, i), program(p) {}
    void displayInfo() const override {
        cout << "Student - ID: " << id << ", Name: " << name << ", Program: " << program << endl;
    }
};

// Teacher class
class Teacher : public Person {
    string subject;
public:
    Teacher(string n = "", int i = 0, string s = "") : Person(n, i), subject(s) {}
    void displayInfo() const override {
        cout << "Teacher - ID: " << id << ", Name: " << name << ", Subject: " << subject << endl;
    }
};

// Classroom class
class Classroom {
    int roomNumber, capacity;
    Teacher* teacher;
    Student students[MAX_STUDENTS];
    string logs[MAX_LOGS];
    int studentCount = 0;
    int logCount = 0;
public:
    Classroom(int r = 0, int c = 0) : roomNumber(r), capacity(c), teacher(nullptr) {}
    ~Classroom() { delete teacher; }

    int getRoomNumber() const { return roomNumber; }
    int getCapacity() const { return capacity; }
    int getOccupancy() const { return studentCount; }
    Teacher* getTeacher() const { return teacher; }
    const Student* getStudents() const { return students; }

    string getOccupancyStatus() const {
        if (studentCount == 0) return "Empty";
        if (studentCount == capacity) return "Full";
        return "Partially Occupied (" + to_string((studentCount * 100) / capacity) + "%)";
    }

    void assignTeacher(const Teacher& t) {
        if (teacher) {
            logs[logCount++] = "Replaced teacher " + teacher->getName();
            delete teacher;
        }
        teacher = new Teacher(t);
        logs[logCount++] = "Assigned teacher " + t.getName();
    }

    bool addStudent(const Student& s) {
        if (studentCount >= capacity) {
            cout << "Room is full!\n";
            return false;
        }
        for (int i = 0; i < studentCount; ++i) {
            if (students[i].getId() == s.getId()) {
                cout << "Student ID already exists.\n";
                return false;
            }
        }
        students[studentCount++] = s;
        logs[logCount++] = "Added student " + s.getName();
        return true;
    }

    bool removeStudent(int sid) {
        for (int i = 0; i < studentCount; ++i) {
            if (students[i].getId() == sid) {
                logs[logCount++] = "Removed student " + students[i].getName();
                for (int j = i; j < studentCount - 1; ++j) students[j] = students[j + 1];
                studentCount--;
                return true;
            }
        }
        return false;
    }

    bool hasStudent(int sid) const {
        for (int i = 0; i < studentCount; ++i)
            if (students[i].getId() == sid) return true;
        return false;
    }

    void displayDetails() const {
        cout << "Classroom " << roomNumber << " [Capacity: " << capacity << ", Occupied: " << studentCount << ", Status: " << getOccupancyStatus() << "]\n";
        int bars = (studentCount * 10) / capacity;
        cout << "Occupancy: [";
        for (int i = 0; i < 10; ++i) cout << (i < bars ? "#" : " ");
        cout << "] " << (studentCount * 100) / capacity << "%\n";

        cout << "Teacher:\n";
        if (teacher) teacher->displayInfo();
        else cout << " - No teacher assigned.\n";

        cout << "Students:\n";
        if (studentCount == 0) cout << " - No students.\n";
        else for (int i = 0; i < studentCount; ++i) students[i].displayInfo();

        cout << "Logs:\n";
        if (logCount == 0) cout << " - No logs.\n";
        else for (int i = 0; i < logCount; ++i) cout << " - " << logs[i] << endl;
    }

    bool transferStudentTo(Classroom& dest, int sid) {
        for (int i = 0; i < studentCount; ++i) {
            if (students[i].getId() == sid) {
                if (dest.addStudent(students[i])) {
                    removeStudent(sid);
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    void save() const {
        cout << "Room:" << roomNumber << "|Capacity:" << capacity << "|Students:";
        for (int j = 0; j < studentCount; j++) {
            cout << students[j].getId() << "," << students[j].getName() << ";";
        }
        cout << "|Teacher:";
        if (teacher) cout << teacher->getId() << "," << teacher->getName();
        cout << "\n";
    }
};

// Building class
class Building {
    string name;
    Classroom classrooms[MAX_CLASSROOMS];
    int classroomCount = 0;
public:
    Building(string n = "") : name(n) {}

    void addClassroom(int room, int cap) {
        if (cap <= 0 || classroomCount >= MAX_CLASSROOMS) {
            cout << "Invalid capacity or room limit reached.\n";
            return;
        }
        for (int i = 0; i < classroomCount; ++i) {
            if (classrooms[i].getRoomNumber() == room) {
                cout << "Room already exists.\n";
                return;
            }
        }
        classrooms[classroomCount++] = Classroom(room, cap);
        cout << "Classroom added.\n";
    }

    Classroom* getClassroomByNumber(int room) {
        for (int i = 0; i < classroomCount; ++i)
            if (classrooms[i].getRoomNumber() == room)
                return &classrooms[i];
        return nullptr;
    }

    void displayAllClassrooms() const {
        if (classroomCount == 0) cout << "No classrooms in building.\n";
        else for (int i = 0; i < classroomCount; ++i) {
            classrooms[i].displayDetails();
            cout << "------------------\n";
        }
    }

    void searchStudentById(int sid) const {
        for (int i = 0; i < classroomCount; ++i)
            if (classrooms[i].hasStudent(sid)) {
                cout << "Student found in Room " << classrooms[i].getRoomNumber() << ".\n";
                return;
            }
        cout << "Student not found.\n";
    }

    void searchTeacherById(int tid) const {
        for (int i = 0; i < classroomCount; ++i)
            if (classrooms[i].getTeacher() && classrooms[i].getTeacher()->getId() == tid) {
                cout << "Teacher found in Room " << classrooms[i].getRoomNumber() << ".\n";
                return;
            }
        cout << "Teacher not found.\n";
    }

    void transferStudent(int sid, int fromRoom, int toRoom) {
        Classroom* from = getClassroomByNumber(fromRoom);
        Classroom* to = getClassroomByNumber(toRoom);
        if (!from || !to) cout << "Invalid room(s).\n";
        else if (from->transferStudentTo(*to, sid)) cout << "Student transferred.\n";
        else cout << "Transfer failed.\n";
    }

    void showSummary() const {
        cout << "Building Summary: " << name << "\n";
        int totalStudents = 0, totalCapacity = 0;
        for (int i = 0; i < classroomCount; ++i) {
            totalStudents += classrooms[i].getOccupancy();
            totalCapacity += classrooms[i].getCapacity();
            cout << "Room " << classrooms[i].getRoomNumber() << ": " << classrooms[i].getOccupancy() << "/" << classrooms[i].getCapacity() << "\n";
        }
        cout << "Total Students: " << totalStudents << "\n";
        cout << "Total Capacity: " << totalCapacity << "\n";
        cout << "Occupancy: " << (totalCapacity ? (totalStudents * 100) / totalCapacity : 0) << "%\n";
    }

    void saveData() const {
        for (int i = 0; i < classroomCount; ++i)
            classrooms[i].save();
    }
};

// Helper functions
void clearInputBuffer() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
void pause() { cout << "Press Enter to continue..."; cin.get(); }
int getValidInt(string prompt) {
    int val;
    cout << prompt;
    cin >> val;
    while (!cin.good() || val < 0) {
        cin.clear(); clearInputBuffer();
        cout << "Invalid input. Enter positive number: "; cin >> val;
    }
    return val;
}
string getValidString(string prompt) {
    string input;
    cout << prompt;
    clearInputBuffer();
    getline(cin, input);
    return input.empty() ? getValidString(prompt) : input;
}

// Updated menu functions
void addClassroom(Building& b) {
    int again = 1;
    while (again) {
        int r = getValidInt("Enter room number: ");
        int c = getValidInt("Enter capacity: ");
        b.addClassroom(r, c);
        cout << "Add another classroom? (1 = Yes, 0 = No): ";
        cin >> again;
    }
}
void assignTeacher(Building& b) {
    int again = 1;
    while (again) {
        int r = getValidInt("Enter room number: ");
        Classroom* c = b.getClassroomByNumber(r);
        if (!c) { cout << "Room not found.\n"; }
        else {
            string name = getValidString("Enter teacher name: ");
            int id = getValidInt("Enter teacher ID: ");
            string subj = getValidString("Enter subject: ");
            c->assignTeacher(Teacher(name, id, subj));
        }
        cout << "Assign another teacher? (1 = Yes, 0 = No): ";
        cin >> again;
    }
}
void addStudent(Building& b) {
    int again = 1;
    while (again) {
        int r = getValidInt("Enter room number: ");
        Classroom* c = b.getClassroomByNumber(r);
        if (!c) { cout << "Room not found.\n"; }
        else {
            string name = getValidString("Enter student name: ");
            int id = getValidInt("Enter student ID: ");
            string prog = getValidString("Enter program: ");
            c->addStudent(Student(name, id, prog));
        }
        cout << "Add another student? (1 = Yes, 0 = No): ";
        cin >> again;
    }
}
void removeStudent(Building& b) {
    int r = getValidInt("Enter room number: ");
    Classroom* c = b.getClassroomByNumber(r);
    if (!c) { cout << "Room not found.\n"; return; }
    int sid = getValidInt("Enter student ID: ");
    if (!c->removeStudent(sid)) cout << "Student not found.\n";
}
void mainMenu(Building& b) {
    int ch;
    do {
        cout << "\n==== Classroom Occupancy Menu ====\n"
             << "1. Add Classroom\n2. Assign Teacher\n3. Add Student\n4. Remove Student\n"
             << "5. Show All Classrooms\n6. Search Student\n7. Search Teacher\n8. Transfer Student\n"
             << "9. Show Summary\n10. Save Data\n11. Exit\nChoice: ";
        ch = getValidInt("");
        switch (ch) {
            case 1: addClassroom(b); break;
            case 2: assignTeacher(b); break;
            case 3: addStudent(b); break;
            case 4: removeStudent(b); break;
            case 5: b.displayAllClassrooms(); break;
            case 6: b.searchStudentById(getValidInt("Enter student ID: ")); break;
            case 7: b.searchTeacherById(getValidInt("Enter teacher ID: ")); break;
            case 8: {
                int sid = getValidInt("Enter student ID: ");
                int from = getValidInt("From room: "), to = getValidInt("To room: ");
                b.transferStudent(sid, from, to); break;
            }
            case 9: b.showSummary(); break;
            case 10: b.saveData(); break;
            case 11: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
        if (ch != 11) pause();
    } while (ch != 11);
}

// Main function
int main() {
    Building b("Academic Block");
    mainMenu(b);
    return 0;
}

