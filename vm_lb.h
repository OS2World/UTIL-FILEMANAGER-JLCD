/*
 * VM_LB.H
 * VidMgr Listbox
 *
 * ver 1.0, 05 Sep 1998
 *
 * Public domain by:
 *   Jari Laaksonen
 *   Arkkitehdinkatu 30 A 2
 *   FIN-33720 Tampere
 *   FINLAND
 *
 *   Fidonet : 2:220/855.20
 *   Internet: jla@to.icl.fi
 */

#ifndef __VM_LB_H__
#define __VM_LB_H__

#ifdef __cplusplus
extern "C" {
#endif

int vm_listbox (const char *title, int x1, int y1, int w1, int h1, int def, char *list[]);
void vm_listbox_init (char text, char frame, char hilit);

#ifdef __cplusplus
}
#endif

#endif
