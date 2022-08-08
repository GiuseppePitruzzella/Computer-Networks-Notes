Spiegazione Server

Note:

-Per il server vengono utilizzate socket non bloccanti.

-Ad ogni processo è assegnato un id.

Per la gestione del multiclient si usa un file di testo, che funge da memoria condivisa tra i processi.
Quando un processo (figlio del server) riceve un messagio scrive sul file condiviso il messaggio e l'id nel seguente formato:
<username>: <messaggio>, id

Gli altri processi leggono il file ed inviano al client, che stanno gestendo, il messaggio, successivamente il processo inserisce 
il suo id nel file, così da "sapere" che lo ha già spedito, quindi quando viene letto il file e viene trovato l'id del processo il messaggio non viene inviato.

visualizzare gli utenti:
Quando un client si logga, il suo username viene registrato in un file (condiviso). Mentre quando si disconnette viene cancellato dal file.

Problemi:

- Nessuna gestione della concorrenza

- Formattazione non perfetta.
