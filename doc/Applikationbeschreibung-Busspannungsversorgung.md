# Applikationsbeschreibung Busspannungsversorgung

Die Applikation Busspannungsversorgung erlaubt eine Parametrisierung von Diagnose-Parametern mit der ETS.

## **Verwendete Module**

Die Busspannungsversorgung verwendet weitere OpenKNX-Module, die alle ihre eigene Dokumentation besitzen. Im Folgenden werden die Module und die Verweise auf deren Dokumentation aufgelistet.

### **OpenKNX**

Dies ist eine Seite mit allgemeinen Parametern, die unter [Applikationsbeschreibung-Common](https://github.com/OpenKNX/OGM-Common/blob/v1/doc/Applikationsbeschreibung-Common.md) beschrieben sind. 

### **Konfigurationstransfer**

Der Konfigurationstransfer erlaubt einen

* Export von Konfigurationen von OpenKNX-Modulen und deren Kanälen
* Import von Konfigurationen von OpenKNX-Modulen und deren Kanälen
* Kopieren der Konfiguration von einem OpenKNX-Modulkanal auf einen anderen
* Zurücksetzen der Konfiguration eines OpenKNX-Modulkanals auf Standardwerte

Die Funktionen vom Konfigurationstranfer-Modul sind unter [Applikationsbeschreibung-ConfigTransfer](https://github.com/OpenKNX/OFM-ConfigTransfer/blob/v1/doc/Applikationsbeschreibung-ConfigTransfer.md) beschrieben.

### **Logiken**

Wie die meisten OpenKNX-Applikationen enthält auch die Schaltaktor-Applikation ein Logikmodul.

Die Funktionen des Logikmoduls sind unter [Applikationsbeschreibung-Logik](https://github.com/OpenKNX/OFM-LogicModule/blob/v1/doc/Applikationsbeschreibung-Logik.md) beschrieben.

## **Busspannungsversorgung**

<!-- DOC HelpContext="Dokumentation" -->
Mit diesem Modul können u. A. Diagnose-Parametern der Busspannungsversorgung parametrisiert werden.

### **Allgemein**

#### **Einstellungen**

Hier können alle überpreifenden Einstellungen vorgenommen werden.

<!-- DOC -->
##### **Resetzeit**

Die Spannungsversorgung besitzt an der Front eine Schaltfläche, um einen Bus-Reset auszulösen. Alternativ kann dieser auch per KO getriggered werden.

An dieser Stelle hier kann die Resetzeit festgelegt werden, also wie viele Sekunden die Busspannung während eines Resets abgeschaltet bleiben soll.

#### **Ausgaben**

Die verschiedenen Ausgabemöglichkeiten können im Folgenden aktiviert werden.

<!-- DOC -->
##### **Status Netzteil senden**

Legen Sie hier fest, ob der Status der beiden Netzteile ausgegeben werden soll.

<!-- DOC -->
##### **Busspannung senden**

Legen Sie hier fest, ob die Busspannung ausgegeben werden soll.

<!-- DOC -->
##### **Busstrom senden**

Legen Sie hier fest, ob der Busstrom ausgegeben werden soll.

<!-- DOC -->
##### **Busauslastung senden**

Legen Sie hier fest, ob die Busauslastung ausgegeben werden soll.

<!-- DOC -->
##### **Hilfsspannung senden**

Legen Sie hier fest, ob die Hilfsspannung ausgegeben werden soll.

<!-- DOC -->
##### **Hilfsstrom senden**

Legen Sie hier fest, ob der Hilfsstrom ausgegeben werden soll.

<!-- DOC -->
##### **Temperatur senden**

Legen Sie hier fest, ob die Temperatur ausgegeben werden soll.

<!-- DOC -->
###### **Mindeständerung relativ**

Hier kann eine relative Mindeständerung festgelegt werden, bevor die Ausgabe erneut erfolgt.

<!-- DOC -->
###### **Mindeständerung absolut**

Hier kann eine absolute Mindeständerung festgelegt werden, bevor die Ausgabe erneut erfolgt.

<!-- DOC -->
###### **Status zyklisch senden**

Der Status kann bei Bedarf auch zyklisch gesendet werden.
