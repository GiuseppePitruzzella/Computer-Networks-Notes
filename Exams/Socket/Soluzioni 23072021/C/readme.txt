il programma si divide in due parti principali che a loro volta sono composte da altri 
piccoli sottosistemi, gli attori principali da lanciare su macchine diverse sono:

-client: si occupa di inviare le credenziali di registrazione, di inviare il comando "WHO"
         che permette di scaricare la lista di utenti connessi e i file che sono disposti 
         a condividere, inoltre, tramite il client è possibile modificare un file "addFile"
         che permette di modificare la lista dei file condivisi in corso d'opera.

         Oltre a queste funzioni il client implementa un demone che rimane in ascolto su 
         una porta statica "DAEMON_PORT", questo demone rimane in ascolto delle richieste
         di ack periodiche inviate dal server per verificare lo stato del client stesso,
         tramite questo demone vengono inviate come messaggio di ack eventuali modifiche 
         della lista dei client condivisi.

         Per quanto riguarda la seconda parte del programma, ogni client implementa al suo 
         interno un server TCP che sta sempre in ascolto di eventuali richieste da parte di 
         altri client per la condivisione di file, questo server si limita ad accettare le 
         richieste, bufferizzare il contenuto del file richiesto e inviarlo. Oltre ad un 
         server TCP, il client implementa un client TCP che viene lanciato ogni volta 
         che si vuole richiedere un file ad un altro client dopo aver ricevuto la lista
         dei client attualmente connessi dal server: questo client seleziona il nome
         del client da contattare, invia il nome del file desiderato e attende che esso venga
         inviato, una volta ricevuto, crea un file con quel nome e lo riempie con il contenuto 
         ricevuto (per semplicità si considerano soltanto file di una riga).


-server: il server si occupa di ricevere le richieste di registrazione (tramite UDP) da parte
         dei client e di mantenere un file "login.txt" di utenti connessi con relative  
         informazioni, inoltre, il server, potrebbe ricevere dai client connessi, il comando 
         "who", in questo caso esso si occuperà di inviare la lista dei client connessi al 
         client richiedente più le informazioni aggiuntive (indirizzo IP, porta TCP, lista dei file).
         Queste informazioni saranno bufferizzate da ogni client richiedente in un file "log.txt".
         Oltre a questa funzione, il server implementa un demone che interroga periodicamente
         i client connessi per verificare il loro stato: i client connessi risponderanno con un ack
         che conterrà le eventuali modifiche da effettuare alla propria lista dei file condivisi, 
         quelli che non forniranno una risposta, saranno disconnessi (eliminati dal file di login),
         per effettuare questa operazione ci serviremo di un file di appoggio "logout.txt", in 
         esso andremo a scrivere tutti i client tranne quello da eliminare per poi ricopiare 
         il contenuto nel file "login.txt".

Per quanto riguarda il funzionamento generale, bisognerà prima lanciare il server fornendo una porta d'ascolto
in seguito sarà possibile lanciare i client fornendo: "ip_server, porta_server, porta_ascolto, porta_TCP",
dopo l'avvio verrà chiesto al client di immettere nome e lista dei file da condividere, in seguito,
dopo questa fase preliminare di registrazione, al client verrà fornita la possibilità di modificare la propria 
lista di file condivisi (tramite file) o di lanciare il comando "who" per ricevere la lista dei client 
connessi: solo dopo la ricezione di quest'ultima, sarà data la possibilità al client di scegliere il client 
da contattare e il file da scaricare. 


OSS: a causa del binding effettuato dal sistema operativo, ad ogni esecuzione sarà necessario cambiare il numero di porta utilizzato
sia per la comunicazione UDP che per la comunicazione statica del demone.