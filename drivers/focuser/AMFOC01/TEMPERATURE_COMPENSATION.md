# AMFOC01 - Temperature Compensation Implementation

## ✅ Nově implementované funkce:

### 📑 **Options Tab - Teplotní kompenzace**

#### **1. Temperature Compensation Mode**
- **Off:** Teplotní kompenzace vypnuta
- **Driver:** Kompenzace řízena v driveru (software)
- **Focuser:** Kompenzace řízena ve focuseru (hardware)

#### **2. Temperature Coefficient**
- **Rozsah:** -999.9 až +999.9 kroků/°C
- **Přesnost:** 0.1 kroků/°C
- **Aplikace:** 
  - V režimu "Driver": Použito pro výpočty v software
  - V režimu "Focuser": Odesláno do focuseru

#### **3. Compensation Settings**
- **Update Period:** 1-3600 sekund (výchozí: 60s)
- **Threshold:** 0.01-10.0°C (výchozí: 0.1°C)

## 🔧 **Technické detaily:**

### **Driver Mode (Software kompenzace):**
```cpp
bool performDriverTempCompensation() {
    // Kontrola intervalu a prahu
    if (now - lastTempCompTime < tempCompPeriod) return;
    if (fabs(tempDiff) < tempCompThreshold) return;
    
    // Výpočet kompenzace
    int32_t steps = tempDiff * tempCoefficient;
    
    // Provedení pohybu
    gotoAbsolutePosition(currentPosition + steps);
}
```

### **Focuser Mode (Hardware kompenzace):**
```cpp
bool enableTempCompensationInFocuser(bool enable) {
    // TODO: Implementovat protokolový příkaz
    // Například: sendCommand(enable ? ":TC1#" : ":TC0#");
}

bool setTempCoefficientInFocuser(double coefficient) {
    // TODO: Implementovat protokolový příkaz  
    // Například: sendCommandWithParam("TF", coeff * 10, 4);
}
```

## 🎛️ **Uživatelské rozhraní:**

### **Main Control Tab:**
- ✅ **Absolute Position** - Slider (0-1,000,000)
- ✅ **Relative Position** - Numeric input
- ✅ **Sync Position** - Sync funkce
- ✅ **Temperature** - Read-only zobrazení

### **Options Tab:**
- ✅ **Temperature Compensation Mode** - 3 možnosti (Off/Driver/Focuser)
- ✅ **Temperature Coefficient** - Nastavení kroků/°C
- ✅ **Compensation Settings** - Perioda a prah

## 📋 **Workflow použití:**

### **1. Driver Mode (Software kompenzace):**
```
1. Nastavit Mode -> "Driver"
2. Nastavit Coefficient -> např. -2.5 kroků/°C
3. Nastavit Period -> např. 30 sekund
4. Nastavit Threshold -> např. 0.05°C
→ Driver automaticky kompenzuje teplotní změny
```

### **2. Focuser Mode (Hardware kompenzace):**
```  
1. Nastavit Mode -> "Focuser"
2. Nastavit Coefficient -> odesláno do focuseru
→ Focuser sám řídí teplotní kompenzaci
```

## 💡 **Příklad použití:**

**Scenario:** Teplota klesla z 20.0°C na 19.5°C (-0.5°C)  
**Coefficient:** -2.5 kroků/°C  
**Výpočet:** -0.5°C × -2.5 = +1.25 ≈ +1 krok  
**Akce:** Focuser se posune o 1 krok ven

## 🔄 **Stav implementace:**
- ✅ **UI komponenty** - Dokončeno
- ✅ **Driver logic** - Dokončeno
- ✅ **Software kompenzace** - Dokončeno
- ⚠️ **Focuser protokol** - Připraveno (TODO)
- ✅ **Kompilace** - Úspěšná
- ✅ **Instalace** - Dokončena

## 📝 **TODO pro focuser protokol:**
Potřebuje se implementovat skutečné příkazy pro:
- Zapnutí/vypnutí kompenzace ve focuseru
- Nastavení koeficientu ve focuseru
- Případně další parametry (perioda, prah)

Driver je připraven k použití s teplotní kompenzací!
