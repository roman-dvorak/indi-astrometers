# AMFOC01 Driver - Position Polling Implementation

## ✅ Implementované funkce:

### 1. **Pravidelné pollingování pozice**
- **TimerHit()** - Volá se pravidelně podle getCurrentPollingPeriod()
- **updateStatus()** - Čte aktuální pozici ze zařízení pomocí `:GP#`
- **getActualPosition()** - Implementuje protokol `:GP#` pro čtení pozice

### 2. **Sériová komunikace s AMFOC01 protokolem**
- **sendCommand()** - Odesílání příkazů přes sériový port
- **readResponse()** - Čtení odpovědí s timeout 10ms
- **sendAndReceive()** - Kompletní komunikační cyklus
- **hexToUint32()** - Konverze hexadecimálních odpovědí

### 3. **Automatické aktualizace UI**
- Pozice se aktualizuje pouze pokud se změnila
- Teplota se také pravidelně čte pomocí `:GT#`
- INDI properties se automaticky notifikují klientům

### 4. **Protokolové příkazy implementované**
- `:GP#` - Čtení aktuální pozice (XACTUAL)
- `:GT#` - Čtení teploty
- Připraveno pro `:SN#` a `:FG#` příkazy pro pohyb

## 🔧 Technické detaily:

### **Timer Management:**
```cpp
void TimerHit() {
    if (!isConnected()) return;
    updateStatus();                    // Poll device
    SetTimer(getCurrentPollingPeriod()); // Schedule next
}
```

### **Position Polling:**
```cpp
bool updateStatus() {
    uint32_t pos;
    if (getActualPosition(pos)) {      // :GP# command
        if (pos != currentPosition) {   // Only update if changed
            currentPosition = pos;
            FocusAbsPosN[0].value = pos;
            IDSetNumber(&FocusAbsPosNP, nullptr); // Notify clients
        }
    }
    return true;
}
```

### **Communication Protocol:**
```cpp
bool getActualPosition(uint32_t& position) {
    char response[32];
    if (sendAndReceive(":GP#", response, sizeof(response))) {
        position = hexToUint32(response);  // Convert hex to int
        return true;
    }
    return false;
}
```

## 📊 Features Status:
- ✅ **Slider rozsah:** 0-1,000,000 kroků
- ✅ **Autorské informace:** Roman Dvořák <info@astrometers.cz>
- ✅ **Periodické pollingování:** Implementováno
- ✅ **Sériová komunikace:** 9600 baud, 10ms timeout
- ✅ **AMFOC01 protokol:** :GP#, :GT# příkazy
- ✅ **Kompilace:** Úspěšná
- ✅ **Instalace:** /usr/local/bin/indi_amfoc01

## 🚀 Použití:
Driver nyní pravidelně čte aktuální pozici ze zařízení a aktualizuje UI.
Pollingová perioda je řízena INDI frameworkem (standardně každou sekundu).

```bash
# Spuštění driveru
indi_amfoc01 -v

# Driver XML registrace
ls /usr/local/share/indi/indi_amfoc01.xml
```

Driver je připraven k použití s reálným AMFOC01 zařízením!
