#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <thread>
#include <limits>

// --- Generator Angka Acak ---
std::mt19139 rng(std::chrono::steady_clock::now().time_since_epoch().count());

// --- Struktur & Enum ---

enum class Location { Foothills, PineForest, RockySlope, StormyRidge, Summit };

struct Player {
    std::string name;
    int hp = 100;
    int maxHp = 100;
    int stamina = 100;
    int maxStamina = 100;
    int warmth = 100;
    int maxWarmth = 100;

    std::map<std::string, int> inventory;
    Location currentLocation = Location::Foothills;
};

// --- Variabel Global ---
Player player;
bool isGameRunning = true;
int day = 1;
int hoursPassed = 0; // 0-23

// --- Fungsi Bantu ---
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void separator() { std::cout << "\n--------------------------------------------------\n"; }
void shortPause(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
void pause() {
    std::cout << "\n(Tekan Enter untuk melanjutkan...)";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// --- Sistem Inti ---

std::string getLocationName(Location loc) {
    switch (loc) {
        case Location::Foothills: return "Kaki Gunung";
        case Location::PineForest: return "Hutan Pinus";
        case Location::RockySlope: return "Lereng Bebatuan";
        case Location::StormyRidge: return "Punggungan Badai";
        case Location::Summit: return "Puncak Misteri";
        default: return "Tidak Diketahui";
    }
}

void advanceTime(int hours) {
    hoursPassed += hours;
    if (hoursPassed >= 24) {
        day++;
        hoursPassed -= 24;
        std::cout << "Matahari terbenam dan pagi baru telah tiba. Ini adalah hari ke-" << day << ".\n";
        shortPause(2000);
    }
    // Suhu lebih dingin di malam hari
    if (hoursPassed >= 18 || hoursPassed <= 6) {
        player.warmth -= 5 * hours;
        std::cout << "Udara malam yang dingin menusuk tulang.\n";
    }
}

void checkStatusEffects() {
    if (player.warmth < 20) {
        std::cout << "Kamu sangat kedinginan! Kamu mulai kehilangan nyawa!\n";
        player.hp -= 5;
    }
    if (player.stamina <= 0) {
        std::cout << "Kamu pingsan karena kelelahan!\n";
        player.hp -= 20; // Hukuman karena pingsan
        player.stamina = 10;
        advanceTime(4);
    }
}

void checkWinLoss() {
    if (player.hp <= 0) {
        clearScreen();
        std::cout << "Nyawamu telah habis. Pendakianmu berakhir tragis di lereng gunung...\n";
        std::cout << "--- GAME OVER ---\n";
        isGameRunning = false;
        pause();
    }
    if (player.currentLocation == Location::Summit) {
        clearScreen();
        std::cout << "*****************************************************\n";
        std::cout << "                 SELAMAT, PENDAKI!                   \n";
        std::cout << "*****************************************************\n";
        std::cout << "Setelah perjalanan yang berat, kamu akhirnya mencapai Puncak Misteri!\n";
        std::cout << "Pemandangan dari atas sini sungguh luar biasa. Namamu akan tercatat dalam sejarah!\n";
        std::cout << "--- KAMU MENANG! ---\n";
        isGameRunning = false;
        pause();
    }
}

void displayHUD() {
    std::cout << "Hari: " << day << " | Waktu: " << hoursPassed << ":00 | Lokasi: " << getLocationName(player.currentLocation) << "\n";
    std::cout << "HP: " << player.hp << "/" << player.maxHp 
              << " | Stamina: " << player.stamina << "/" << player.maxStamina 
              << " | Kehangatan: " << player.warmth << "/" << player.maxWarmth << "\n";
    separator();
}

void useInventory() {
    clearScreen();
    std::cout << "--- Ransel ---\n";
    if (player.inventory.empty()) {
        std::cout << "Ranselmu kosong.\n";
    } else {
        for (const auto& item : player.inventory) {
            std::cout << "- " << item.first << " x" << item.second << "\n";
        }
    }
    separator();
    std::cout << "Ketik nama item untuk digunakan (atau 'kembali'):\n> ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "kembali") return;
    
    if (player.inventory[choice] > 0) {
        if (choice == "Makanan") {
            player.stamina = std::min(player.maxStamina, player.stamina + 40);
            std::cout << "Kamu makan dan memulihkan 40 stamina.\n";
            player.inventory[choice]--;
        } else if (choice == "Air") {
            player.stamina = std::min(player.maxStamina, player.stamina + 20);
            std::cout << "Kamu minum dan memulihkan 20 stamina.\n";
            player.inventory[choice]--;
        } else if (choice == "P3K") {
            player.hp = std::min(player.maxHp, player.hp + 50);
            std::cout << "Kamu menggunakan P3K dan memulihkan 50 HP.\n";
            player.inventory[choice]--;
        } else {
             std::cout << "Item ini tidak bisa digunakan seperti itu.\n";
        }
    } else {
        std::cout << "Kamu tidak punya item itu.\n";
    }
    pause();
}

// --- Aksi & Lokasi ---

void rest(bool inTent) {
    if (inTent) {
        if (player.inventory["Tenda"] > 0) {
            if (player.stamina < 20) { std::cout << "Kamu butuh setidaknya 20 stamina untuk mendirikan tenda!\n"; pause(); return; }
            player.stamina -= 20;
            std::cout << "Kamu mendirikan tenda dan beristirahat dengan aman.\n";
            int staminaGain = 60;
            int warmthGain = 50;
            player.stamina = std::min(player.maxStamina, player.stamina + staminaGain);
            player.warmth = std::min(player.maxWarmth, player.warmth + warmthGain);
            std::cout << "Stamina pulih " << staminaGain << ", Kehangatan pulih " << warmthGain << ".\n";
            advanceTime(6);
        } else {
            std::cout << "Kamu tidak punya tenda!\n";
        }
    } else {
        std::cout << "Kamu beristirahat sejenak di tempat terbuka.\n";
        int staminaGain = 25;
        player.stamina = std::min(player.maxStamina, player.stamina + staminaGain);
        std::cout << "Stamina pulih " << staminaGain << ".\n";
        advanceTime(2);
    }
    pause();
}

void atFoothills() {
    std::cout << "Kamu berada di kaki gunung yang rimbun. Jalan setapak menanjak ke utara menuju Hutan Pinus.\n";
    std::cout << "[1] Mulai Mendaki ke Hutan Pinus\n";
    std::cout << "[2] Beristirahat Sejenak\n";
    std::cout << "[3] Periksa Ransel\n> ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        std::cout << "Kamu memulai pendakian...\n";
        player.stamina -= 20;
        player.warmth -= 10;
        advanceTime(3);
        player.currentLocation = Location::PineForest;
    } else if (choice == "2") {
        rest(false);
    } else if (choice == "3") {
        useInventory();
    }
}

void atPineForest() {
    std::cout << "Pohon-pohon pinus menjulang tinggi. Udara semakin dingin. Jalan setapak terus menanjak ke Lereng Bebatuan.\n";
    std::cout << "[1] Lanjut Mendaki ke Lereng Bebatuan\n";
    std::cout << "[2] Beristirahat di Tenda\n";
    std::cout << "[3] Periksa Ransel\n> ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        std::cout << "Pendakian semakin curam...\n";
        player.stamina -= 30;
        player.warmth -= 15;
        advanceTime(4);
        player.currentLocation = Location::RockySlope;
    } else if (choice == "2") {
        rest(true);
    } else if (choice == "3") {
        useInventory();
    }
}

void atRockySlope() {
     std::cout << "Medan berubah menjadi bebatuan terjal. Angin mulai bertiup kencang. Di depan ada Punggungan Badai yang tampak mengerikan.\n";
    std::cout << "[1] Daki ke Punggungan Badai (Sangat Berisiko)\n";
    std::cout << "[2] Beristirahat di Tenda\n";
    std::cout << "[3] Periksa Ransel\n> ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        std::cout << "Dengan sekuat tenaga, kamu memanjat bebatuan licin...\n";
        player.stamina -= 45;
        player.warmth -= 25;
        advanceTime(5);
        // Event acak: terpeleset
        if (rng() % 4 == 0) {
            std::cout << "Kamu terpeleset! Kakimu terkilir!\n";
            player.hp -= 20;
        }
        player.currentLocation = Location::StormyRidge;
    } else if (choice == "2") {
        rest(true);
    } else if (choice == "3") {
        useInventory();
    }
}

void atStormyRidge() {
    std::cout << "Badai salju menderu! Jarak pandang sangat pendek. Puncak sudah dekat!\n";
    // Efek cuaca buruk
    player.warmth -= 15;
    std::cout << "Badai menguras kehangatanmu!\n";

    std::cout << "[1] Dorong Diri Menuju Puncak! (Menguras Stamina)\n";
    std::cout << "[2] Bertahan di Tenda (Sangat Direkomendasikan)\n";
    std::cout << "[3] Periksa Ransel\n> ";
    std::string choice;
    std::getline(std::cin, choice);
    
    if (choice == "1") {
        std::cout << "Melawan badai, kamu mengambil langkah terakhir menuju puncak!\n";
        player.stamina -= 60;
        player.warmth -= 30;
        advanceTime(3);
        player.currentLocation = Location::Summit;
    } else if (choice == "2") {
        rest(true);
    } else if (choice == "3") {
        useInventory();
    }
}


// --- Main Function ---
int main() {
    // Inisialisasi Player
    player.inventory["Makanan"] = 3;
    player.inventory["Air"] = 5;
    player.inventory["Tenda"] = 1;
    player.inventory["P3K"] = 1;

    clearScreen();
    std::cout << "=======================================\n";
    std::cout << "||      PENDAKIAN PUNCAK MISTERI     ||\n";
    std::cout << "=======================================\n";
    std::cout << "Siapakah nama pendaki pemberani ini?\n> ";
    std::getline(std::cin, player.name);

    while (isGameRunning) {
        clearScreen();
        displayHUD();

        switch (player.currentLocation) {
            case Location::Foothills: atFoothills(); break;
            case Location::PineForest: atPineForest(); break;
            case Location::RockySlope: atRockySlope(); break;
            case Location::StormyRidge: atStormyRidge(); break;
            // Lokasi Puncak akan langsung memicu kemenangan
        }
        
        checkStatusEffects();
        checkWinLoss();
    }

    return 0;
}
