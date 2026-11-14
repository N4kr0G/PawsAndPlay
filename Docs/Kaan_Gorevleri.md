# Kaan — Donanım Görevleri (Üretim ve test, 16.11–13.12.2026)

Her görev bir commit'e denk gelir. Haftada en az 2 commit hedefi için iki görev yeterli.
Commit atarken: `git add <dosyalar>` → `git kaan-commit -m "mesaj"` → `git push origin HEAD:main`.

## 1. Kartı ağa bağla ve uçları dene

1. `Hardware/ESP32Feeder/secrets.example.h` → `secrets.h` olarak kopyala; Wi-Fi adı, şifre ve `FEEDER_KEY` yaz (`secrets.h` depoya girmez).
2. Arduino IDE: *ESP32 by Espressif* kart paketi + *ESP32Servo* kütüphanesi → `ESP32Feeder.ino`'yu yükle.
3. Seri monitör (115200 baud): `IP: ...` satırındaki adresi not al.
4. Tarayıcıda `http://<ip>/status` → `portionsToday`, `maxPerDay`, `cooldownLeft` görünmeli.
5. Yanlış anahtarla `/feed` → 401; doğru anahtarla → 200; 60 sn dolmadan tekrar → 429.

**Commit:** Deneme sonucunu README yol haritasına yaz — örn. “ESP32 uçlarını gerçek ağda dene ve sonuçları README'ye ekle”.

## 2. Servo bağlantısı ve şema

1. Servo sinyal → GPIO13, VCC → 5 V, GND → ESP32 ile ortak GND. Servo için ayrı 5 V kaynak daha kararlıdır.
2. Bağlantının fotoğrafını çek veya basit bir şema çiz → `Docs/Hardware/baglanti.jpg`.
3. README “Donanımı Kurma” bölümüne görseli ekle.

**Commit:** “Servo bağlantı şemasını README'ye ekle”.

## 3. Porsiyon kalibrasyonu (`OPEN_MS`)

1. Hazneyi doldur, tartıyı hazırla.
2. `OPEN_MS` = 400 / 600 / 800 ile her biri için 5 kez dök, gramları not al.
3. Hedef porsiyona (ör. 15–20 g) en yakın değeri `ESP32Feeder.ino` satır 17'ye yaz.
4. Ölçüm tablosunu README'ye ekle.

**Commit:** “Porsiyon kalibrasyonunu ölç ve OPEN_MS değerini güncelle”.

## 4. Oyunla uçtan uca deneme

1. `Saved/Config/WindowsEditor/Game.ini` dosyasına `[/Script/PawsAndPlay.PawsGameMode]` başlığı altında `FeederUrl=http://<ip>` ve `FeederKey=<anahtar>` yaz.
2. Oyunda sağ üstte cihaz çevrimiçi görünmeli; 10 puanla [F] → servo açılmalı.
3. ESP32'nin fişini çek → oyunda kırmızı “ÇEVRİMDIŞI”, puan harcanmamalı.
4. Sonucu ve varsa sorunları yol haritasına yaz; mümkünse kısa video çek (final için).

**Commit:** “Oyun ile mama kabını uçtan uca dene, yol haritasını güncelle”.

## 5. Mama kabı montajı

1. Hazne + servo ile açılan kapak; mamanın sıkışmadığını kontrol et.
2. Fotoğrafını `Docs/Hardware/` altına koy, README'ye ekle.

**Commit:** “Mama kabı montaj fotoğraflarını ekle”.

---

Firmware ayarları `ESP32Feeder.ino` satır 14–20'de: pin, açılar, `OPEN_MS`, bekleme, günlük limit.
