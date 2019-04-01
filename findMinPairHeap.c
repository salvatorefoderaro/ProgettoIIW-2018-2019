/* 
Per quanto riguarda la rimozione dalla coda per le dimensioni del buffer, l'idea
potrebbe essere quella di inserire nella coda le singole versioni dell'immagine come
nodi a se stanti.

In questo modo, quando viene effettuata l'estrazione del minimo, viene effettuata la
rimozione della singola versione di un nodo, tra tutte le immagini, quella a bassa priorità.

Va implementata la rimozione dell'elemento dalla coda, nei casi in cui:
    1) Elemento è il primo nella coda
    2) Elemento all'interno della coda
    3) Elemento è l'ultimo nella coda

Per l'eliminazione dell'elemento in testa, va modificato anche il riferimento
nel nodo dell'albero.
Per l'eliminazione dell'elemento in coda, va modificato il riferimento al nodo precedenteò,
che teoricamente è l'eliminazione di un nodo qualsiasi dalla coda.
*/