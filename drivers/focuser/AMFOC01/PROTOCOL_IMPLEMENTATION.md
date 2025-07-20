# AMFOC01 Protocol Implementation

Tato implementace obsahuje kompletní podporu pro komunikační protokol AMFOC01 fokusér.

## Implementované funkce

### 1. Sériová komunikace
- **Rychlost:** 9600 baud
- **Timeout:** 10 ms
- **Format:** `:<PŘÍKAZ><PARAMETR>#`

### 2. GET příkazy (čtení)
- `:GP#` - Získat aktuální pozici motoru (XACTUAL)
- `:GN#` - Získat cílovou pozici motoru (XTARGET)  
- `:GT#` - Získat aktuální teplotu
- `:GI#` - Zkontrolovat, zda je motor v pohybu
- `:GD#` - Získat typ zařízení
- `:GV#` - Získat verzi firmware
- `:GH#` - Získat maximální pozici
- `:GC#` - Získat status příkazu

### 3. SET příkazy (nastavení)
- `:SN<hodnota>#` - Nastavit budoucí pozici (FuturePos)
- `:SP<hodnota>#` - Nastavit aktuální pozici motoru (synchronizace)
- `:SD<hodnota>#` - Nastavit typ zařízení a reinicializovat motor
- `:SC<hodnota>#` - Nastavit kalibrační hodnotu
- `:PO<hodnota>#` - Nastavit výstupní port

### 4. Pohybové příkazy
- `:FG#` - Spustit pohyb na budoucí pozici
- `:FQ#` - Zastavit pohyb motoru

### 5. Ostatní příkazy
- `:C##` - Požadavek na měření teploty

## INDI Interface

### Vlastnosti (Properties)
- **ABS_FOCUS_POSITION** - Absolutní pozice (0-65535 kroků)
- **REL_FOCUS_POSITION** - Relativní pozice (-32768 až 32767 kroků)
- **FOCUS_TEMPERATURE** - Teplota (pouze čtení)
- **FOCUS_SPEED** - Rychlost (kompatibilita)
- **FOCUS_MOTION** - Ovládání pohybu (In/Out)
- **FOCUS_ABORT** - Zastavení pohybu
- **DEVICE_INFO** - Informace o zařízení

### Automatické funkce
- **Periodické aktualizace** - Status, pozice a teplota
- **Monitorování pohybu** - Detekce dokončení pohybu
- **Handshake** - Automatické ověření připojení

## Workflow použití

1. **Připojení:**
   ```
   Handshake -> :GD# (ověření typu zařízení)
   ```

2. **Pohyb na absolutní pozici:**
   ```
   :SN<pozice># -> :FG# -> :GI# (dokud != 00#) -> :GP#
   ```

3. **Relativní pohyb:**
   ```
   Vypočítej novou pozici -> použij absolutní pohyb
   ```

4. **Zastavení:**
   ```
   :FQ# (nastaví XTARGET na XACTUAL)
   ```

## Konverze dat
- **Pozice:** 4-místné hexadecimální číslo (0000-FFFF)
- **Parametry SET:** 5-místné hexadecimální číslo (00000-FFFFF)  
- **Teplota:** 4-místné hexadecimální číslo (konverze /100.0)
- **Port:** 3-místné hexadecimální číslo (000-FFF)

## Výsledek
Driver byl úspěšně zkompilován a je připraven k použití s INDI serverem.
Implementace pokrývá všechny příkazy definované v protokolu AMFOC01.

