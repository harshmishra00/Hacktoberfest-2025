#include <bits/stdc++.h>
using namespace std;

struct Task {
    int id;
    string title;
    string dueDate;
    int priority; // 1=Low, 2=Medium, 3=High
    bool done;
    string notes;
    time_t created;

    Task() : id(0), priority(2), done(false), created(time(nullptr)) {}
};

class TaskManager {
    vector<Task> tasks;
    int nextId;
    const string filename = "tasks.db";

public:
    TaskManager() : nextId(1) {
        load();
    }

    ~TaskManager() {
        save();
    }

    void addTask() {
        Task t;
        t.id = nextId++;

        cout << "Title: ";
        getline(cin >> ws, t.title);
        if (t.title.empty()) {
            cout << "Title is required.\n";
            return;
        }

        cout << "Due date (YYYY-MM-DD or leave empty): ";
        getline(cin, t.dueDate);

        cout << "Priority (1=Low, 2=Medium, 3=High) [2]: ";
        string input;
        getline(cin, input);
        if (!input.empty()) t.priority = stoi(input);

        cout << "Notes (optional): ";
        getline(cin, t.notes);

        tasks.push_back(t);
        cout << "Task added successfully.\n";
    }

    void listTasks() {
        if (tasks.empty()) {
            cout << "No tasks available.\n";
            return;
        }

        sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
            if (a.done != b.done) return !a.done; // Pending first
            if (a.priority != b.priority) return a.priority > b.priority; // High first
            return a.created < b.created;
        });

        cout << "\n--- Task List ---\n";
        for (auto &t : tasks) {
            cout << "[" << t.id << "] "
                 << (t.done ? "[✔] " : "[ ] ")
                 << t.title
                 << " (Priority: " << priorityName(t.priority)
                 << ", Due: " << (t.dueDate.empty() ? "N/A" : t.dueDate)
                 << ")\n";
        }
    }

    void markDone(int id, bool status) {
        auto *t = findTask(id);
        if (!t) return;
        t->done = status;
        cout << "Task " << id << (status ? " marked done.\n" : " marked undone.\n");
    }

    void removeTask(int id) {
        auto it = remove_if(tasks.begin(), tasks.end(), [&](const Task &t) { return t.id == id; });
        if (it == tasks.end()) {
            cout << "Task not found.\n";
            return;
        }
        tasks.erase(it, tasks.end());
        cout << "Task removed.\n";
    }

    void viewTask(int id) {
        auto *t = findTask(id);
        if (!t) return;
        cout << "\n--- Task Details ---\n";
        cout << "ID: " << t->id << "\n"
             << "Title: " << t->title << "\n"
             << "Status: " << (t->done ? "Done" : "Pending") << "\n"
             << "Priority: " << priorityName(t->priority) << "\n"
             << "Due: " << (t->dueDate.empty() ? "N/A" : t->dueDate) << "\n"
             << "Notes: " << t->notes << "\n"
             << "Created: " << formatTime(t->created) << "\n";
    }

    void searchTask(const string &query) {
        string q = toLower(query);
        bool found = false;
        for (auto &t : tasks) {
            string all = toLower(t.title + " " + t.notes);
            if (all.find(q) != string::npos) {
                cout << "[" << t.id << "] " << t.title << "\n";
                found = true;
            }
        }
        if (!found) cout << "No matching tasks.\n";
    }

    void save() {
        ofstream out(filename);
        if (!out) return;
        for (auto &t : tasks) {
            out << t.id << "|" << t.done << "|" << t.priority << "|"
                << t.created << "|" << t.dueDate << "|" << t.title << "|" << t.notes << "\n";
        }
    }

    void load() {
        ifstream in(filename);
        if (!in) return;
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            Task t;
            string token;

            getline(ss, token, '|'); t.id = stoi(token);
            getline(ss, token, '|'); t.done = stoi(token);
            getline(ss, token, '|'); t.priority = stoi(token);
            getline(ss, token, '|'); t.created = stol(token);
            getline(ss, t.dueDate, '|');
            getline(ss, t.title, '|');
            getline(ss, t.notes);
            tasks.push_back(t);
            nextId = max(nextId, t.id + 1);
        }
    }

private:
    static string priorityName(int p) {
        switch (p) {
            case 1: return "Low";
            case 2: return "Medium";
            case 3: return "High";
            default: return "?";
        }
    }

    static string formatTime(time_t t) {
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&t));
        return buf;
    }

    static string toLower(string s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    Task* findTask(int id) {
        for (auto &t : tasks)
            if (t.id == id) return &t;
        cout << "Task not found.\n";
        return nullptr;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    TaskManager tm;
    cout << "best_way — C++ Task Manager\n";
    cout << "Type 'help' for commands.\n";

    string cmd;
    while (true) {
        cout << "\n>>> ";
        if (!(cin >> cmd)) break;

        if (cmd == "add") tm.addTask();
        else if (cmd == "list") tm.listTasks();
        else if (cmd == "view") { int id; cin >> id; tm.viewTask(id); }
        else if (cmd == "done") { int id; cin >> id; tm.markDone(id, true); }
        else if (cmd == "undone") { int id; cin >> id; tm.markDone(id, false); }
        else if (cmd == "rm") { int id; cin >> id; tm.removeTask(id); }
        else if (cmd == "search") {
            string q; getline(cin >> ws, q);
            tm.searchTask(q);
        }
        else if (cmd == "help") {
            cout << "Commands:\n"
                 << "  add        Add a new task\n"
                 << "  list       List all tasks\n"
                 << "  view <id>  View task details\n"
                 << "  done <id>  Mark as done\n"
                 << "  undone <id>Mark as not done\n"
                 << "  rm <id>    Remove task\n"
                 << "  search <q> Search tasks\n"
                 << "  quit       Exit\n";
        }
        else if (cmd == "quit" || cmd == "exit") break;
        else cout << "Unknown command. Type 'help' for list.\n";
    }

    cout << "\nGoodbye!\n";
    return 0;
}
