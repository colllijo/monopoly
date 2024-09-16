# **Datenstruktur**

1. **Spiel:**
   - **Name**: Der Name des Spiels.
   - **Spieler**: Eine Liste von Spielern, die am Spiel teilnehmen.
   - **Status**: Der aktuelle Status des Spiels (z.B. "warten", "laufen", "beendet").
   - **Id**: Eine eindeutige Identifikationsnummer des Spiels.
   - **OnTurn**: Der Spieler, der gerade am Zug ist.
   - **Turn**: Der aktuelle Spielzug (fortlaufende Nummer).

2. **Spieler:**
   - **Name**: Der Name des Spielers.
   - **Geld**: Das aktuelle Guthaben des Spielers.
   - **Grundstücke**: Eine Liste der Grundstücke, die der Spieler besitzt.
   - **Position**: Die aktuelle Position des Spielers auf dem Spielfeld.

3. **Brett:**
   - **Felder**: Eine Liste aller Felder auf dem Spielbrett. Diese können Grundstücke, Ereignisse oder Spezialfelder sein.
   - **JOINTable mit Zustand**: Ein Beziehungstabelle, die den Zustand der Felder speichert (z.B. ob ein Grundstück besetzt ist und von wem).

4. **Feld:**
   - **Typ**: Der Typ des Feldes (Grundstück, Ereignis, Spezial).
   - **Name**: Der Name des Feldes (z.B. "Bahnhof", "Schlossallee").
   - **Preis**: Der Kaufpreis des Feldes, falls es ein Grundstück ist.
   - **Art**: Gibt die Art des Grundstücks an (z.B. Wohnhaus, Bahnhof, Werk).
