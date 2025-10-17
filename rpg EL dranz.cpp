#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // Untuk rand() dan srand()
#include <ctime>   // Untuk time()
#include <limits>  // Untuk std::numeric_limits

// --- Struktur Data ---

struct Player {
    std::string name;
    int hp;
    int maxHp;
    int attack;
    int level;
    int xp;
    int xpToNextLevel;
    int gold;
    int potions;
};

struct Enemy {
    std::string name;
    int hp;
    int attack;
    int xpReward;
    int goldReward;
};

// --- Variabel Global ---
Player player;
bool isGameRunning = true;

// --- Fungsi Bantu ---

void bersihkanLayar() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void garisPemisah() {
    std::cout << "\n--------------------------------------------------\n";
}

void jeda() {
    std::cout << "\n(Tekan Enter untuk melanjutkan...)";
    // Membersihkan buffer input sebelum menunggu input baru
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// --- Fungsi Utama Game ---

void displayStats() {
    garisPemisah();
    std::cout << "--- STATUS " << player.name << " ---\n";
    std::cout << " Level: " << player.level << "\n";
    std::cout << " HP: " << player.hp << " / " << player.maxHp << "\n";
    std::cout << " Attack: " << player.attack << "\n";
    std::cout << " XP: " << player.xp << " / " << player.xpToNextLevel << "\n";
    std::cout << " Gold: " << player.gold << "\n";
    std::cout << " Potion: " << player.potions << "\n";
    garisPemisah();
}

void levelUp() {
    while (player.xp >= player.xpToNextLevel) {
        player.level++;
        player.xp -= player.xpToNextLevel;
        player.xpToNextLevel *= 1.5; // Kebutuhan XP meningkat 50% setiap level

        int hpGain = 15;
        int attackGain = 4;
        player.maxHp += hpGain;
        player.attack += attackGain;
        player.hp = player.maxHp; // Sembuh total saat level up

        bersihkanLayar();
        std::cout << "******************************************\n";
        std::cout << "                 LEVEL UP!                \n";
        std::cout << "           Kamu mencapai Level " << player.level << "!\n";
        std::cout << "******************************************\n";
        std::cout << " HP Maksimal meningkat -> " << player.maxHp << " (+" << hpGain << ")\n";
        std::cout << " Attack meningkat -> " << player.attack << " (+" << attackGain << ")\n";
        std::cout << " HP telah pulih sepenuhnya!\n";
        jeda();
    }
}

void startCombat(Enemy& enemy) {
    bersihkanLayar();
    std::cout << "!!! MUSUH MUNCUL !!!\n";
    std::cout << "Kamu berhadapan dengan " << enemy.name << "!\n";
    jeda();

    while (player.hp > 0 && enemy.hp > 0) {
        bersihkanLayar();
        std::cout << "--- PERTARUNGAN ---\n";
        std::cout << player.name << " (HP: " << player.hp << "/" << player.maxHp << ") vs "
                  << enemy.name << " (HP: " << enemy.hp << ")\n";
        garisPemisah();
        std::cout << "Pilih aksimu:\n";
        std::cout << "[1] Serang\n";
        std::cout << "[2] Gunakan Potion (" << player.potions << " tersisa)\n";
        std::cout << "[3] Coba Kabur\n";
        
        std::string pilihan;
        std::cout << "\n> ";
        std::getline(std::cin, pilihan);

        // Aksi Pemain
        if (pilihan == "1") {
            int damage = player.attack + (rand() % 5); // Damage + sedikit acak
            enemy.hp -= damage;
            std::cout << "Kamu menyerang " << enemy.name << " dan memberikan " << damage << " damage!\n";
        } else if (pilihan == "2") {
            if (player.potions > 0) {
                int healAmount = 40;
                player.hp += healAmount;
                if (player.hp > player.maxHp) player.hp = player.maxHp;
                player.potions--;
                std::cout << "Kamu menggunakan Potion dan memulihkan " << healAmount << " HP!\n";
            } else {
                std::cout << "Kamu tidak punya Potion!\n";
            }
        } else if (pilihan == "3") {
            if ((rand() % 100) < 50) { // 50% kesempatan kabur
                std::cout << "Kamu berhasil kabur dari pertarungan!\n";
                jeda();
                return;
            } else {
                std::cout << "Kamu gagal kabur!\n";
            }
        } else {
            std::cout << "Pilihan tidak valid, kamu kehilangan giliran!\n";
        }

        // Aksi Musuh
        if (enemy.hp > 0) {
            int enemyDamage = enemy.attack + (rand() % 3);
            player.hp -= enemyDamage;
            std::cout << enemy.name << " menyerangmu dan memberikan " << enemyDamage << " damage!\n";
        }

        jeda();
    }

    // Akhir Pertarungan
    if (player.hp <= 0) {
        bersihkanLayar();
        std::cout << "##########################################\n";
        std::cout << "#                KAMU KALAH              #\n";
        std::cout << "##########################################\n";
        std::cout << "Petualanganmu berakhir di sini.\n";
        isGameRunning = false;
        jeda();
    } else {
        std::cout << "Kamu berhasil mengalahkan " << enemy.name << "!\n";
        std::cout << "Kamu mendapatkan " << enemy.xpReward << " XP dan " << enemy.goldReward << " Gold.\n";
        player.xp += enemy.xpReward;
        player.gold += enemy.goldReward;
        jeda();
        levelUp();
    }
}


Enemy generateEnemy() {
    std::vector<Enemy> enemyList = {
        {"Slime Kecil", 30, 5, 10, 5},
        {"Goblin Pencuri", 50, 8, 20, 15},
        {"Serigala Hutan", 70, 12, 35, 20}
    };

    if (player.level >= 5) {
        enemyList.push_back({"Ogre Brutal", 120, 18, 80, 50});
    }

    // Boss
    if (player.level >= 10) {
        // Peluang 1/5 untuk bertemu bos jika sudah level 10
        if ((rand() % 5) == 0) {
             return {"RAJA GOBLIN", 300, 25, 500, 200};
        }
    }
    
    return enemyList[rand() % enemyList.size()];
}

void visitShop() {
    std::string pilihan;
    while(true) {
        bersihkanLayar();
        garisPemisah();
        std::cout << "--- TOKO ITEM ---\n";
        std::cout << "Selamat datang! Apa yang kamu butuhkan?\n";
        std::cout << "Gold kamu: " << player.gold << "\n";
        garisPemisah();
        std::cout << "[1] Beli Potion (Harga: 25 Gold)\n";
        std::cout << "[2] Kembali ke menu utama\n";

        std::cout << "\n> ";
        std::getline(std::cin, pilihan);
        
        if (pilihan == "1") {
            if (player.gold >= 25) {
                player.gold -= 25;
                player.potions++;
                std::cout << "Kamu berhasil membeli 1 Potion!\n";
            } else {
                std::cout << "Gold kamu tidak cukup!\n";
            }
            jeda();
        } else if (pilihan == "2") {
            std::cout << "Terima kasih sudah mampir!\n";
            jeda();
            break;
        }
    }
}

void createCharacter() {
    bersihkanLayar();
    std::cout << "Siapakah nama ksatria-mu?\n> ";
    std::getline(std::cin, player.name);
    player.level = 1;
    player.maxHp = 100;
    player.hp = 100;
    player.attack = 10;
    player.xp = 0;
    player.xpToNextLevel = 50;
    player.gold = 0;
    player.potions = 3; // Mulai dengan 3 potion
    std::cout << "\nSelamat datang, Ksatria " << player.name << "! Kerajaan Varia menantimu.\n";
    jeda();
}

void explore() {
    bersihkanLayar();
    garisPemisah();
    std::cout << "Kamu berada di Hutan Varia. Apa yang akan kamu lakukan?\n";
    std::cout << "[1] Cari Pertarungan\n";
    std::cout << "[2] Kunjungi Toko\n";
    std::cout << "[3] Tampilkan Status Karakter\n";
    std::cout << "[4] Keluar dari Game\n";
    garisPemisah();
    
    std::string pilihan;
    std::cout << "\n> ";
    std::getline(std::cin, pilihan);
    
    if (pilihan == "1") {
        Enemy enemy = generateEnemy();
        if (enemy.name == "RAJA GOBLIN") {
            std::cout << "Kamu merasakan aura yang sangat kuat... Ini pasti sang Raja Goblin!\n";
            jeda();
        }
        startCombat(enemy);
        if (enemy.name == "RAJA GOBLIN" && player.hp > 0) {
            bersihkanLayar();
            std::cout << "****************************************************\n";
            std::cout << "                 SELAMAT KSATRIA!                   \n";
            std::cout << "****************************************************\n";
            std::cout << "Dengan jatuhnya Raja Goblin, kedamaian kembali ke Kerajaan Varia.\n";
            std::cout << "Namamu, " << player.name << ", akan dikenang sebagai pahlawan legendaris!\n";
            std::cout << "--- TAMAT ---\n";
            isGameRunning = false;
            jeda();
        }
    } else if (pilihan == "2") {
        visitShop();
    } else if (pilihan == "3") {
        displayStats();
        jeda();
    } else if (pilihan == "4") {
        std::cout << "Kamu memutuskan untuk beristirahat. Sampai jumpa lagi, Ksatria!\n";
        isGameRunning = false;
    }
}

int main() {
    srand(time(0)); // Inisialisasi generator angka acak
    std::cout << "=======================================\n";
    std::cout << "||       LEGENDA KSATRIA VARIA       ||\n";
    std::cout << "=======================================\n";
    jeda();
    
    createCharacter();

    while(isGameRunning) {
        explore();
    }
    
    std::cout << "\nTerima kasih telah bermain!\n";

    return 0;
}
