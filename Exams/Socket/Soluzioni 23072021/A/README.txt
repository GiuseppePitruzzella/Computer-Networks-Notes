La memoria condivisa nel server da parte di tutti i client è un file e come tale avrebbe bisogno di gestione di corse cristiche,
tuttavia la fase di registrazione inserisce le informazioni dei client in modo sequenziale con una logica:
- username
- ip
- porta udp
- porta tcp
- file da condividere
- X carattere di terminazione
Per tale motivo, per una esecuzione prima bisogna registrare interamente un client e poi un altro (registrazione fino alla fase di WHO).
Il client dopo aver visto le risorse condivise da tutti i client, può decidere di scaricare un file (esclusivamente in formato .txt).
Il server periodicamente (ogni 1 minuto) invia un messaggio di ping, l'idea è quella di utilizzare un daemon per verificare se tutti i client sono attivi,
questi ultimi nel caso lo fossero rispondono inviando la lista dei file che vogliono condividere (fase di aggiornamento), il server
aggiorna il file di login e nel caso in cui un client non risponda il server lo cancella dalla lista (fase di logout).

*** La fase di verifica che il client è attivo e della relativa fase di aggionamento delle risorse condivise viene fatta da un daemon
(LATO CLIENT) e come tale si avrebbe race conditions su il canale stdin, per tale motivo dopo una fase di registrazione dove si possono inserire tutti i file che
si desidera da terminale, invece le relative modifiche devono essere fatte sul file file.txt come se fosse un nuovo canale di input.
