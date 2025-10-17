#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm> // Untuk std::find
#include <limits>

// --- Generator Angka Acak (Lebih Baik dari rand()) ---
std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

// --- Struktur Data ---

struct Player {
    std::string name;
    int level = 1;
    int xp = 0;
    int xpToNextLevel = 100;
    int gold = 20;
    int energy = 20;
    int maxEnergy = 20;
    int rodLevel = 1; // 1: Pancingan Bambu, 2: Grafit, 3: Serat Karbon
    std::map<std::string, int> baitInventory;
};

struct Fish {
    std::string name;
    std::string rarity; // Biasa, Langka, Legendaris
    int baseValue;
    int xpReward;
};

struct Location {
    std::string name;
    int requiredLevel;
    std::string requiredBait; // "Tidak ada" jika tidak perlu umpan khusus
    std::vector<std::string> fishAvailable; // Daftar nama ikan
};

// --- Database Game (Global) ---
Player player;
std::map<std::string, Fish> fishDatabase;
std::vector<Location> locationDatabase;

// --- Fungsi Bantu ---

void bersihkanLayar() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void garisPemisah() { std::cout << "\n--------------------------------------------------\n"; }
void jedaSingkat(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
void jeda() {
    std::cout << "\n(Tekan Enter untuk melanjutkan...)";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// --- Inisialisasi Data Game ---

void initializeGameData() {
    // Database Ikan
    fishDatabase["Lele"] = {"Lele", "Biasa", 10, 10};
    fishDatabase["Nila"] = {"Nila", "Biasa", 12, 12};
    fishDatabase["Sepatu Butut"] = {"Sampah", "Sampah", 1, 1};
    fishDatabase["Gurame"] = {"Langka", "Langka", 50, 40};
    fishDatabase["Patin"] = {"Langka", "Langka", 75, 50};
    fishDatabase["Tuna Sirip Biru"] = {"Legendaris", "Legendaris", 500, 300};
    fishDatabase["Marlin"] = {"Legendaris", "Legendaris", 600, 350};
    fishDatabase["Cumi-Cumi Raksasa"] = {"Legendaris", "Legendaris", 800, 500};

    // Database Lokasi
    locationDatabase.push_back({"Danau Desa", 1, "Tidak ada", {"Lele", "Nila", "Sepatu Butut"}});
    locationDatabase.push_back({"Sungai Deras", 3, "Umpan Cacing", {"Gurame", "Patin", "Nila"}});
    locationDatabase.push_back({"Laut Dalam", 8, "Umpan Udang", {"Tuna Sirip Biru", "Marlin", "Cumi-Cumi Raksasa"}});
}

// --- Fungsi Mekanik Game ---

void levelUp() {
    while (player.xp >= player.xpToNextLevel) {
        player.level++;
        player.xp -= player.xpToNextLevel;
        player.xpToNextLevel = static_cast<int>(player.xpToNextLevel * 1.7);
        player.maxEnergy += 5;
        player.energy = player.maxEnergy; // Energi pulih saat level up

        bersihkanLayar();
        std::cout << "******************************************\n";
        std::cout << "                 LEVEL UP!                \n";
        std::cout << "           Kamu mencapai Level " << player.level << "!\n";
        std::cout << "******************************************\n";
        std::cout << " Energi Maksimal meningkat -> " << player.maxEnergy << "\n";
        std::cout << " Energi telah pulih sepenuhnya!\n";
        jeda();
    }
}

bool reelingMinigame(const Fish& fish) {
    std::string sequence = "";
    int length = 0;
    float timeLimit = 0.0f;

    if (fish.rarity == "Biasa") { length = 3; timeLimit = 5.0f; }
    else if (fish.rarity == "Langka") { length = 4; timeLimit = 4.0f; }
    else if (fish.rarity == "Legendaris") { length = 6; timeLimit = 3.5f; }
    else return true; // Sampah tidak perlu ditarik

    std::uniform_int_distribution<int> charDist(0, 3);
    const char keys[] = {'a', 's', 'd', 'w'};
    for (int i = 0; i < length; ++i) {
        sequence += keys[charDist(rng)];
    }

    std::cout << "\nIkan melawan! Tarik dengan mengetik urutan ini:\n";
    std::cout << "-> " << sequence << " <-\n";
    std::cout << "Kamu punya " << timeLimit << " detik! Ketik sekarang!\n> ";

    auto startTime = std::chrono::high_resolution_clock::now();
    std::string userInput;
    std::getline(std::cin, userInput);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = endTime - startTime;

    if (elapsedTime.count() > timeLimit) {
        std::cout << "\nTerlalu lambat! Tali pancing putus!\n";
        return false;
    }
    if (userInput != sequence) {
        std::cout << "\nUrutan salah! Ikan berhasil melepaskan diri!\n";
        return false;
    }
    
    return true;
}

void startFishingSession(Location& loc) {
    std::map<std::string, int> fishCaughtInSession;

    while (true) {
        bersihkanLayar();
        std::cout << "Memancing di: " << loc.name << " | Energi: " << player.energy << "/" << player.maxEnergy << "\n";
        std::cout << "[1] Lempar Kail | [2] Selesai Memancing\n> ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice != "1") break;

        if (player.energy <= 0) {
            std::cout << "Kamu terlalu lelah untuk memancing. Pulang dan istirahatlah.\n";
            jedaSingkat(2000);
            break;
        }

        player.energy -= 5; // Biaya energi untuk melempar kail

        std::cout << "\nMelempar kail... sabar...\n";
        std::uniform_int_distribution<int> waitDist(2000, 6000);
        jedaSingkat(waitDist(rng));

        std::cout << "\n!!! KAIL DISAMBAR !!!\nTEKAN [ENTER] SEKARANG!\n";
        auto hookStartTime = std::chrono::high_resolution_clock::now();
        std::cin.get();
        auto hookEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> reactionTime = hookEndTime - hookStartTime;

        if (reactionTime.count() < 150 || reactionTime.count() > 2000) {
            std::cout << "Gagal mengait ikan! Ikannya lepas.\n";
            jedaSingkat(1500);
            continue;
        }

        std::cout << "Kena! Ikan terkait!\n";
        jedaSingkat(1000);
        
        // Menentukan ikan yang didapat
        std::uniform_int_distribution<int> fishChance(0, 100);
        int roll = fishChance(rng) + player.rodLevel * 5; // Pancingan bagus menambah peluang
        std::string fishName;

        if (roll >= 90 && std::any_of(loc.fishAvailable.begin(), loc.fishAvailable.end(), [](const std::string& name){ return fishDatabase.at(name).rarity == "Legendaris"; })) fishName = "Legendaris";
        else if (roll >= 65 && std::any_of(loc.fishAvailable.begin(), loc.fishAvailable.end(), [](const std::string& name){ return fishDatabase.at(name).rarity == "Langka"; })) fishName = "Langka";
        else fishName = "Biasa";

        std::vector<std::string> possibleCatches;
        for(const auto& name : loc.fishAvailable){
            if(fishDatabase.at(name).rarity == fishName || (fishName == "Biasa" && fishDatabase.at(name).rarity == "Sampah")){
                possibleCatches.push_back(name);
            }
        }
        if (possibleCatches.empty()) { // Fallback ke ikan biasa jika langka/legendaris tidak ada
             for(const auto& name : loc.fishAvailable){
                if(fishDatabase.at(name).rarity == "Biasa" || fishDatabase.at(name).rarity == "Sampah") possibleCatches.push_back(name);
             }
        }

        std::uniform_int_distribution<int> finalCatchDist(0, possibleCatches.size() - 1);
        Fish caughtFish = fishDatabase.at(possibleCatches[finalCatchDist(rng)]);

        if (reelingMinigame(caughtFish)) {
            std::cout << "\nBERHASIL! Kamu mendapatkan " << caughtFish.name << "!\n";
            fishCaughtInSession[caughtFish.name]++;
        }
        jeda();
    }

    // Selesai memancing, jual semua ikan
    if (!fishCaughtInSession.empty()) {
        bersihkanLayar();
        std::cout << "Sesi memancing selesai. Menjual hasil tangkapan...\n";
        int totalGold = 0;
        int totalXp = 0;
        for (const auto& pair : fishCaughtInSession) {
            const Fish& fish = fishDatabase.at(pair.first);
            int count = pair.second;
            totalGold += fish.baseValue * count;
            totalXp += fish.xpReward * count;
            std::cout << "- " << pair.first << " x" << count << " -> " << fish.baseValue * count << " Emas, " << fish.xpReward * count << " XP\n";
        }
        garisPemisah();
        std::cout << "Total didapat: " << totalGold << " Emas dan " << totalXp << " XP.\n";
        player.gold += totalGold;
        player.xp += totalXp;
        jeda();
        levelUp();
    }
}

// --- Fungsi Menu ---

void displayStats() {
    bersihkanLayar();
    const char* rodName[] = {"Bambu", "Grafit", "Serat Karbon"};
    garisPemisah();
    std::cout << "--- STATUS PEMANCING ---\n";
    std::cout << " Nama: " << player.name << "\n";
    std::cout << " Level: " << player.level << " (" << player.xp << "/" << player.xpToNextLevel << " XP)\n";
    std::cout << " Emas: " << player.gold << "\n";
    std::cout << " Energi: " << player.energy << "/" << player.maxEnergy << "\n";
    std::cout << " Pancingan: " << rodName[player.rodLevel - 1] << " (Lv. " << player.rodLevel << ")\n";
    std::cout << "--- UMPAN ---\n";
    if (player.baitInventory.empty()) std::cout << " Tidak ada umpan.\n";
    else {
        for(const auto& bait : player.baitInventory) std::cout << " " << bait.first << ": " << bait.second << "\n";
    }
    garisPemisah();
    jeda();
}

void visitShop() {
    const int rodUpgradeCost[] = {0, 250, 1000}; // Biaya upgrade ke Lv 2 & 3
    const int wormBaitCost = 15;
    const int shrimpBaitCost = 50;

    while (true) {
        bersihkanLayar();
        std::cout << "--- TOKO PAK KUMIS ---\n";
        std::cout << "Emas-mu: " << player.gold << "\n";
        garisPemisah();
        if (player.rodLevel < 3) {
            std::cout << "[1] Upgrade Pancingan ke Lv. " << player.rodLevel + 1 << " (" << rodUpgradeCost[player.rodLevel] << " Emas)\n";
        } else {
            std::cout << "[1] Pancingan sudah maksimal!\n";
        }
        std::cout << "[2] Beli Umpan Cacing (15 Emas)\n";
        std::cout << "[3] Beli Umpan Udang (50 Emas)\n";
        std::cout << "[4] Kembali\n> ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1" && player.rodLevel < 3) {
            if (player.gold >= rodUpgradeCost[player.rodLevel]) {
                player.gold -= rodUpgradeCost[player.rodLevel];
                player.rodLevel++;
                std::cout << "Selamat! Pancinganmu berhasil di-upgrade!\n";
            } else std::cout << "Emas tidak cukup!\n";
        } else if (choice == "2") {
            if (player.gold >= wormBaitCost) {
                player.gold -= wormBaitCost;
                player.baitInventory["Umpan Cacing"]++;
                std::cout << "Kamu membeli 1 Umpan Cacing.\n";
            } else std::cout << "Emas tidak cukup!\n";
        } else if (choice == "3") {
            if (player.gold >= shrimpBaitCost) {
                player.gold -= shrimpBaitCost;
                player.baitInventory["Umpan Udang"]++;
                std::cout << "Kamu membeli 1 Umpan Udang.\n";
            } else std::cout << "Emas tidak cukup!\n";
        } else if (choice == "4") break;
        else std::cout << "Pilihan tidak valid.\n";
        jedaSingkat(1500);
    }
}

void chooseLocation() {
    bersihkanLayar();
    std::cout << "--- PILIH LOKASI MEMANCING ---\n";
    for (size_t i = 0; i < locationDatabase.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << locationDatabase[i].name << " (Butuh Lv. " << locationDatabase[i].requiredLevel << ")\n";
    }
    std::cout << "[" << locationDatabase.size() + 1 << "] Kembali\n> ";
    std::string choice;
    std::getline(std::cin, choice);
    int idx = -1;
    try { idx = std::stoi(choice) - 1; } catch (...) {}

    if (idx >= 0 && idx < locationDatabase.size()) {
        Location& loc = locationDatabase[idx];
        if (player.level < loc.requiredLevel) {
            std::cout << "Level-mu belum cukup untuk memancing di sini!\n";
            jedaSingkat(1500);
            return;
        }
        if (loc.requiredBait != "Tidak ada" && player.baitInventory[loc.requiredBait] <= 0) {
            std::cout << "Kamu butuh " << loc.requiredBait << " untuk memancing di sini!\n";
            jedaSingkat(1500);
            return;
        }

        if (loc.requiredBait != "Tidak ada") {
            player.baitInventory[loc.requiredBait]--;
        }
        startFishingSession(loc);
    }
}

void restAtHome() {
    bersihkanLayar();
    std::cout << "Kamu pulang ke rumah untuk beristirahat...\n";
    jedaSingkat(2000);
    player.energy = player.maxEnergy;
    std::cout << "Zzz... Energi telah pulih sepenuhnya!\n";
    jeda();
}

int main() {
    initializeGameData();
    bersihkanLayar();
    std::cout << "=======================================\n";
    std::cout << "||   LEGENDA PEMANCING NUSANTARA     ||\n";
    std::cout << "=======================================\n";
    std::cout << "Siapakah nama pemancing legendaris kita?\n> ";
    std::getline(std::cin, player.name);

    while (true) {
        bersihkanLayar();
        garisPemisah();
        std::cout << "Halo, " << player.name << "! (Level " << player.level << ")\n";
        std::cout << "Emas: " << player.gold << " | Energi: " << player.energy << "/" << player.maxEnergy << "\n";
        garisPemisah();
        std::cout << "[1] Pergi Memancing\n";
        std::cout << "[2] Kunjungi Toko\n";
        std::cout << "[3] Istirahat di Rumah\n";
        std::cout << "[4] Lihat Status\n";
        std::cout << "[5] Keluar\n> ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") chooseLocation();
        else if (choice == "2") visitShop();
        else if (choice == "3") restAtHome();
        else if (choice == "4") displayStats();
        else if (choice == "5") {
            std::cout << "Terima kasih telah bermain! Lautan menantimu kembali!\n";
            break;
        }
    }
    return 0;
}
