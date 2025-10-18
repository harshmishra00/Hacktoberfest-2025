import os
from PIL import Image

# Tentukan ukuran baru yang Anda inginkan
NEW_WIDTH = 800
# Folder output untuk gambar yang sudah di-resize
OUTPUT_FOLDER = 'resized_images'
# Folder input tempat gambar Anda berada
INPUT_FOLDER = 'original_images'

def resize_images(input_dir, output_dir, target_width):
    # Buat folder output jika belum ada
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # List semua file di folder input
    for filename in os.listdir(input_dir):
        # Cek apakah file adalah gambar
        if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif')):
            input_path = os.path.join(input_dir, filename)
            output_path = os.path.join(output_dir, filename)
            
            try:
                # Buka gambar
                img = Image.open(input_path)
                
                # Hitung tinggi baru dengan rasio yang sama
                width_percent = (target_width / float(img.size[0]))
                target_height = int((float(img.size[1]) * float(width_percent)))
                
                # Resize gambar
                img_resized = img.resize((target_width, target_height), Image.LANCZOS)
                
                # Simpan gambar baru
                img_resized.save(output_path)
                print(f"Berhasil me-resize: {filename}")
                
            except Exception as e:
                print(f"Gagal memproses {filename}: {e}")

# --- Program Utama ---
# Pastikan Anda punya folder 'original_images' berisi beberapa gambar
if not os.path.exists(INPUT_FOLDER):
    os.makedirs(INPUT_FOLDER)
    print(f"Folder '{INPUT_FOLDER}' dibuat. Silakan isi dengan beberapa gambar.")
else:
    print("Mulai memproses gambar...")
    resize_images(INPUT_FOLDER, OUTPUT_FOLDER, NEW_WIDTH)
    print(f"Selesai! Gambar tersimpan di '{OUTPUT_FOLDER}'.")
