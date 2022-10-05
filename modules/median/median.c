/**
 * @file   median.c
 * @brief  Median-Filter zur Beseitigung von Datenausreißern
 *	@author	Michael Kreutzer, Grigory Fridman
 * @version 1.0
 * @date	3.12.2012
 */

/* Includes */

#include "median.h"

/* Static module functions (prototypes) */

void median_sort_list(uint32_t *list, uint32_t *srcList, uint16_t length);

/* Public functions */

/**
 * @brief  Medianfilter zur Beseitigung von Datenausreißern.
 *
 * Die Funktion baut einen internen Ring-Puffer mit 9 Elementen auf,
 * dessen Inhalt in eine nach Elementgröße sortierte Liste	übertragen wird.
 * Das mittlere Element dieser Liste nennt man Median - dieses wird von der Funktion
 * als Filterergebnis zurück gegeben.
 * Da Datenausreißer immer am oberen bzw. unteren Ende der sort. Liste stehen werden
 * diese heraus gefiltert.
 * Bsp: 3 - Element Mittelwert vs. Median
 *		- Eingabe:	[125 123 666]
 *		- Mittelwert: (125+123+666)/3 = 305 -> Ausreißer 666 "verzieht" Mittelwert
 *		- Median:	123 - >>125<< - 666 = 125 -> Ausreißer 666 eleminiert!
 *
 *	Anwendungsbeispiel:
 *		- gefilterte Drehzahl = MED_Median(Drehzahl);
 *		-> Datenausreißer in "Drehzahl" werden beseitigt!
 *
 * @param  newElement:	Neuer Datenwert
 * @retval Median-gefilterter Wert.
 */
uint32_t median_get_median(uint32_t newElement)
{
	static uint32_t ringBuffer[MEDIAN_BUFFER_LENGTH] = { 0 };	// mit 0,0,0,... initialisieren
	static uint16_t pos = 0;
	static uint32_t lastMedian = 0;
	uint32_t		median;				// Positionszeiger deklarieren
	uint32_t list[MEDIAN_BUFFER_LENGTH];					// leere Datenstruktur für den sortierten Ring-Puffer

	// 1. neues Element in Ring-Puffer einfügen
	ringBuffer[pos] = newElement;
	pos = (pos+1) % MEDIAN_BUFFER_LENGTH;	// neuge Zeigerposition berechnen

	// 2. Ring-puffer sortieren
	median_sort_list(list, ringBuffer, MEDIAN_BUFFER_LENGTH);

	// 3. Mittleres Element der sortierten Liste (=Median) zurück geben
	median = list[MEDIAN_BUFFER_LENGTH/2];

	// 4. zusätzlich, leichte Glättung via Mittelwert-Filter
	median = (4*lastMedian + 1*median) / 5;
	lastMedian = median;

	return median;
}

/* Static module functions (implementation) */

/**
 * @brief  Sortieralgorithmus zur Sortierung einer ungeordneten Liste.
 * @param  list:	 Zeiger auf Ziel-Liste.
 * @param  srcList: Zeiger auf Quell-Liste.
 * @param	length	 Länge der Liste(n).
 * @retval None
 */
void median_sort_list(uint32_t *list, uint32_t *srcList, uint16_t length)
{
	uint16_t i,j;
	uint32_t tmp;

	// 1. Quell-Liste kopieren
	for(j=0; j<length; j++)
	{
		list[j] = srcList[j];
	}

	// 2. Sortierung durchführen
	for(j=0; j<length; j++)
	{	// Bouble-Sort muss length mal aufgerufen werden,
		// um sicher zu stellen, dass die Liste auch im Worst-Case sortiert ist.

		for(i=0; i<(length-1); i++)
		{
			if( list[i] > list[i+1] )
			{	// Elemente tauschen, falls list[i] > list[i+1]
				tmp = list[i];
				list[i] = list[i+1];
				list[i+1] = tmp;
			}
		}	// Ende for i
	}	// Ende for j
}
