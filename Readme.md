# IPK projekt 1

* autor: Tetauer Pavel
* login: xtetau00
### Popis
Jedná se o server v jazyce C++, komunikující prostřednictvím
protokolu HTTP, který poskytuje různé informace o systému. Server naslouchá na zadaném 
portu a podle url vrací požadované informace.

### Překlad

Server lze přeložit pomocí přiloženého souboru makefile, který
vytvoří spustitelný soubor **_hinfosvc_** pomocí příkazu
```
make
```

## Použití

Po spuštění serveru příkazem 
```
./hinfosvc port
```
lze použít následující příkazy:
```
1.) Získání doménového jména 
    curl http://servername:port/hostname
2.) Získání informací o CPU
    curl http://servername:port/cpu-name
3.) Získání aktuální zátěže CPU
    curl http://servername:port/load 
```

## Příklady použití

```
curl http://localhost:12345/hostname
```
**merlin.fit.vutbr.cz**
```
curl http://localhost:12345/cpu-name
```
**Intel(R) Xeon(R) CPU E5-2640 0 @ 2.50GHz**
```
curl http://localhost:12345/load
```
**45.6125%**

