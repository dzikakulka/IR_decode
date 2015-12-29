/*
 * makrau.h
 *
 *  Created on: 21-04-2013
 *       Autor: Miros³aw Kardaœ
 */

#ifndef MAKRAU_H_
#define MAKRAU_H_

// jeœli makra tego typu wystêpuj¹ w kilku plikach mo¿na wyprowadziæ je do jednego np makrau.h
// a nastêpnie importowaæ w ka¿dym potrzebnym pliku nag³ówkowym
// tak aby by³y tylko w jednym miejscu programu

// Makra upraszczaj¹ce dostêp do portów
// *** PORT
#define PORT(x) XPORT(x)
#define XPORT(x) (PORT##x)
// *** PIN
#define PIN(x) XPIN(x)
#define XPIN(x) (PIN##x)
// *** DDR
#define DDR(x) XDDR(x)
#define XDDR(x) (DDR##x)


#endif /* MAKRAU_H_ */
