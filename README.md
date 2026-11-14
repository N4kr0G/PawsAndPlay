# Paws & Play

Uzaktaki gerçek bir otomatik mama kabını oyunla besleyen Unreal Engine 5 prototipi.
Oyuncu sahnede mama taneleri toplayıp veya reklam izleyip **Mama Puanı** kazanır; yeterli puanla
**[F] Mama dök** dediğinde oyun Wi-Fi üzerinden ESP32'ye istek gönderir, servo kapağı açar ve bir
porsiyon mama kaba düşer.

## Proje özeti ve ekip

| Üye | GitHub | Rol |
| --- | --- | --- |
| Gürkan Lale | _kullanıcı adı_ | _rol_ |
| Kaan Efe Çolakoğlu | _kullanıcı adı_ | _rol_ |

## Amaç ve kapsam

- **Hedef kitle:** Sokak hayvanlarına / barınaklara uzaktan katkı vermek isteyen, kısa oturumlarda oynayan mobil/PC kullanıcıları.
- **Problem:** Barınak ve sokak mama noktalarının sürekli doldurulması emek ve maliyet ister; bağışçı katkısının somut sonucu görünmez.
- **Seçilen kapsam:** *Probleme çözüm* + *oyun dışı veri / özel donanım*. Oyundaki eylem (puan harcama) gerçek dünyada bir porsiyon mamaya dönüşür; cihazın durumu (bugünkü porsiyon, bekleme süresi, çevrimiçi/dışı) oyuna geri gelir ve oyuncunun mama dökme imkânını değiştirir.
- **Başarı göstergesi:** Test oyuncusunun yönlendirme olmadan 3 dakika içinde ilk porsiyonu dökebilmesi; cihazın limitler dışında hiç mama dökmemesi.

## Oynanış ve etkileşim

**Çekirdek döngü:** Gez → mama tanesi topla (+1) / reklam izle (+5) → 10 puana ulaş → mama dök → cihaz onaylar → tekrar.

| Tuş | Eylem |
| --- | --- |
| WASD / fare | Hareket / kamera |
| Boşluk | Zıpla |
| E | Reklam izle (+5 puan, 30 sn bekleme) |
| F | 1 porsiyon mama dök (10 puan) |

**Kurallar**
- Puan yalnızca cihaz `200` döndüğünde düşülür; bağlantı hatası, bekleme veya limit durumunda puan korunur.
- Hayvanı aşırı beslemeye karşı limitler **cihazda** uygulanır (porsiyonlar arası 60 sn, günde en fazla 20 porsiyon). Oyun hileli/çok istek gönderse bile cihaz reddeder.

**Veri akışı**
```
UE5 oyun ──POST /feed (X-Feeder-Key)──▶ ESP32 ──▶ servo kapak ──▶ mama kabı
        ◀──GET /status (10 sn'de bir)── {"portionsToday","maxPerDay","cooldownLeft"}
```
**Veri yokken davranış:** `FeederUrl` boşsa oyun **SİMÜLASYON** modunda çalışır (donanımsız demo). URL girili ama cihaz yanıt vermiyorsa HUD'da kırmızı “ÇEVRİMDIŞI” görünür, mama dökme denemeleri puan harcamaz.

## Kurulum ve çalıştırma

**Oyun**
1. Unreal Engine **5.4** + Visual Studio 2022 (veya Build Tools) “C++ ile masaüstü geliştirme”.
2. `PawsAndPlay.uproject` → sağ tık → *Generate Visual Studio project files* → projeyi açınca modül derlenir
   (ya da: `Engine\Build\BatchFiles\Build.bat PawsAndPlayEditor Win64 Development -Project="<yol>\PawsAndPlay.uproject"`).
3. Play'e basın. Varsayılan olarak simülasyon modundadır.

**Donanım (ESP32)**
- Parçalar: ESP32 DevKit, SG90/MG90S servo, mama haznesi + servo ile açılan kapak, 5 V güç.
- Bağlantı: Servo sinyal → **GPIO13**, servo VCC → 5 V, GND → GND (ESP32 ile ortak).
- `Hardware/ESP32Feeder/secrets.example.h` dosyasını `secrets.h` olarak kopyalayıp Wi-Fi ve anahtarı yazın.
- Arduino IDE → *ESP32Servo* kütüphanesini kurun → `ESP32Feeder.ino`'yu yükleyin → Seri monitörde IP'yi görün.
- `OPEN_MS` değerini kalibre edin (1 porsiyonun gramı buna bağlı).

**Oyunu cihaza bağlama** — `Saved/Config/WindowsEditor/Game.ini` (paketli oyunda `Saved/Config/Windows/Game.ini`):
```ini
[/Script/PawsAndPlay.PawsGameMode]
FeederUrl=http://192.168.1.50
FeederKey=secrets.h içindeki FEEDER_KEY
```
Bu dosya repoya girmez; anahtar ve IP depoda tutulmaz.

**Test:** Cihaz açıkken tarayıcıdan `http://<ip>/status`; mama dökme: `curl -X POST -H "X-Feeder-Key: <anahtar>" http://<ip>/feed`.

## Kod haritası

| Dosya | Görev |
| --- | --- |
| `Source/PawsAndPlay/PawsGameMode.*` | Puan, reklam, cihaz HTTP istekleri, mama tanesi doğurma |
| `Source/PawsAndPlay/Kibble.*` | Toplanabilir mama tanesi |
| `Source/PawsAndPlay/PawsPlayerController.*` | E / F tuşları, HUD oluşturma |
| `Source/PawsAndPlay/PawsHUDWidget.*` | C++ ile kurulan arayüz ve reklam ekranı |
| `Hardware/ESP32Feeder/` | ESP32 firmware'i |
| `Config/DefaultGame.ini` | Puan/porsiyon/reklam ayarları |

## Yol haritası

| Aşama | Görev ve çıktı | Sorumlu | Durum |
| --- | --- | --- | --- |
| Araştırma | Fikir, hedef kitle, donanım seçimi (ESP32 + servo) | | Tamamlandı |
| Tasarım | Çekirdek döngü, veri akışı, HUD yerleşimi | | Tamamlandı |
| İlk prototip | C++ oyun döngüsü, simülasyon modu, ESP32 HTTP API | | Sürüyor |
| Üretim ve test | Gerçek kap + servo montajı, porsiyon kalibrasyonu, 3 kullanıcı testi | | Yapılacak |
| Final | Görsel iyileştirme, mobil paket, gerçek reklam SDK'sı, video | | Yapılacak |

## Bilinen sınırlılıklar

- Reklam **simüle** edilir (5 sn sayaç). Mobil sürümde AdMob vb. SDK ile değiştirilecek.
- Oyun ve ESP32 **aynı ağda** olmalı. Gerçekten “uzaktan” erişim için port yönlendirme, tünel (ör. Cloudflare Tunnel) veya cihazın bir sunucuyu yokladığı aracı servis gerekir.
- Puanlar kaydedilmez; oyun kapanınca sıfırlanır.
- ESP32 günlük sayaç gece yarısı değil, açılıştan itibaren 24 saatte bir sıfırlanır.

## Görseller ve testler

_Ekran görüntüleri ve test bulguları eklenecek (en az 3 kullanıcı, anonim kod ile)._

## Kaynaklar ve beyanlar

| İçerik | Kaynak | Lisans |
| --- | --- | --- |
| Third Person şablonu, Starter Content | Epic Games | Unreal Engine EULA |
| ESP32Servo kütüphanesi | Arduino Library Manager | LGPL-2.1 |

**YZ kullanımı:** Claude (Anthropic) — C++ oyun kodu, ESP32 firmware taslağı ve README yapısı. Çıktılar ekip tarafından derlenip test edilerek düzenlenmiştir.
