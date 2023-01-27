Image editor
##### Copyright 2022 Elena Dulgheru 314CA (elenadulgheru03@gmail.com)
# README check
    *Structura temei:
    
    1.image_editor.c - contine functionalitatea programului, mai precis implementarea
        comenzilor cerute (LOAD, SELECT, ROTATE, ..)
    
    2. main.h - contine structura principala, matricea de imagini, alaturi de antetul fiecarei functii
    
    3. apply_values.h - contine matricele de filtre folosite pentru operatia APPLY.
	
    
    *Detalii legate de implementare:

    1. In main se afla:
        a)coordonatele corespunzatoare selectiei curente: x1, x2, y1, y2; 
        b) max_value, reprezentand valoarea maxima a unui pixel din imaginea curenta,
        c) buffer-ul pentru citirea comenzilor
        d) matricea de imagini  
	
    2. Scurta decurgere a programului:
        Daca se primeste comanda LOAD atunci se verifica daca matricea
    curenta este plina, daca s-a incarcat deja o imagine in memorie
    se apeleaza functia free_matrix() cu ajutorul careia eliberez
    memoria matricei curente si initializez linia si coloana cu 0.
        Ulterior citesc numele fisierului din care citesc matricea si
    apelez functia load_matrix() care returneaza imaginea nou incarcata
    daca operatia s-a finalizat cu succes sau NULL altfel, functia
    load_matrix() fiind de tipul image**. In functia apelata deschid
    fisierul in modul read text si citesc tipul pozei, dimensiunile ei
    si valoarea maxima care se gaseste in matrice.
        In continuare se vor citi comenzile date, cum ar fi SELECT, SELECT
    ALL, EQUALIZE, HISTOGRAM, APPLY, ROTATE, urmand sa fie executate
    secvential, pana la gasirea comenzii EXIT, ce va duce la eliberarea
    memoriei utilizate si la oprirea programului.
    