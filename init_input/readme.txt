Il file domanda.txt è dedicato ai dati della domanda riguardanti tutti gli Hub, escludendo quello nazionale di Pratica di Mare che si occupa solamente della distribuzione sul territorio regionale.
Nello specifico il contenuto è:

------------------------------------------------------------------------------------------------------------------
1  	Numero di Hub coinvolti nell’algoritmo escluso quello nazionale   Il totale della domanda di tutti gli Hub
------------------------------------------------------------------------------------------------------------------
2	Id dell’Hub	Totale della domanda	Richiesta per la prima dose	Richiesta per la seconda dose
------------------------------------------------------------------------------------------------------------------
…	   …			…			   …					…
------------------------------------------------------------------------------------------------------------------
11	Id dell’Hub	Totale della domanda	Richiesta per la prima dose	Richiesta per la seconda dose
------------------------------------------------------------------------------------------------------------------

Il file grafo.txt si riferisce invece ai dati che riguardano la localizzazione spaziale degli Hub necessari per ricavare il grafo, in base al quale si possono calcolare i percorsi e i costi concernenti i mezzi che trasportano i vaccini, che rimarrà sempre lo stesso per ogni istanza di input da calcolare. 
Si fa quindi riferimento a dei nodi che possono essere:
•	Principali: gli Hub delle 10 Asl della regione più l’Hub nazionale di Pratica di Mare;
•	Intermedi: i punti in cui avviene un cambiamento nella tipologia di strada (urbana, extraurbana, autostrada) che implica una diversa velocità media di percorrenza.
Nel file vengono presentati i dati sottoforma di righe, ciascuna delle quali riguarda un percorso tra due nodi principali e/o intermedi.
Il contenuto del file è il seguente:

------------------------------------------------------------------------------------------------------------------
1	Numero di Hub coinvolti nell’algoritmo compreso quello nazionale	Primo Hub (nazionale)
------------------------------------------------------------------------------------------------------------------
2	Hub di partenza		Hub di arrivo		Velocità media in Km/h		Distanza tra i due Hub in Km
------------------------------------------------------------------------------------------------------------------
…	       …		      …				…					…
------------------------------------------------------------------------------------------------------------------
111	Hub di partenza		Hub di arrivo		Velocità media in Km/h		Distanza tra i due Hub in Km
------------------------------------------------------------------------------------------------------------------


Il terzo file, costi_q.txt, contiene i costi che devono essere valutati nel modello e altri input che non rientrano nel contesto degli altri due file appena esposti:

----------------------------------------------------------
1	Costo giornaliero del mezzo
----------------------------------------------------------
2	Costo orario dell’autista in regime ordinario
----------------------------------------------------------
3	Costo orario dell’autista in regime straordinario
----------------------------------------------------------
4	Costo del carburante per Km
----------------------------------------------------------
5	Capacità massima dei mezzi
----------------------------------------------------------
6	Tempo di carico in minuti
----------------------------------------------------------
7	Tempo di scarico in minuti
----------------------------------------------------------
8	Numero di mezzi disponibili
----------------------------------------------------------
9	Numero di autisti disponibili
----------------------------------------------------------

L’ultimo file, rt_altro.txt, contiene gli indici Rt ricavati da dati epidemiologici reali del Lazio, che verranno poi utilizzati nella generazione di un gruppo di istanze di input.
Il contenuto del file è il seguente:

------------------------------------
1	Id Hub		Indice Rt
------------------------------------
…	   …	 	     …
------------------------------------
10	Id Hub		Indice Rt
------------------------------------


