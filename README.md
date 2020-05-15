# LFPC

Sunt prezente toate features care le-am mentionat in mesaj
 
gramatica arata cam asa

[int, float, string] {identifier} ;  -> declararea variabilelor

{identifier} = {function name}( [identifier]* ); -> call la functii; se controleaza daca functia si variabilele au fost declarate, scrierea corecta, daca coincid tipurile de date si numarul de argumenti

{identifier} = {number} | {string} ; -> atribuirea valorilor la variabile; se controleaza tipul de date si valoarea atribuita(fara numere negative pentru float), scrierea corecta

{identifier} = {math.expresie}; -> se calculeaza valoarea expresiei; se controleaza daca expresia a fost scrisa corecta; nu se controleaza tipurile de date si nu sunt suportate parantezele

function {identifier}( [data type identifier]* ) -> declararea functillor, se controleaza scrierea corecta
Deoarece parserul e recursiv, declararea functiei trebuie sa fie mai sus decit call-ul la ea.
La moment nu se suportu 'body' functiei

La fiecare expresie practic se controleaza scrierea corecta: daca se scapa semnul egal, ';' etc.

input sample_code.txt

output in terminal : in main sunt niste functii care arata in terminal toate variabilele (nume, tip si valoare daca o au) si functiile (nume, tipul si numele argumentului) declarate.
