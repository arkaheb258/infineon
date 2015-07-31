#ifndef _PT100_H_
#define _PT100_H_

#define PT100MIN_T	(-10000)	//-100 st. C.
#define PT100MAX_T	(25000)		//+250 st. C.

#define PT100MIN_R	(6000)		//60 Ohm
#define PT100MAX_R	(20000)		//200 Ohm

INTEGER16 GetPt100Temperature(UNSIGNED32 r){
	INTEGER32 t = 0;
	INTEGER32 b = 263;
	INTEGER32 c = -26405;
	if (r < PT100MIN_R) return PT100MIN_T;
	if (r > PT100MAX_R) return PT100MAX_T;
	t = r;
	t *= b;
	t /= 100;
	t += c;
	if (t<PT100MIN_T) { return PT100MIN_T; }
	if (t>PT100MAX_T) { return PT100MAX_T; }
	return (INTEGER16)t;
//	return (r*b)/100 + c;
}

#endif  // ifndef _PT100_H_
