#  Assembly of chromosome-scale contigs by efficiently resolving repetitive sequences with long reads
Postupak sastavljanja složenih genoma može dati fragmentiran rezultat zbog velikog broja ponavljajućih sekvenci koje otežavaju proces poravnanja. Naš zadatak bio je pokušati međusobno povezati 
dobivene fragmente - contige u cijeli genom. Algoritam koji smo pri tome koristili zasniva se na konstruiranju grafa preklapanja te pronalaženja optimalnih staza među preklapanjima. Pronađena
optimalna staza će nam služiti da povežemo dva contig-a.


## Upute za pokretanje
**Ulazni podaci:**
- Skup već sastavljenih contig-a
- Skup očitanja
- Preklapanja između contig-a i očitanja u PAF formatu
- Međusobna preklapanja očitanja u PAF formatu

**Izlazni podaci:**
- Poboljšani skup sastavljenih contiga u FASTA formatu

Skupovi očitanja i već sastavljenih contiga su pripremljeni kao testni podaci. Preklapanja treba dobiti 
pomoću alata Minimap2 (https://github.com/lh3/minimap2), koristeći opciju:
```bash
   ./minimap2 -x ava-pb contigs.fasta reads.fasta > overlaps1.paf
   
   ./minimap2 -x ava-pb reads.fasta reads.fasta > overlaps2.paf
```   
Prije pokretanja programa potrebno je izgraditi projekt naredbom **make**. Program mora primati 2 argumenta, 2 paf datoteke koje smo dobili pomoću alata Minimap gdje je datoteka s preklapanjima contiga i očitanja prva.
Program ima opcije mijenjanja uvjeta prema kojem stvaramo graf, za overlap score, extension score te MonteCarlo score. Ukoliko nije zadano, program će konstruirati graf prema overlap score-u.
```bash
    ./graphContigs overlaps1.paf overlaps2.paf
```
```bash
    ./graphContigs overlaps1.paf overlaps2.paf -o --overlapScore
```
```bash
    ./graphContigs overlaps1.paf overlaps2.paf -e --extensionScore
```
```bash
    ./graphContigs overlaps1.paf overlaps2.paf -m --monteCarlo
``` 


## Izlazni podaci i evaluacija

Poboljšani skup sastavljenih contiga će biti spremljen u datoteci **output.fasta**. Testiranje na sintetskim podacima i usporedba s referencom moguća je pomoću alata Gepard koji je dostupan na
http://cube.univie.ac.at/gepard.

## Podaci
Unutar git repozitorija se nalazi mapa **data** u kojoj su 2 PAF datoteke, **contigs_reads.paf** i **reads_reads.paf** koje su već dobivene pomoću alata Minimap2 na kojima se može testirati program.


 


