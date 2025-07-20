# AMFOC01 Enhanced Features Implementation

## 🎛️ Nové funkce implementované:

### 1. **Slider pro absolutní pozici**
- **Změna:** Absolutní pozice je nyní zobrazena jako slider/tahátko místo číselného vstupu
- **Konfigurace:** `IP_RW, 0` (timeout 0 = slider mode)
- **Rozsah:** 0-65535 kroků
- **Použití:** Hladký drag-and-drop pohyb fokusér

### 2. **Sync funkcionalita** 
- **Nová vlastnost:** `FOCUS_SYNC` v Main Control tabu
- **Příkaz:** `:SP<pozice>#` - synchronizuje aktuální pozici motoru
- **Funkce:** `syncPosition(uint32_t position)`
- **Efekt:** Nastaví XACTUAL, XTARGET i FuturePos na stejnou hodnotu
- **UI:** Numerický input s tlačítkem "Set"

### 3. **Temperature Compensation**

#### **Enable/Disable Switch**
- **Vlastnost:** `TEMP_COMPENSATION` v Options tabu
- **Stavy:** Enable/Disable (výchozí: Disable)
- **Funkce:** Zapne/vypne automatickou teplotní kompenzaci

#### **Koeficient kompenzace**
- **Vlastnost:** `TEMP_COEFF` v Options tabu
- **Rozsah:** -999.9 až +999.9 kroků/°C
- **Přesnost:** 0.1 kroků/°C
- **Výchozí:** 0.0 kroků/°C

#### **Nastavení kompenzace**
- **Vlastnost:** `TEMP_SETTINGS` v Options tabu
- **Parametry:**
  - **Update Period:** 1-3600 sekund (výchozí: 60s)
  - **Threshold:** 0.01-10.0°C (výchozí: 0.1°C)

## 🔧 Technické detaily:

### **Temperature Compensation Logic**
```cpp
bool performTemperatureCompensation()
{
    // Kontrola časového intervalu
    if (now - lastTempCompTime < tempCompPeriod) return;
    
    // Kontrola prahu změny teploty
    double tempDiff = currentTemperature - lastTemperature;
    if (fabs(tempDiff) < tempCompThreshold) return;
    
    // Výpočet kompenzace
    int32_t compensationSteps = tempDiff * tempCoefficient;
    
    // Provedení pohybu
    gotoAbsolutePosition(currentPosition + compensationSteps);
}
```

### **Sync Implementation**
```cpp
bool syncPosition(uint32_t position)
{
    // Pošle :SP<position># příkaz
    setCurrentPosition(position);
    
    // Aktualizuje všechny interní proměnné
    currentPosition = targetPosition = futurePosition = position;
    
    // Aktualizuje UI
    FocusAbsPosN[0].value = position;
    IDSetNumber(&FocusAbsPosNP, nullptr);
}
```

## 📋 Nové UI komponenty:

### **Main Control Tab:**
- ✅ **Absolute Position** - Slider (0-65535)
- ✅ **Relative Position** - Numeric (-32768 to 32767)
- ✅ **Sync Position** - Numeric (0-65535) + Set button
- ✅ **Temperature** - Read-only display
- ✅ **Motion controls** - In/Out buttons
- ✅ **Abort** - Stop button

### **Options Tab:**
- ✅ **Temperature Compensation** - Enable/Disable switch
- ✅ **Temperature Coefficient** - Numeric input (steps/°C)
- ✅ **Compensation Settings** - Period & Threshold
- ✅ **Focus Speed** - Compatibility setting

## 🚀 Workflow s novými funkcemi:

### **Základní použití:**
1. **Připojení** → Automatický handshake
2. **Sync** → Nastav aktuální pozici pomocí Sync
3. **Pohyb** → Použij slider pro plynulý pohyb
4. **Monitoring** → Automatické aktualizace pozice a teploty

### **Temperature Compensation:**
1. **Nastavení koeficientu** → např. -2.5 kroků/°C
2. **Konfigurace** → Period: 30s, Threshold: 0.05°C  
3. **Aktivace** → Enable Temperature Compensation
4. **Automatický provoz** → Driver automaticky kompenzuje teplotní změny

### **Příklad teplotní kompenzace:**
```
Současná teplota: 20.0°C
Nová teplota: 19.5°C (rozdíl: -0.5°C)
Koeficient: -2.5 kroků/°C
→ Kompenzace: -0.5 × -2.5 = +1.25 ≈ +1 krok
→ Fokusér se posune o 1 krok ven
```

## ✅ Status implementace:
- 🟢 **Slider absolutní pozice** - Dokončeno
- 🟢 **Sync funkce** - Dokončeno  
- 🟢 **Temperature compensation** - Dokončeno
- 🟢 **Koeficienty a nastavení** - Dokončeno
- 🟢 **Automatické provádění** - Dokončeno
- 🟢 **Kompilace a testování** - Úspěšné

Driver je nyní připraven s všemi požadovanými funkcemi!
